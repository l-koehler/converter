// Needed for basic logic and interaction with files
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not supported!
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)

#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>


#include "functions.h"

// Declare various functions used in convert.cpp
std::string getCurrentFilePath() {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(buffer);
    } else {
        std::cerr << "Error getting the executable path." << std::endl;
        return "";
        }
    }

void show_error(const std::string& title , const std::string& text , const std::string& info , const bool is_error, const bool use_gui) {
        if (use_gui) {
            QMessageBox error_box;
            if (is_error) {
                error_box.setIcon(QMessageBox::Critical);
            } else {
                error_box.setIcon(QMessageBox::Information);
            }
            error_box.setWindowTitle(QString::fromStdString(title));
            error_box.setText(QString::fromStdString(text));
            error_box.setInformativeText(QString::fromStdString(info));
            error_box.setStandardButtons(QMessageBox::Ok);
            error_box.exec();
        } else {
            std::cerr << title << " " << text << std::endl;
        }
    }

std::string getFolder(const std::string& path) {
     size_t pos = path.find_last_of("\\/");
     return (std::string::npos == pos)
         ? ""
         : path.substr(0, pos);
}

std::string readNthLine(const std::string& filePath, int n) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    std::string line;
    int lineCount = 1; // 1-based indexing

    while (std::getline(file, line)) {
        if (lineCount == n) {
            file.close();
            return line;
        }
        lineCount++;
    }

    file.close();
    std::cerr << "Line " << n << " not found in file: " << filePath << std::endl;
    return "";
}

std::string getExtension(const std::string& fullPath) {
    size_t dotPosition = fullPath.find_last_of('.');
    if (dotPosition != std::string::npos) {
        // Extract the substring after the dot, e.g. "txt"
        return fullPath.substr(dotPosition + 1);
    }
    // If there is no dot or the dot is at the end of the string (e.g., "file."), return an empty string
    return "";
}

std::string removeExtension(const std::string& filePath) {
    size_t lastDotIndex = filePath.find_last_of(".");
    if (lastDotIndex != std::string::npos) {
        return filePath.substr(0, lastDotIndex);
    }
    return filePath;
}

int execSystem(const std::string& command) {
    // TODO: Inherently and unavoidably unsafe.
    // Call the binary directly instead.
    // Use system() to run the command and get the return code
    int returnCode = system(command.c_str());

    // Extract the exit code from the return value
    if (returnCode == -1) {
        // An error occurred while trying to execute the command
        return -1;
    } else {
        // Extract the exit code (the lower 8 bits) from the return value
        return returnCode & 0xFF;
    }
}

std::string getBasename(const std::string &filename) {
    if (filename.empty()) {
        return {};
    }
    auto len = filename.length();
    auto index = filename.find_last_of("/\\");
    
    if (index == std::string::npos) {
        return filename;
    }
    if (index + 1 >= len) {
        len--;
        index = filename.substr(0, len).find_last_of("/\\");
        if (len == 0) {
            return filename;
        }
        if (index == 0) {
            return filename.substr(1, len - 1);
        }
        if (index == std::string::npos) {
            return filename.substr(0, len);
        }
        return filename.substr(index + 1, len - index - 1);
    }
    return filename.substr(index + 1, len - index);
}

std::vector<std::string> getPossibleOutput(const std::string& input_file) {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    std::string filepath;

    if (len != -1) {
        buffer[len] = '\0';
        filepath = (buffer);
    } else {
        std::cerr << "Error getting the executable path." << std::endl;
        std::exit(-1);
    }
    std::filesystem::path pathObj(filepath);
    std::string directorypath = pathObj.parent_path().string();
    std::string typefilepath = directorypath + "/supported_types";
    std::string supported_in_types;
    std::string supported_out_types;
    std::string input_file_ext = getExtension(input_file);
    std::vector<std::string> optionsVector;
    for (const auto& entry : std::filesystem::directory_iterator(typefilepath)) {
        // Check if the current entry is a regular file
        if (entry.is_regular_file()) {
            // Get the file path
            std::string file_path = entry.path().string();
            // currently iterating over typefiles, filename is the current file name.
            supported_in_types = readNthLine(file_path, 2);
            supported_out_types = readNthLine(file_path, 3);
            if (supported_in_types.find(" "+input_file_ext) != std::string::npos || supported_in_types.find(input_file_ext+" ") != std::string::npos) {
                //std::cout << "Found available Converter: " << readNthLine(file_path, 1) << std::endl;
                std::istringstream iss(supported_out_types);
                std::string item;
                // Split the string and store unique elements in optionsVector
                while (iss >> item) {
                    if (std::find(optionsVector.begin(), optionsVector.end(), item) == optionsVector.end()) {
                        optionsVector.push_back(item);
                    }
                }
            }
        }
    }
    return optionsVector;
}

std::string getConverter(const std::string& input_file, const std::string& output_file) {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    std::string filepath;

    if (len != -1) {
        buffer[len] = '\0';
        filepath = (buffer);
    } else {
        std::cerr << "Error getting the executable path." << std::endl;
        std::exit(-1);
    }
    // Extract the directory path
    std::filesystem::path pathObj(filepath);
    std::string directorypath = pathObj.parent_path().string();
    std::string typefilepath = directorypath + "/supported_types";
    std::string supported_in_types;
    std::string supported_out_types;
    std::string output_file_ext = getExtension(output_file);
    std::string input_file_ext = getExtension(input_file);
    std::string converter = "";

    for (const auto& entry : std::filesystem::directory_iterator(typefilepath)) {
        // Check if the current entry is a regular file
        if (entry.is_regular_file()) {
            // Get the file path
            std::string file_path = entry.path().string();
            // currently iterating over typefiles, filename is the current file name.
            supported_in_types = readNthLine(file_path, 2);
            supported_out_types = readNthLine(file_path, 3);
            if (     supported_in_types.find(" "+input_file_ext)  != std::string::npos ||  supported_in_types.find( input_file_ext+" ") != std::string::npos) {
                if (supported_out_types.find(" "+output_file_ext) != std::string::npos || supported_out_types.find(output_file_ext+" ") != std::string::npos) {
                    converter = readNthLine(file_path, 1);
                }
            }
        }
    }
    return converter;
}


void help() {
    std::cout << "Available Arguments:"
    << "\n-i (--input): Pass a file as input. In console mode you will be prompted to enter a path if this is missing"
    << "\n-o (--output): Pass a file path to write the output to. Like with input, you will be prompted if missing"
    << "\n-c (--console): When passed, no graphical interface will start, instead the console will be used"
    << "\n-h (--help): Display this help, then exit" << std::endl;
    std::exit(0);
}