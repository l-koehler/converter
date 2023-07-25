#!/bin/python3

import subprocess, shutil, sys, re

def is_ffmpeg_installed():
    try:
        # Run the 'ffmpeg' command with the '-version' flag
        # This will output the version information if FFmpeg is installed
        subprocess.check_output(['ffmpeg', '-version'])
        return True
    except subprocess.CalledProcessError:
        return False

def install_ffmpeg():
    package_managers = ["apt-get", "dnf", "yum", "pacman", "zypper", "apk", "xbps", "pkgtool"]

    for pm in package_managers:
        if shutil.which(pm):
            try:
                print(f"Installing FFmpeg using {pm}...")
                if pm == "apt-get":
                    subprocess.run([pm, "install", "-y", "ffmpeg"])
                elif pm in ["dnf", "yum"]:
                    subprocess.run([pm, "install", "-y", "ffmpeg"])
                elif pm == "pacman":
                    subprocess.run([pm, "-S", "--noconfirm", "ffmpeg"])
                elif pm == "zypper":
                    subprocess.run([pm, "install", "-y", "ffmpeg"])
                elif pm == "apk":
                    subprocess.run([pm, "add", "--no-cache", "ffmpeg"])
                elif pm == "xbps":
                    subprocess.run([pm, "install", "-y", "ffmpeg"])
                elif pm == "pkgtool":
                    subprocess.run([pm, "installpkg", "ffmpeg"])
                else:
                    print("Unknown Package manager!")
                    return False

                print("FFmpeg has been successfully installed.")
                return True
            except subprocess.CalledProcessError as e:
                print(f"An error occurred while installing FFmpeg using {pm}: {e}")
                return False

    print("No supported package manager found. Cannot install FFmpeg.")
    return False

if is_ffmpeg_installed():
    print("FFmpeg is installed.")
else:
    print("FFmpeg is not installed/not on the $PATH. It is required for Audio and Video conversion.")
    print("If you do not want to install it, you can disable Audio/Video conversion.")
    print("")
    print("[1]: Disable Audio/Video Conversion")
    print("[2]: Attempt to install FFmpeg")
    print("[3]: Exit the converter")
    ffmpeg_err_choice = input("1/2/3: ")
    if ffmpeg_err_choice == "3":
        sys.exit(0)
    if ffmpeg_err_choice == "2":
        if install_ffmpeg() == True:
            if not is_ffmpeg_installed():
                print("The Installer seems to have sucessfully installed FFmpeg, but FFmpeg is still not present on $PATH. Install FFmpeg or restart the program and select 'Disable Audio/Video Conversion'.")
            else:
                print("Sucessfully installed FFmpeg.")
        else:
            print("The Installer failed. Install FFmpeg or restart the program and select 'Disable Audio/Video Conversion'.")
    else:
        with open("./supported_types/ffmpeg_types.txt", "w") as file1:
            # Line 1 lists input types, Line 2 lists output types
            file1.write("./converters/ffmpeg.py\n[]\n[]")
            print("Set FFmpegs supported In/Output types to none. FFmpeg will not be used.")
            sys.exit(0)

# FFmpeg is assumed to be functional now.

# Run 'ffmpeg -formats' and store the output
completed_process = subprocess.run(['ffmpeg', '-formats'], capture_output=True, text=True)

# Check if the command was successful
if completed_process.returncode == 0:
    ffmpeg_formats = completed_process.stdout
else:
    print(f"Error: FFmpeg command returned non-zero exit status {completed_process.returncode}")

def parse_formats(input_str):
    demux_formats = []
    mux_formats = []

    # Define the regular expression pattern to match lines starting with 'D' or 'E' and capture the first "word"
    pattern = r'^\s*(DE|D|E)\s+(\S+)'

    # Split the input string into lines
    lines = input_str.splitlines()

    # Iterate over each line and extract format information
    for line in lines:
        match = re.match(pattern, line)
        if match:
            action, file_extension = match.groups()
            if action == 'D' or action == 'DE':
                demux_formats.append(file_extension)
            if action == 'E' or action == 'DE':
                mux_formats.append(file_extension)

    return demux_formats, mux_formats

demux_formats, mux_formats = parse_formats(ffmpeg_formats)

with open("./supported_types/ffmpeg_types.txt", "w") as file1:
    # Line 1 lists input types, Line 2 lists output types
    ffmpeg_types_content = "./converters/ffmpeg.py\n" + str(demux_formats) + "\n" + str(mux_formats)
    file1.write(ffmpeg_types_content)
