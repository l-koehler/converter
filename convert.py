#!/bin/python3

import sys, os, ast, importlib
# Check if --nogui was passed
if "--nogui" in sys.argv:
    # Do stuff and then exit! If just left running, it will open the GUI.
    commandline_args = sys.argv
    commandline_args.remove("--nogui")
    if len(commandline_args) >= 2:
        input_file_path = commandline_args[1]
        if len(commandline_args) >= 3:
            output_file_path = commandline_args[2]
        else:
            output_file_path = input("Enter the output path: ")
    else:
        input_file_path = input("Enter the input path: ")
        output_file_path = input("Enter the output path: ")
    # input_path and output_path are now set.
    # search for a converter supporting the conversion
    input_file_type = input_file_path.split('.')[-1]
    output_file_type = output_file_path.split('.')[-1]

    folder_path = os.path.dirname(os.path.abspath(__file__)) + "/supported_types"
    folder_path = os.path.abspath(folder_path)
    entries = os.listdir(folder_path)

    converter_path = ""
    for entry in entries:
        # Construct the full path for the entry
        full_path = os.path.join(folder_path, entry)
        # Check if the entry is a file
        if os.path.isfile(full_path):
            with open(full_path, 'r') as file:
                type_file = file.read()
                input_types = ast.literal_eval(type_file.splitlines()[1]) # split file into lines, pick line 1 (0-based index) and convert it into a list (from a string representation of one)
                output_types = ast.literal_eval(type_file.splitlines()[2])
                if input_file_type in input_types and output_file_type in output_types:
                    converter_path = type_file.splitlines()[0]
                    break # avoid continuing the loop after already having found a matching converter + preserve full_path for eventual error messages

    if converter_path == "":
        print(f"Conversion from {input_file_type} to {output_file_type} not possible. You can add your own converter as described in ./README.md")
        sys.exit(-1)

    # we can now assume that converter_path is a path to a python file that allows this conversion.
    converter_directory, converter_file = os.path.split(converter_path)
    converter_directory = os.path.dirname(os.path.abspath(__file__)) + "/converters/"

    sys.path.append(os.path.abspath(converter_directory)) # add the folder the converter is in as importable, we can now import modules (converters) in there.
    converter = importlib.import_module(converter_file.split('.')[0]) # import the converter

    converter_success = converter.convert(input_file_path, output_file_path)

    if converter_success == True:
        print("Success! File converted.")
        sys.exit(0)
    else:
        print(f"The converter was called successfully, but returned an error.\nConverter File: {converter_path}\nType File: {full_path}")
        sys.exit(-1)


from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QLineEdit, QPushButton, QVBoxLayout, QHBoxLayout, QFileDialog, QComboBox, QMessageBox

def pre_display(self):
    # The GUI does not display yet, run code to change the contents of GUI textboxes and similar here.
    # use command line args as in/output path if possible
    if len(sys.argv) >= 2:
        self.path_textbox_in.setText(sys.argv[1])
        # manually call set_dropdown_choices. It should be called automatically by textChanged, but that only applies to edits by the user
        self.set_dropdown_choices(sys.argv[1])
        if len(sys.argv) >= 3:
            self.path_textbox_out.setText(sys.argv[2])
            # select the extension in the dropdown menu
            new_option = sys.argv[2].split('.')[-1]
            index = self.options_combobox.findText(new_option)
            if index != -1:
                self.options_combobox.setCurrentIndex(index)
            else:
                self.show_error(text="Type not supported!", info=f"Conversion from \n'{sys.argv[1].split('.')[-1]}' to '{new_option}' not possible!")



    # Already declare the Variable that will store the selected converter, because it is lazily set on listing the file types for the dropdown.
    # We need to be able to detect when it has not been touched at all to then search for the converter.
    global converter_path
    converter_path = ""

