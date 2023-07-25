#!/bin/python3
# Command line argument 1 is the input file path, 2 is the output file path.
import sys, os, ast, importlib

input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

input_file_type = input_file_path.split('.')[-1]
output_file_type = output_file_path.split('.')[-1]

# Check if both input and output type are mentioned in any file in ./supported_types

def check_type_support(input_file_type, output_file_type):

    folder_path = os.path.abspath("./supported_types")
    entries = os.listdir(folder_path)
    possible_converters = []
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
                    possible_converters.append([type_file.splitlines()[0], full_path])
        else:
            pass
            # TODO: add support for recursive search later
    if possible_converters == []:
        return "Conversion not possible, no installed converter supports this.", ""
    elif len(possible_converters) == 1:
        return possible_converters[0][0], possible_converters[0][1]
    else:
        # we have multiple converters to choose from
        priority_list = ["ffmpeg.py", "vlc_converter.py"]

        # Create a dictionary to store the positions of files in priority_list
        file_positions = {file: index for index, file in enumerate(priority_list)}

        # Define the sorting key function
        def sorting_key(file):
            return (file not in file_positions, file_positions.get(file, float('inf')))

        # Sort Files based on the custom sorting key
        sorted_files = sorted(possible_converters, key=sorting_key)

        return sorted_files[0][0], sorted_files[0][1]

converter_path, type_file_path = check_type_support(input_file_type, output_file_type)
if converter_path == "Conversion not possible, no installed converter supports this.":
    print("Conversion from {} to {} not possible. You can add your own converter as described in ./README.md".format(input_file_type, output_file_type))
    sys.exit(-1)

# we can now assume that converter_path is a path to a python file that allows this conversion.
converter_directory, converter_file = os.path.split(converter_path)

sys.path.append(os.path.abspath(converter_directory)) # add the folder the converter is in as importable, we can now import modules (converters) in there.

converter = importlib.import_module(converter_file.split('.')[0]) # import the converter

converter_success = converter.convert(input_file_path, output_file_path)

if converter_success == True:
    print("Success! File converted.")
else:
    print("The converter was called successfully, but returned an error.\nConverter File: {}\nType File: {}".format(converter_path, type_file_path))
