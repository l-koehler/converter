// Needed for basic logic and interaction with files
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not supported!
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <dirent.h>
#include <cstdlib>

// Needed for the GUI
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

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

// this Class contains the GUI Program
class ConverterApp : public QWidget {
    Q_OBJECT

public:
    // This defines the layout of the GUI, make changes here to change the user interfase.
    ConverterApp(QWidget *parent = nullptr) : QWidget(parent) {
        // Text boxes
        path_textbox_in = new QLineEdit(this);
        connect(path_textbox_in, &QLineEdit::textChanged, this, &ConverterApp::setDropdownChoices);
        connect(path_textbox_in, &QLineEdit::textChanged, this, &ConverterApp::setOutputPath);
        path_textbox_out = new QLineEdit(this);

        // Browse button
        browse_button = new QPushButton("Browse", this);
        connect(browse_button, &QPushButton::clicked, this, &ConverterApp::browseFile);

        // Drop-down menu (ComboBox)
        options_combobox = new QComboBox(this);
        connect(options_combobox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, &ConverterApp::setOutputPath);

        // Confirm button
        confirm_button = new QPushButton("Confirm", this);
        connect(confirm_button, &QPushButton::clicked, this, &ConverterApp::convert);

        // Layout setup
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Select input file:"));
        layout->addWidget(path_textbox_in);
        layout->addWidget(browse_button);
        layout->addWidget(new QLabel("Select desired output location:"));
        layout->addWidget(path_textbox_out);
        layout->addWidget(new QLabel("Select output format:"));
        layout->addWidget(options_combobox);
        layout->addWidget(confirm_button);

        setLayout(layout);
        show();
    }

// Various functions the GUI program uses.
// The --nogui program has those separately, changes here will only affect the GUI Program.
private slots:
    // Returns the absolute path of the executable
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

    // Takes a file path and a number n, returns the line n of the file.
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

    // Takes a file path or name, returns the file extension.
    std::string getExtension(const std::string& fullPath) {
        size_t dotPosition = fullPath.find_last_of('.');
        if (dotPosition != std::string::npos) {
            // Extract the substring after the dot
            return fullPath.substr(dotPosition + 1);
        }
        // If there is no dot or the dot is at the end of the string (e.g., "file."), return an empty string
        return "";
        }


    // Takes a file path or name, returns everything but the file extension.
    std::string removeExtension(const std::string& filePath) {
        size_t dotPosition = filePath.find_last_of(".");
        if (dotPosition != std::string::npos) {
            return filePath.substr(0, dotPosition);
        }
        return filePath;
        }

    // Returns true when word is contained in inputString, returns false otherwise
    bool isWordPresent(const std::string& inputString, const std::string& word) {
        size_t found = inputString.find(word);
        return found != std::string::npos;
        }

    // Executes a String using the system shell (Terminal) and returns the exit Code.
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

    // Shows a Popup Message using QT. Misleading naming: It can also be a regular information popup when is_error = false.
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

