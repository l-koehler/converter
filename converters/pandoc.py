#!/bin/python3
import pypandoc

def convert(input_file_path, output_file_path):
    input_file_format = input_file_path.split('.')[-1]
    output_file_format = output_file_path.split('.')[-1]
    try:
        pypandoc.convert_file(input_file_path, output_file_format, format=input_file_format, outputfile=output_file_path)
        print(f"File converted from {input_file_format} to {output_file_format} successfully.")
    except Exception as e:
        print(f"Conversion failed: {e}")
        return False
    return True
