#!/bin/python3
import os, subprocess

def get_unoconv_format(extension):
    format_map = {
        # 'extension': 'format name',
        # text formats
        'bib': 'bib',
        'xml': 'docbook',
        'ltx': 'latex',
        'doc': 'doc', # TODO: could also be doc6 or doc95 format, add check
        'odt': 'odt',
        'txt': 'txt',
        'rtf': 'rtf',
        'sdw': 'sdw',
        # image formats
        'eps': 'eps',
        'emf': 'emf',
        'gif': 'gif',
        'jpg': 'jpg',
        'odd': 'odd',
        'png': 'png',
        'svg': 'svg',
        'tiff': 'tiff',
        'bmp': 'bmp',
        # calc format
        'csv': 'csv',
        'xls': 'xls', # TODO: could also be xls95 format, add check
        'ods': 'ods',
        'sdc': 'sdc',
        # slideshow format
        'swf': 'sfw',
        'odp': 'odp',
        'ppt': 'ppt',
        'sda': 'sda',
        # ambiguous formats
        'xhtml': 'xhtml',
        'xml': 'xhtml',
        'html': 'html',
        'pdf': 'pdf'
    }
    return format_map.get(extension.lower())

def convert(input_file_path, output_file_path):
    try:
        # Get the output format based on the output_file_path extension
        output_extension = output_file_path.split('.')[-1]
        unoconv_format = get_unoconv_format(output_extension)

        if unoconv_format is None:
            print(f"Unsupported output file extension: {output_extension}")
            return

        # Prepare the unoconv command
        unoconv_command = ['unoconv', '-f', unoconv_format, '-o', output_file_path, input_file_path]

        # Run the unoconv command
        subprocess.run(unoconv_command, check=True)

        print(f"File converted successfully. Output saved at: {output_file_path}")
    except subprocess.CalledProcessError as e:
        print(f"Error occurred during conversion: {e}")
        return False
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return False

    return True