    // Gets called each time the Input Text box changes content. Takes the extension and checks for possible output extensions, then lists those in the dropdown menu.
    void setDropdownChoices() {
        std::string textbox_in_content = path_textbox_in->text().toStdString();
        std::string textbox_in_extension = getExtension(textbox_in_content);
        std::string filepath = getCurrentFilePath();
        // Extract the directory path
        std::filesystem::path pathObj(filepath);
        std::string directorypath = pathObj.parent_path().string();
        std::string typefilepath = directorypath + "/supported_types";
        std::vector<std::string> optionsVector;
        std::string supported_in_types;
        std::string supported_out_types;
        for (const auto& entry : std::filesystem::directory_iterator(typefilepath)) {
            // Check if the current entry is a regular file
            if (entry.is_regular_file()) {
                // Get the file path
                std::string file_path = entry.path().string();
                // currently iterating over typefiles, filename is the current file name.
                supported_in_types = readNthLine(file_path, 2);
                supported_out_types = readNthLine(file_path, 3);
                if (isWordPresent(supported_in_types, textbox_in_extension)) {
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
        options_combobox->clear();
        for (const auto& option : optionsVector) {
            options_combobox->addItem(QString::fromStdString(option));
        }
    }

    // Gets called each time the Input Text box or the dropdown menu changes content. WARNING: This might be redundant, one of these triggers might get removed. Updates the path to the output file to reflect
    // the path to the input file, but with the extension selected by the dropdown menu.
    void setOutputPath() {
        std::string textbox_in_content = path_textbox_in->text().toStdString();
        std::string input_path_filename = removeExtension(textbox_in_content);
        std::string textbox_out_content = input_path_filename + "." + options_combobox->currentText().toStdString();
        path_textbox_out->setText(QString::fromStdString(textbox_out_content));
    }

    // Gets called whenever the user clicks on the file chooser button. Sets the selected file to be the input path.
    void browseFile() {
        // Open a file dialog to browse for the input file
        QString file_path = QFileDialog::getOpenFileName(this, "Select Input File", "", "All Files (*.*);;Text Files (*.txt)");
        // Check if a file was selected
        if (!file_path.isEmpty()) {
            // Set the selected file path in the input text box
            path_textbox_in->setText(file_path);
            }
    }

    // Gets called when the User clicks Convert. WARNING: Contains a lot of redundant code, will be optimized/rewritten soon.
    void convert() {
        // Implement the conversion logic.
        std::string input_file = path_textbox_in->text().toStdString();
        std::string output_file = path_textbox_out->text().toStdString();
        std::string filepath = getCurrentFilePath();
        // Extract the directory path
        std::filesystem::path pathObj(filepath);
        std::string directorypath = pathObj.parent_path().string();
        std::string typefilepath = directorypath + "/supported_types";
        std::string supported_in_types;
        std::string supported_out_types;
        std::string output_file_ext = options_combobox->currentText().toStdString();
        std::string input_file_ext = getExtension(input_file);
        std::string converter;
        int exit_code;
        for (const auto& entry : std::filesystem::directory_iterator(typefilepath)) {
            // Check if the current entry is a regular file
            std::cout << "checking out " << entry << "\n";
            if (entry.is_regular_file()) {
                // Get the file path
                std::string file_path = entry.path().string();
                // currently iterating over typefiles, filename is the current file name.
                supported_in_types = readNthLine(file_path, 2);
                supported_out_types = readNthLine(file_path, 3);
                std::cout << "self i/o: " << input_file_ext << " / " << output_file_ext << "\n";
                if (isWordPresent(supported_in_types, input_file_ext) && isWordPresent(supported_out_types, output_file_ext)) {
                    std::cout << "matched " << entry << "\n";
                    converter = readNthLine(file_path, 1);
                }
            }
        }
        if (converter == "") {
            show_error("Error!", "A unexpected Error occured!", "No converter found. Conversion is not possible.", true);
            std::exit(-1);
        }
        // converter is set, input_file is set, output_file is set.
        if (converter == "ffmpeg") {
            // use ffmpeg for conversion
            exit_code = execSystem(std::string("ffmpeg -i \"" + input_file + "\" \"" + output_file + "\""));
            if (exit_code == 0) {
                show_error("Success!", "The file has been converted successfully!", "", false);
            } else {
                show_error("Error!", "A unexpected Error occured!", "FFmpeg was called successfully, but returned a non-zero exit code.", true);
                std::exit(-1);
            }
        } else if (converter == "unoconv") {
            // use unoconv for conversion
            exit_code = execSystem(std::string("unoconv \"" + input_file + "\" \"" + output_file + "\""));
            if (exit_code == 0) {
                show_error("Success!" , "The file has been converted successfully!" , "" , false);
            } else {
                show_error("Error!" , "A unexpected Error occured!" , "unoconv was called successfully, but returned a non-zero exit code. " , true);
                std::exit(-1);
            }
        } else if (converter == "pandoc") {
            // use pandoc for conversion
            exit_code = execSystem(std::string("pandoc \"" + input_file + "\" \"" + output_file + "\""));
            if (exit_code == 0) {
                show_error("Success!" , "The file has been converted successfully!" , "" , false);
            } else {
                show_error("Error!" , "A unexpected Error occured!" , "pandoc was called successfully, but returned a non-zero exit code. " , true);
                std::exit(-1);
            }
        } else if (converter.find("file/") == 0) {
            // use other converter
            converter.erase(0, 5);
            exit_code = execSystem(std::string(converter + " \"" + input_file + "\" \"" + output_file + "\""));
            if (exit_code == 0) {
                show_error("Success!" , "The file has been converted successfully!" , "" , false);
            } else {
                show_error("Error!" , "A unexpected Error occured!" , "The specified converter was called successfully, but returned a non-zero exit code. " , true);
                std::exit(-1);
            }
        } else {
            show_error("Error!" , "A unexpected Error occured!" , "A unknown converter was specified by the type file. Prepend custom converters with 'file/'. Example: file/python3 ~/converter.py. " , true);
            std::exit(-1);
        }
    }

// Set the variables that represent the GUI elements.
private:
    QLineEdit *path_textbox_in;
    QLineEdit *path_textbox_out;
    QPushButton *browse_button;
    QComboBox *options_combobox;
    QPushButton *confirm_button;
};

// Repeat the function definitions from above for the nogui converter. TODO: Make the ones below have a GUI toggle and replace the definitions above with calls to the ones here.
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
bool isWordPresent(const std::string& inputString, const std::string& word) {
    size_t found = inputString.find(word);
    return found != std::string::npos;
    }
std::string removeExtension(const std::string& filePath) {
    size_t lastDotIndex = filePath.find_last_of(".");
    if (lastDotIndex != std::string::npos) {
        return filePath.substr(0, lastDotIndex);
    }
    return filePath;
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

// main function, either converts without GUI or calls the GUI application.
int main(int argc, char *argv[]) {

    // Check if --nogui is present
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--nogui") == 0) {
            std::cout << "No GUI mode enabled." << std::endl;
            std::string input_file, output_file;

            // Iterate through the command-line arguments
            for (int i = 1; i < argc; ++i) {
                // Compare each argument with the "--nogui" flag
                if (strcmp(argv[i], "--nogui") == 0) {
                    // Skip "--nogui" arguments
                    continue;
                }

                // Check if input_file has been assigned, if not, assign the current argument to it
                if (input_file.empty()) {
                    input_file = argv[i];
                } else {
                    // Check if output_file has been assigned, if not, assign the current argument to it
                    if (output_file.empty()) {
                        output_file = argv[i];
                    } else {
                        // If both arguments are already assigned, break the loop (optional)
                        break;
                    }
                }
            }

            if (!input_file.empty()) {
                std::cout << "input_file: " << input_file << std::endl;
            } else {
                std::cout << "Enter value for input_file: ";
                std::cin >> input_file;
            }

            if (!output_file.empty()) {
                std::cout << "output_file: " << output_file << std::endl;
            } else {
                std::cout << "Enter value for output_file: ";
                std::cin >> output_file;
            }
            // input_file and output_file are set now
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
            std::string converter;
            int exit_code;
            for (const auto& entry : std::filesystem::directory_iterator(typefilepath)) {
                // Check if the current entry is a regular file
                //std::cout << "checking out " << entry << "\n";
                if (entry.is_regular_file()) {
                    // Get the file path
                    std::string file_path = entry.path().string();
                    // currently iterating over typefiles, filename is the current file name.
                    supported_in_types = readNthLine(file_path, 2);
                    supported_out_types = readNthLine(file_path, 3);
                    //std::cout << "self i/o: " << input_file_ext << " / " << output_file_ext << "\n";
                    if (isWordPresent(supported_in_types, input_file_ext) && isWordPresent(supported_out_types, output_file_ext)) {
                        //std::cout << "matched " << entry << "\n";
                        converter = readNthLine(file_path, 1);
                    }
                }
            }
            if (converter == "") {
                std::cerr << "Error: No converter found. Conversion is not possible." << std::endl;
                std::exit(-1);
            }
            // converter is set, input_file is set, output_file is set.
            if (converter == "ffmpeg") {
                // use ffmpeg for conversion
                exit_code = execSystem(std::string("ffmpeg -i \"" + input_file + "\" \"" + output_file + "\""));
                if (exit_code == 0) {
                    std::cout << "Success: The file has been converted!";
                } else {
                    std::cerr << "Error: FFmpeg was called successfully, but returned a non-zero exit code." << std::endl;
                    std::exit(-1);
                }
            } else if (converter == "unoconv") {
                // use unoconv for conversion
                exit_code = execSystem(std::string("unoconv \"" + input_file + "\" \"" + output_file + "\""));
                if (exit_code == 0) {
                    std::cout << "Success: The file has been converted!";
                } else {
                    std::cerr << "Error: unoconv was called successfully, but returned a non-zero exit code. " << std::endl;
                    std::exit(-1);
                }
            } else if (converter == "pandoc") {
                // use pandoc for conversion
                exit_code = execSystem(std::string("pandoc \"" + input_file + "\" \"" + output_file + "\""));
                if (exit_code == 0) {
                    std::cout << "Success: The file has been converted!";
                } else {
                    std::cerr << "Error: pandoc was called successfully, but returned a non-zero exit code. " << std::endl;
                    std::exit(-1);
                }
            } else if (converter.find("file/") == 0) {
                // use other converter
                converter.erase(0, 5);
                exit_code = execSystem(std::string(converter + " \"" + input_file + "\" \"" + output_file + "\""));
                if (exit_code == 0) {
                    std::cout << "Success: The file has been converted!";
                } else {
                    std::cerr << "Error: The custom specified Converter was called successfully, but returned a non-zero exit code. " << std::endl;
                    std::exit(-1);
                }
            } else {
                std::cerr << "A unknown converter was specified by the type file. Prepend custom converters with 'file/'. Example: file/python3 ~/converter.py." << std::endl;
                std::exit(-1);
            }
            break;
            return 0;
            }
        }

    // --nogui is not present, use the GUI

    QApplication app(argc, argv);
    ConverterApp converterApp;
    return app.exec();
    }

#include "main.moc"