class FileConverter(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('File Converter')

        # Text boxes
        self.path_textbox_in = QLineEdit(self)
        self.path_textbox_in.textChanged.connect(self.set_dropdown_choices)
        self.path_textbox_in.textChanged.connect(self.set_output_path)
        self.path_textbox_out = QLineEdit(self)

        # Browse button
        self.browse_button = QPushButton('Browse', self)
        self.browse_button.clicked.connect(self.browse_file)

        # Drop-down menu (ComboBox)
        self.options_combobox = QComboBox(self)
        self.options_combobox.addItems([])
        self.options_combobox.currentTextChanged.connect(self.set_output_path)

        # Confirm button
        self.confirm_button = QPushButton('Confirm', self)
        self.confirm_button.clicked.connect(self.convert)

        # Layout setup
        layout = QVBoxLayout()
        layout.addWidget(QLabel('Select input file:'))
        layout.addWidget(self.path_textbox_in)
        layout.addWidget(self.browse_button)
        layout.addWidget(QLabel('Select desired output location:'))
        layout.addWidget(self.path_textbox_out)
        layout.addWidget(QLabel('Select output format:'))
        layout.addWidget(self.options_combobox)
        layout.addWidget(self.confirm_button)

        self.setLayout(layout)
        self.show()

    def browse_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, 'Select File')
        if file_path:
            self.path_textbox_in.setText(file_path)

    def show_error(self, title="Error", text="An unexpected error occured!", info="No Information given.", is_error=True):
        error_box = QMessageBox()
        if is_error:
            error_box.setIcon(QMessageBox.Critical)
        else:
            error_box.setIcon(QMessageBox.Information)
        error_box.setWindowTitle(title)
        error_box.setText(text)
        error_box.setInformativeText(info)
        error_box.setStandardButtons(QMessageBox.Ok)
        error_box.exec_()

    def set_output_path(self):
        # set output path to input_path with the file extension provided by the dropdown
        input_path = self.path_textbox_in.text()
        output_path = os.path.splitext(input_path)[0] + '.' + self.options_combobox.currentText()
        self.path_textbox_out.setText(output_path)

    def set_dropdown_choices(self, converter_scan=False):
        input_path = self.path_textbox_in.text()
        input_extension = input_path.split('.')[-1]
        output_extension = self.path_textbox_out.text().split('.')[-1]
        # set dropdown choices to all extensions the extension of input_path can be converted into
        typefile_folder = os.path.dirname(os.path.abspath(__file__)) + "/supported_types"
        entries = os.listdir(typefile_folder)
        options = []
        for entry in entries:
            # Construct the full path for the entry
            full_path = os.path.join(typefile_folder, entry)
            # Check if the entry is a file
            if os.path.isfile(full_path):
                with open(full_path, 'r') as file:
                    type_file = file.read()
                    input_types = ast.literal_eval(type_file.splitlines()[1]) # split file into lines, pick line 1 (0-based index) and convert it into a list (from a string representation of one)
                    output_types = ast.literal_eval(type_file.splitlines()[2])

                    if input_extension in input_types:
                        options = list(set(output_types+options)) # merge the lists
                        # Strictly speaking, this part does not belong here. It will store the selected converter and has nothing to do with the function.
                        # But we can possibly avoid looping through all the directorys one extra time.
                        if output_extension in output_types:
                            global converter_path
                            converter_path = type_file.splitlines()[0]
        options.sort()
        self.options_combobox.addItems(options)

    def convert(self):
        input_path = self.path_textbox_in.text()
        output_path = self.path_textbox_out.text()
        input_extension = input_path.split('.')[-1]
        output_extension = output_path.split('.')[-1]
        typefile_folder = os.path.dirname(os.path.abspath(__file__)) + "/supported_types"

        print("Path Input:", input_path)
        print("Path Output:", output_path)
        global converter_path
        if converter_path == "":
            # Global Variable is now set. If it is stlii empty, the conversion is not supported.
            self.set_dropdown_choices(converter_scan=True)
        if converter_path == "": # DO NOT merge with the above condition, converter_path might change!
            self.show_error(text="Conversion failed!", info=f"Conversion from \n'{input_extension}' to '{output_extension}' not possible!\nIf you believe this is wrong, please ensure \n'{typefile_folder}'\n contains the type files.")

        # we can now start using the converter
        converter_file = os.path.split(converter_path)[-1]
        converter_directory = os.path.dirname(os.path.abspath(__file__)) + "/converters/"

        sys.path.append(os.path.abspath(converter_directory)) # add the folder the converter is in as importable, we can now import modules (converters) in there.
        converter = importlib.import_module(converter_file.split('.')[0]) # import the converter

        converter_success = converter.convert(input_path, output_path)

        if converter_success == True:
            print("Success! File converted.")
            self.show_error(title="Success!", text="The file has been converted.", info="", is_error=False)
        else:
            self.show_error(text="Conversion failed!", info=f"The converter was called successfully,\nbut returned an error. Converter File:\n{converter_path}")



if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = FileConverter()
    # add any code you want to run on start of gui mode to pre_display()
    pre_display(window)
    sys.exit(app.exec_())

