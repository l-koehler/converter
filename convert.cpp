// Needed for basic logic and interaction with files
#include <iostream>
#include <string>
#include <filesystem> // only works in C++17 and later, older versions are not 
                      // supported!
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

#include "functions.h"

// Needed to get the current location of the executable. This is needed to find
// the typefiles.
#ifdef _WIN32
    #include <Windows.h>
    #define GetCurrentPath(buffer, size) GetModuleFileName(NULL, buffer, size)
#elif __linux__ || __APPLE__ || __unix__ || defined(__posix)
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#else
    #warning "Unknown or less common operating system. Assuming UNIX-like."
    #include <unistd.h>
    #include <limits.h>
    #define GetCurrentPath(buffer, size) readlink("/proc/self/exe", buffer, size)
#endif

// this Class contains the GUI Program
class ConverterApp : public QWidget {
    Q_OBJECT

public:
    // This defines the layout of the GUI, make changes here to change the user interfase.
    ConverterApp(const std::string& file_input = "", const std::string& file_output = "", QWidget *parent = nullptr) : QWidget(parent) {
        // Title
        setWindowTitle("File Converter");

        // Text boxes
        path_textbox_in = new QLineEdit(this);
        path_textbox_in->setText(QString::fromStdString(file_input));
        connect(path_textbox_in, &QLineEdit::textChanged, this, &ConverterApp::setDropdownChoices);
        connect(path_textbox_in, &QLineEdit::textChanged, this, &ConverterApp::setOutputPath);
        path_textbox_out = new QLineEdit(this);
        path_textbox_out->setText(QString::fromStdString(file_output));

        // Browse button
        browse_button = new QPushButton("Browse", this);
        connect(browse_button, &QPushButton::clicked, this, &ConverterApp::browseFile);

        // Drop-down menu (ComboBox)
        options_combobox = new QComboBox(this);
        connect(options_combobox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, &ConverterApp::setOutputPath);

        // Confirm button
        confirm_button = new QPushButton("Confirm", this);
        connect(confirm_button, &QPushButton::clicked, this, &ConverterApp::startConversion);

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
    // Gets called each time the Input Text box changes content. Takes the extension and checks for possible output extensions, then lists those in the dropdown menu.
    void setDropdownChoices() {
        std::string input_file = path_textbox_in->text().toStdString();
        std::string output_file = path_textbox_out->text().toStdString();

        std::vector<std::string> optionsVector = getPossibleOutput(input_file);

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

    // Gets called when the User clicks Convert.
    void startConversion() {
        // Implement the conversion logic.
        std::string input_file = path_textbox_in->text().toStdString();
        std::string output_file = path_textbox_out->text().toStdString();

        convert(input_file, output_file, true);
    }

// Set the variables that represent the GUI elements.
private:
    QLineEdit *path_textbox_in;
    QLineEdit *path_textbox_out;
    QPushButton *browse_button;
    QComboBox *options_combobox;
    QPushButton *confirm_button;
};

// main function, either converts without GUI or calls the GUI application.
int main(int argc, char *argv[]) {

    std::string input_file = "";
    std::string output_file = "";

    // Check command line arguments (-i, --input, -o, --output, -h, --help, -c, --console)
    std::vector<std::string> args(argv, argv+argc);
    bool use_gui = true;
    
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--console" or args[i] == "-c") {
            use_gui = false;
        } else if (args[i] == "--input" or args[i] == "-i") {
            output_file = args[i+1];
        } else if (args[i] == "--output" or args[i] == "-o") {
            input_file = args[i+1];
        } else if (args[i] == "--help" or args[i] == "-h") {
            help();
        }
    }
    
    if (use_gui == false) {
        if (input_file == "") {
            std::cout << "Enter a input file path: " << std::endl;
            std::getline(std::cin, input_file);
        }
        if (output_file == "") {
            std::cout << "Enter a output file path: " << std::endl;
            std::getline(std::cin, output_file);
        }
        // Do command line stuff
        convert(input_file, output_file, false);
    } else {
        // Convert with GUI
        if (argc >= 3) {
            QApplication app(argc, argv);
            ConverterApp converterApp(argv[1], argv[2]);
            return app.exec();
        } else if (argc >= 2) {
            QApplication app(argc, argv);
            ConverterApp converterApp(argv[1], "");
            return app.exec();
        } else {
            QApplication app(argc, argv);
            ConverterApp converterApp("", "");
            return app.exec();
        }
    }
    


}
#include "main.moc"
