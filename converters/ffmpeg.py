#!/bin/python3
import subprocess

def convert(input_file_path, output_file_path):

    ffmpeg_command = ['ffmpeg', '-i', input_file_path, output_file_path,]

    if output_file_path.split('.')[-1] == 'avi':
        ffmpeg_command.append('-q:4')

    try:
        # Run FFmpeg using subprocess
        subprocess.run(ffmpeg_command, check=True)
        print("File conversion completed successfully.")
    except subprocess.CalledProcessError as e:
        print(f"File conversion failed: {e}")
        return False
    except FileNotFoundError:
        print("FFmpeg executable not found. Make sure FFmpeg is installed and accessible in the system PATH.")
        return False
    except Exception as e:
        print(f"An error occurred during file conversion: {e}")
        return False
    return True
