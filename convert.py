#!/bin/python3
# Command line argument 1 is the input file path, 2 is the output file path.
import sys

input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

input_file_type = input_file_path.split('.')[-1]
output_file_type = output_file_path.split('.')[-1]

# check if the input type is supported by checking if it is present in line 2 (input) of any file in ./supported_types

