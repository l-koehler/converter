#!/bin/python3
import os, subprocess

def get_encoder_for_format(file_format):
    encoders = {
        'mp4': ' H.264 MKV 1080p30',
        'webm': 'VP9 MKV 1080p30',
        'avi': 'MPEG-4 Part 2 (Xvid)',
        'mkv': 'H.265 MKV 1080p30',
        'ts': 'H.264 MKV 1080p30',
        '3gp': ''
        # Add more formats and their corresponding encoders as needed.
        # If you add a encoder to improve compatibility/performance, please share it on github.
    }

    return encoders.get(file_format.lower(), 'CLI Default')

def convert(input_file_path, output_file_path):
    try:
        output_file_extension = output_file_path.split('.')[-1]
        encoder = get_encoder_for_format(output_file_extension)

        # TODO: That is terrible. Especially for a FOSS program with a native Linux Package. This needs to be replaced. Sorry for the low performance here too.
        os.system("wine /home/lorenz/Applications/Convert/dependencies/HandBrakeCLI.exe -i '{}' -o '{}' --preset '{}'".format(input_file_path, output_file_path, encoder))

    except subprocess.CalledProcessError as e:
        print(f"Conversion failed: {e}")
