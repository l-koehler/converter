#!/bin/python3
import subprocess

def convert(input_file_path, output_file_path, force_default_codec=False):

    ffmpeg_command = ['ffmpeg', '-i', input_file_path, output_file_path]

    custom_codec_used = False

    if not force_default_codec:
        if output_file_path.split('.')[-1] == 'avi':
            custom_codec_used = True
            ffmpeg_command.append('-c:v libx264')
            ffmpeg_command.append('-q:v 3')

    try:
        # Run FFmpeg using subprocess
        subprocess.run(ffmpeg_command, check=True)
        print("File conversion completed successfully.")
    except subprocess.CalledProcessError as e:
        if custom_codec_used == True:
            try:
                print("Usage of custom codec/bitrate failed, falling back to FFmpeg default settings.")
                return convert(input_file_path, output_file_path, force_default_codec=True)
            except Exception as e:
                print(f"An error occurred during file conversion: {e}")
                print("It already failed using the custom codec, the above error occured using the FFmpeg defaults.")
                return False
        print(f"File conversion failed: {e}")
        return False
    except FileNotFoundError:
        print("FFmpeg executable not found. Make sure FFmpeg is installed and accessible in the system PATH.")
        return False
    except Exception as e:
        print(f"An error occurred during file conversion: {e}")
        return False
    return True
