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
#include <sys/wait.h>

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

using namespace std;

// Declare various functions used in convert.cpp
string getCurrentFilePath() {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return string(buffer);
    } else {
        cerr << "Error getting the executable path." << endl;
        return "";
        }
    }

void show_error(const string& title , const string& text , const string& info , const bool is_error, const bool use_gui) {
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
            cerr << title << " " << text << endl;
        }
    }

string getFolder(const string& path) {
     size_t pos = path.find_last_of("\\/");
     return (string::npos == pos)
         ? ""
         : path.substr(0, pos);
}

string readNthLine(const string& filePath, int n) {
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return "";
    }

    string line;
    int lineCount = 1; // 1-based indexing

    while (getline(file, line)) {
        if (lineCount == n) {
            file.close();
            return line;
        }
        lineCount++;
    }

    file.close();
    cerr << "Line " << n << " not found in file: " << filePath << endl;
    return "";
}

string getExtension(const string& fullPath) {
    size_t dotPosition = fullPath.find_last_of('.');
    if (dotPosition != string::npos) {
        // Extract the substring after the dot, e.g. "txt"
        return fullPath.substr(dotPosition + 1);
    }
    // If there is no dot or the dot is at the end of the string (e.g., "file."), return an empty string
    return "";
}

string removeExtension(const string& filePath) {
    size_t lastDotIndex = filePath.find_last_of(".");
    if (lastDotIndex != string::npos) {
        return filePath.substr(0, lastDotIndex);
    }
    return filePath;
}

int execSystem(const string& command) {
    // TODO: Inherently and unavoidably unsafe.
    // TODO: Remove
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

int execvpString(const vector<string>& cmdline) {
    // convert vector<string>& to vector<const char*>
    vector<const char*> argv;
    for ( const auto& s : cmdline ) {
        argv.push_back( s.data() );
    }
    argv.push_back(NULL);
    argv.shrink_to_fit();
    // fork, exec and wait, return exit code
    int status;
    int pid = fork();
    if (!pid) {
        execvp(cmdline[0].c_str(), const_cast<char* const *>(argv.data()));
    }
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        int exit_status = WEXITSTATUS(status);
        return exit_status;
    }
    return -1;
}

string getBasename(const string &filename) {
    if (filename.empty()) {
        return {};
    }
    auto len = filename.length();
    auto index = filename.find_last_of("/\\");
    
    if (index == string::npos) {
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
        if (index == string::npos) {
            return filename.substr(0, len);
        }
        return filename.substr(index + 1, len - index - 1);
    }
    return filename.substr(index + 1, len - index);
}

vector<string> getPossibleOutput(const string& input_file) {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    string filepath;

    if (len != -1) {
        buffer[len] = '\0';
        filepath = (buffer);
    } else {
        cerr << "Error getting the executable path." << endl;
        exit(-1);
    }
    filesystem::path pathObj(filepath);
    string directorypath = pathObj.parent_path().string();
    string typefilepath = directorypath + "/supported_types";
    string supported_in_types;
    string supported_out_types;
    string input_file_ext = getExtension(input_file);
    vector<string> optionsVector;
    for (const auto& entry : filesystem::directory_iterator(typefilepath)) {
        // Check if the current entry is a regular file
        if (entry.is_regular_file()) {
            // Get the file path
            string file_path = entry.path().string();
            // currently iterating over typefiles, filename is the current file name.
            supported_in_types = readNthLine(file_path, 2);
            supported_out_types = readNthLine(file_path, 3);
            if (supported_in_types.find(" "+input_file_ext) != string::npos || supported_in_types.find(input_file_ext+" ") != string::npos) {
                //cout << "Found available Converter: " << readNthLine(file_path, 1) << endl;
                istringstream iss(supported_out_types);
                string item;
                // Split the string and store unique elements in optionsVector
                while (iss >> item) {
                    if (find(optionsVector.begin(), optionsVector.end(), item) == optionsVector.end()) {
                        optionsVector.push_back(item);
                    }
                }
            }
        }
    }
    return optionsVector;
}

string getConverter(const string& input_file, const string& output_file) {
    char buffer[PATH_MAX];
    ssize_t len = GetCurrentPath(buffer, sizeof(buffer) - 1);
    string filepath;

    if (len != -1) {
        buffer[len] = '\0';
        filepath = (buffer);
    } else {
        cerr << "Error getting the executable path." << endl;
        exit(-1);
    }
    // Extract the directory path
    filesystem::path pathObj(filepath);
    string directorypath = pathObj.parent_path().string();
    string typefilepath = directorypath + "/supported_types";
    string supported_in_types;
    string supported_out_types;
    string output_file_ext = getExtension(output_file);
    string input_file_ext = getExtension(input_file);
    string converter = "";

    for (const auto& entry : filesystem::directory_iterator(typefilepath)) {
        // Check if the current entry is a regular file
        if (entry.is_regular_file()) {
            // Get the file path
            string file_path = entry.path().string();
            // currently iterating over typefiles, filename is the current file name.
            supported_in_types = readNthLine(file_path, 2);
            supported_out_types = readNthLine(file_path, 3);
            if (     supported_in_types.find(" "+input_file_ext)  != string::npos ||  supported_in_types.find( input_file_ext+" ") != string::npos) {
                if (supported_out_types.find(" "+output_file_ext) != string::npos || supported_out_types.find(output_file_ext+" ") != string::npos) {
                    converter = readNthLine(file_path, 1);
                }
            }
        }
    }
    return converter;
}


void help() {
    cout << "Available Arguments:"
    << "\n-i (--input): Pass a file as input. In console mode you will be prompted to enter a path if this is missing"
    << "\n-o (--output): Pass a file path to write the output to. Like with input, you will be prompted if missing"
    << "\n-c (--console): When passed, no graphical interface will start, instead the console will be used"
    << "\n-h (--help): Display this help, then exit" << endl;
    exit(0);
}