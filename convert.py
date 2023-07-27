#!/bin/python3
# Command line argument 1 is the input file path, 2 is the output file path.
import sys, os, ast, importlib

input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

input_file_type = input_file_path.split('.')[-1]
output_file_type = output_file_path.split('.')[-1]

# Check if both input and output type are mentioned in any file in ./supported_types

def check_type_support(input_file_type, output_file_type):

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

                if input_file_type in input_types and output_file_type in output_types:
                    # the file states conversion is possible using the converter in line 0
                    return type_file.splitlines()[0], full_path
        else:
            pass # recursive search disabled
#            try:
#                return check_type_support(input_file_type, output_file_type, full_path)
#            except "no_converter_found":
#                pass'
    return "no_converter_found",""

converter_path, type_file_path = check_type_support(input_file_type, output_file_type)
if converter_path == "no_converter_found":
    print("Conversion from {} to {} not possible. You can add your own converter as described in ./README.md".format(input_file_type, output_file_type))
    sys.exit(-1)

# we can now assume that converter_path is a path to a python file that allows this conversion.
converter_directory, converter_file = os.path.split(converter_path)
converter_directory = os.path.dirname(os.path.abspath(__file__)) + "/converters/"

sys.path.append(os.path.abspath(converter_directory)) # add the folder the converter is in as importable, we can now import modules (converters) in there.

converter = importlib.import_module(converter_file.split('.')[0]) # import the converter

converter_success = converter.convert(input_file_path, output_file_path)

if converter_success == True:
    print("Success! File converted.")
else:
    print("The converter was called successfully, but returned an error.\nConverter File: {}\nType File: {}".format(converter_path, type_file_path))
