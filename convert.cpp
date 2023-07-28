// needed for GUI
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <iostream>
#include <string>
#include <filesystem> // (only works in C++17 and later)
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <dirent.h>
#include <cstdlib>

#ifdef _WIN32
    #include <Windows.h>
    #define GetCurrentPath(buffer, size) GetModuleFileName(NULL, buffer, size)
#elif __linux__ || __APPLE__ || __unix__ || defined(__posix)
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#else
    #warning "Unknown or less common operating system. getCurrentPath() may not work as expected."
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#endif

class ConverterApp : public QWidget {
    Q_OBJECT

public:
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

private slots:
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

    void setDropdownChoices() {
        // Triggered on Input change
        std::string textbox_in_content = path_textbox_in->text().toStdString();
        std::string textbox_in_extension = getExtension(textbox_in_content);
        // Implement logic to set drop-down choices based on input file
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

    void setOutputPath() {
        std::string textbox_in_content = path_textbox_in->text().toStdString();
        std::string input_path_filename = removeExtension(textbox_in_content);
        std::string textbox_out_content = input_path_filename + "." + options_combobox->currentText().toStdString();
        path_textbox_out->setText(QString::fromStdString(textbox_out_content));
    }

    void browseFile() {
    // Open a file dialog to browse for the input file
    QString file_path = QFileDialog::getOpenFileName(this, "Select Input File", "", "All Files (*.*);;Text Files (*.txt)");

    // Check if a file was selected
    if (!file_path.isEmpty()) {
        // Set the selected file path in the input text box
        path_textbox_in->setText(file_path);
        }
    }

    void convert() {
        // Implement the conversion logic.
        std::string input_file = path_textbox_in->text().toStdString();
        std::string output_file = path_textbox_out->text().toStdString();
        std::string filepath = getCurrentFilePath();
        // Extract the directory path
        std::filesystem::path pathObj(filepath);
        std::string directorypath = pathObj.parent_path().string();
        std::string typefilepath = directorypath + "/supported_types";
        std::vector<std::string> optionsVector;
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

private:
    QLineEdit *path_textbox_in;
    QLineEdit *path_textbox_out;
    QPushButton *browse_button;
    QComboBox *options_combobox;
    QPushButton *confirm_button;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ConverterApp converterApp;
    return app.exec();
    }

#include "main.moc"
