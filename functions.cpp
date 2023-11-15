// Needed for basic logic and interaction with files
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not supported!
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>

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

// Needed to get the current location of the executable. This is needed to find the typefiles.
// DEPRECATED! All needed files will be embedded in the executable soon!
#ifdef _WIN32
    #include <Windows.h>
    #define GetCurrentPath(buffer, size) GetModuleFileName(NULL, buffer, size)
#elif __linux__ || __APPLE__ || __unix__ || defined(__posix)
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#else
    #warning "Unknown or less common operating system. The Program may not work as expected. Assuming POSIX compliance."
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#endif

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

void show_error(const std::string& title , const std::string& text , const std::string& info , const bool is_error) {
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
    }

std::string readNthLine(const std::string& filePath, int n) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return "";
    }

    std::string line;
    int lineCount = 1; // WARNING: 1-based indexing

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
        // Extract the substring after the dot
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

bool isWordPresent(const std::string& inputString, const std::string& word) {
    size_t found = inputString.find(word);
    return found != std::string::npos;
}

int execSystem(const std::string& command) {
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
            if (isWordPresent(supported_in_types, input_file_ext)) {
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
            if (isWordPresent(supported_in_types, input_file_ext) && isWordPresent(supported_out_types, output_file_ext)) {
            converter = readNthLine(file_path, 1);
            }
        }
    }
        return converter;
}

void convert(const std::string& input_file, const std::string& output_file, const bool& useGUI = false) {

    std::string converter = getConverter(input_file, output_file);

    if (converter == "") {
        if (useGUI == true) {
            show_error("Error!", "A unexpected Error occured!", "No converter found. Conversion is not possible.", true);
        } else {
            std::cerr << "Error: No converter found. Conversion is not possible." << std::endl;
        }
        std::exit(-1);
    }
    // converter is set, input_file is set, output_file is set.
    int exit_code;
    if (converter == "ffmpeg") {
        // use ffmpeg for conversion
        exit_code = execSystem(std::string("ffmpeg -i \"" + input_file + "\" \"" + output_file + "\""));
    } else if (converter == "unoconv") {
        // use unoconv for conversion
        exit_code = execSystem(std::string("unoconv \"" + input_file + "\" \"" + output_file + "\""));
    } else if (converter == "pandoc") {
        // use pandoc for conversion
        exit_code = execSystem(std::string("pandoc \"" + input_file + "\" -o \"" + output_file + "\""));
    } else if (converter.find("file/") == 0) {
        // use other converter
        converter.erase(0, 5);
        exit_code = execSystem(std::string(converter + " \"" + input_file + "\" \"" + output_file + "\""));
    } else {
        show_error("Error!" , "A unexpected Error occured!" , "A unknown converter was specified by the type file. Prepend custom converters with 'file/'. Example: file/python3 ~/converter.py. " , true);
        std::exit(-1);
    }
    // At this point the file has been passed to the converter and it has finished
    // Check if the resulting file is empty
    std::ifstream read(output_file);
    bool isEmpty = read.peek() == EOF;
    
    if (exit_code == 0 and isEmpty == false) {
        if (useGUI == true) {
            // show_error is a stupid name too lazy to change it rn TODO
            show_error("Success!" , "The file has been converted successfully!" , "" , false);
        } else {
            std::cout << "Success: The file has been converted!" << std::endl;
        }
        std::exit(0);
    } else if (exit_code != 0){
        if (useGUI == true) {
                show_error("Error!" , "A unexpected Error occured!" , "The converter was called successfully, but returned a non-zero exit code. " , true);
        } else {
            std::cerr << "Error: The converter was called successfully, but returned a non-zero exit code." << std::endl;
        }
        std::exit(-1);
    } else {
        if (useGUI == true) {
            show_error("Error!" , "A unexpected Error occured!" , "The converter was called successfully and returned 0, but the resulting file is empty. " , true);
        } else {
            std::cerr << "Error: The converter was called successfully and returned 0, but the resulting file is empty." << std::endl;
        }
    }
}

void help() {
    std::cout << "Available Arguments:"
    << "\n-i (--input): Pass a file as input. In console mode you will be prompted to enter a path if this is missing"
    << "\n-o (--output): Pass a file path to write the output to. Likewith input, you will be prompted if missing"
    << "\n-c (--console): When passed, no graphical interface will start, instead the console will be used"
    << "\n-h (--help): Display this help, then exit" << std::endl;
    std::exit(0);
}