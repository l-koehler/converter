#!/bin/python3

import subprocess, shutil, sys, re

def command_available(argv):
    try:
        subprocess.check_output(argv)
        return True
    except subprocess.CalledProcessError:
        """
        This only means the command returned a non-zero exit code.
        Can be ignored, we only check for availability of the command.
        """
        return True
    except FileNotFoundError:
        return False
"""
Checking for FFmpeg
Needed for most audio and video files.
"""
if command_available(['ffmpeg', '-version']):
    print("FFmpeg is available.")
    # FFmpeg is assumed to be functional now.
    # Run 'ffmpeg -formats' and store the output
    completed_process = subprocess.run(['ffmpeg', '-formats'], capture_output=True, text=True)
    # Check if the command was successful
    if completed_process.returncode == 0:
        ffmpeg_formats = completed_process.stdout
        def parse_formats(input_str):
            demux_formats = []
            mux_formats = []
            # Define the regular expression pattern to match lines starting with 'D' or 'E' and capture the first "word"
            pattern = r'^\s*(DE|D|E)\s+(\S+)'
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
        # sometimes, types are merged like "mp4, mp3" in the output of the above function. This would break the detection of supported types.
        def unmerge(input_list):
            unmerged = []
            for possible_format in input_list:
                if ',' in possible_format:
                    subformats = possible_format.split(',')
                    for subform in subformats:
                        unmerged.append(subform)
                else:
                    unmerged.append(possible_format)
            return unmerged
        demux_formats, mux_formats = parse_formats(ffmpeg_formats)
        demux_formats = unmerge(demux_formats)
        mux_formats = unmerge(mux_formats)
        with open("./supported_types/ffmpeg_types.txt", "w") as file1:
            # Line 1 lists input types, Line 2 lists output types
            write_demux = ""
            write_mux = ""
            for format in demux_formats:
                write_demux = write_demux + " " + format
            for format in mux_formats:
                write_mux = write_mux + " " + format
            ffmpeg_types_content = f"ffmpeg\n{write_demux} \n{write_mux} "
            file1.write(ffmpeg_types_content)
    else:
        print(f"Error: FFmpeg command returned non-zero exit status {completed_process.returncode}")
else:
    print("FFmpeg is not installed/not on the $PATH. It is required for most Audio and Video files.")
    with open("./supported_types/ffmpeg_types.txt", "w") as file:
        file.write("ffmpeg\n \n ")

"""        (with tar)(with ar       )(with squashfs-tools)
file from: (tar/tgz )(ar/deb/o/a/so )(squashfs/snap      )
       to: (tar/tgz )(ar            )(squashfs           )
packaging software is far out-of-scope for now
"""
pack_str = " "
unpack_str = " "

# TAR
if command_available(['tar', '--version']):
    print("tar is available.")
    pack_str += " tar tgz"
    unpack_str += " tar tgz"
else:
    print("tar is not installed/not on the $PATH. It is required for handling .tar/.tgz files. It also is a coreutil, how do you even use your system without it?")
# SQUASHFS
if command_available(['mksquashfs', '-version']):
    print("squashfs-tools is available.")
    pack_str += " squashfs"
    unpack_str += " snap sqfs squashfs"
else:
    print("squashfs-tools is not installed/not on the $PATH. It is required for handling .snap/.squashfs files.")
# AR
if command_available(['ar', '-V']):
    print("ar is available.")
    pack_str += " ar"
    unpack_str += " ar deb a o so"
else:
    print("ar is not installed/not on the $PATH. It is required for handling .deb/.ar/.a files.")

with open("./supported_types/compression_types.txt", "w") as file:
    file.write(f"compressed\n{unpack_str} \n{pack_str} ")

"""
Checking for soffice (Libreoffice)
Some images, most office formats
"""
if command_available(['soffice', '--version']):
    print("LibreOffice is available.")
    """
    No clue how to get LibreOffice to display its supported
    Conversions, so this will save some premade lists. TODO.
    """
    def write_formats_to_file(filename, formats, converter='soffice'):
        in_formats  = ' '.join(formats[0])
        out_formats = ' '.join(formats[1])
        with open(filename, "w") as file:
            file.write(f"{converter}\n {in_formats} \n {out_formats} ")

    calc  = [['csv', 'xls', 'xml', 'xlsx', 'ods', 'sdc'],
             ['csv', 'html', 'xls', 'xml', 'ods', 'sdc', 'xhtml']]
    img   = [['eps', 'emf', 'gif', 'jpg', 'odd', 'png', 'tiff', 'bmp', 'webp'],
             ['eps', 'emf', 'gif', 'html', 'jpg', 'odd', 'pdf', 'png', 'svg', 'tiff', 'bmp', 'xhtml', 'webp']]
    slide = [['odp', 'ppt', 'pptx', 'sda'],
             ['eps', 'gif', 'html', 'swf', 'odp', 'ppt', 'pdf', 'svg', 'sda', 'xml']]
    text  = [['xml', 'html', 'doc', 'docx', 'odt', 'txt', 'rtf', 'sdw'],
             ['bib', 'xml', 'html', 'ltx', 'doc', 'odt', 'txt', 'pdf', 'rtf', 'sdw']]
    write_formats_to_file('./supported_types/soffice_calc.txt', calc)
    write_formats_to_file('./supported_types/soffice_img.txt', img)
    write_formats_to_file('./supported_types/soffice_slide.txt', slide)
    write_formats_to_file('./supported_types/soffice_text.txt', text)
else:
    print("LibreOffice is not installed/not on the $PATH. It is required for most Office and some Image files.")

