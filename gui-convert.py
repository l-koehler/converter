import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox
import sys, os, ast, subprocess, importlib

def on_confirm_click():
    input_file_path = entry_input_file.get()
    output_file_path = entry_output_file.get()

    input_file_type = input_file_path.split('.')[-1]
    output_file_type = output_file_path.split('.')[-1]

    # Check if both input and output type are mentioned in any file in ./supported_types

    folder_path = os.path.dirname(os.path.abspath(__file__)) + "/supported_types"
    folder_path = os.path.abspath(folder_path)

    entries = os.listdir(folder_path)
    converter_path, type_file_path = "no_converter_found",""
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
                    # the file states conversion is possible using the converter in line 0
                    converter_path, type_file_path =  type_file.splitlines()[0], full_path

    if converter_path == "no_converter_found":
        messagebox.showerror("Error", f"Conversion from {input_file_type} to {output_file_type} not possible. You can add your own converter as described in ./README.md")
        sys.exit(-1)

    # we can now assume a path to a python file that allows this conversion. import it.
    converter_directory, converter_file = os.path.split(converter_path)
    converter_directory = os.path.dirname(os.path.abspath(__file__)) + "/converters/"

    sys.path.append(converter_directory)
    converter = importlib.import_module(converter_file.split('.')[0])

    converter_success = converter.convert(input_file_path, output_file_path)

    if converter_success == True:
        print("Success! File converted.")
    else:
        messagebox.showerror("Error", f"The converter was called successfully, but returned an error.\nConverter File: {converter_path}\nType File: {type_file_path}")

def browse_input_file():
    filename = filedialog.askopenfilename()
    entry_input_file.delete(0, tk.END)
    entry_input_file.insert(tk.END, filename)

def browse_output_file():
    filename = filedialog.asksaveasfilename()
    entry_output_file.delete(0, tk.END)
    entry_output_file.insert(tk.END, filename)

def update_dropdown_menu(options_list):
    dropdown['values'] = options_list
    dropdown.set(options_list[0])

def generate_options(path="argv", no_autofill=False):
    options = ["File Type not supported!"]
    if path == "argv":
        input_file_type = sys.argv[1].split('.')[-1]
    else:
        input_file_type = path.split('.')[-1]

    folder_path = os.path.dirname(os.path.abspath(__file__)) + "/supported_types"
    folder_path = os.path.abspath(folder_path)
    entries = os.listdir(folder_path)

    for entry in entries:
        # Construct the full path for the entry
        full_path = os.path.join(folder_path, entry)

        # Check if the entry is a file
        if os.path.isfile(full_path):

            with open(full_path, 'r') as file:
                type_file = file.read()
                input_types = ast.literal_eval(type_file.splitlines()[1]) # split file into lines, pick line 1 (0-based index) and convert it into a list (from a string representation of one)
                output_types = ast.literal_eval(type_file.splitlines()[2])

                if input_file_type in input_types:
                    if options == ["File Type not supported!"]:
                        options = []
                    for output_file_type in output_types:
                        if output_file_type not in options:
                            options.append(output_file_type)
        else:
            pass # recursive search disabled
    if no_autofill:
        return options
    update_dropdown_menu(options)

def on_dropdown_change(*args):
    root_path, file_extension = os.path.splitext(entry_input_file.get())
    file_path = root_path + "." + dropdown_var.get()
    entry_output_file.delete(0, tk.END)
    entry_output_file.insert(tk.END, file_path)

# Create the main tkinter window
root = tk.Tk()
root.title("File Processing App")

# Create input and output file labels and entry boxes
label_input_file = tk.Label(root, text="Input File:")
label_input_file.grid(row=0, column=0, padx=5, pady=5)
entry_input_file = tk.Entry(root, width=50)
entry_input_file.grid(row=0, column=1, columnspan=3, padx=5, pady=5)
entry_input_file.insert(tk.END, sys.argv[1])  # Set sys.argv[1] as default for Input File

label_output_file = tk.Label(root, text="Output File:")
label_output_file.grid(row=1, column=0, padx=5, pady=5)
entry_output_file = tk.Entry(root, width=50)
entry_output_file.grid(row=1, column=1, columnspan=3, padx=5, pady=5)

# Generate a dropdown menu
options = generate_options(no_autofill=True)
dropdown_var = tk.StringVar(root)
dropdown_var.set(options[0])
dropdown_var.trace("w", on_dropdown_change)  # Attach the on_dropdown_change function to the dropdown_var
dropdown = ttk.Combobox(root, textvariable=dropdown_var, state='readonly')
dropdown.grid(row=2, column=0, columnspan=4, padx=5, pady=5)
update_dropdown_menu(options)  # Initialize the dropdown menu with the options list

# Create the Confirm button
confirm_button = tk.Button(root, text="Confirm", command=on_confirm_click)
confirm_button.grid(row=3, column=0, columnspan=4, padx=5, pady=10)

# Create Browse buttons for input and output files
browse_input_button = tk.Button(root, text="Browse", command=browse_input_file)
browse_input_button.grid(row=0, column=4, padx=5, pady=5)
browse_output_button = tk.Button(root, text="Browse", command=browse_output_file)
browse_output_button.grid(row=1, column=4, padx=5, pady=5)

# Start the tkinter main loop
root.mainloop()
