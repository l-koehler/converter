#!/bin/python3

import subprocess, shutil, sys, re

# FFMPEG stuff
def is_ffmpeg_installed():
	try:
		# Run the 'ffmpeg' command with the '-version' flag
		# This will output the version information if FFmpeg is installed
		subprocess.check_output(['ffmpeg', '-version'])
		return True
	except FileNotFoundError:
		return False
if is_ffmpeg_installed():
	print("FFmpeg is installed. Generating list of supported types...")
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
		def unmerge(input_list):
			# sometimes, types are merged like "mp4, mp3" in the output of the above function. This would break the detection of supported types.
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
	print("FFmpeg is not installed/not on the $PATH. It is required for Audio and Video conversion.")
	with open("./supported_types/ffmpeg_types.txt", "w") as file:
		file.write("ffmpeg\n \n ")

"""        (always )(with ar)(with squashfs-tools)
file from: (tar/tgz)(ar/deb )(snap/squashfs      )
       to: (tar/tgz)(ar     )(squashfs           )
packaging software is far out-of-scope for now
"""
def is_squashfs_installed():
	try:
		# Run mk- and unsquashfs commands with the '-v' flag
		# This will output the version information if squashfs-tools is installed
		subprocess.check_output(['unsquashfs', '-v'])
		subprocess.check_output(['mksquashfs', '-v'])
		return True
	except FileNotFoundError: # only this indicates the program isnt installed.
		return False
	except: # all other errors can be anything else.
		return True
def is_ar_installed():
    try:
        subprocess.check_output(['ar', '-V'])
        ar_present = True
    except FileNotFoundError:
        ar_present = False

pack_str = " tar tgz"
unpack_str = " tar tgz"
if is_squashfs_installed():
    pack_str += " squashfs"
    unpack_str += " snap sqfs squashfs"
else:
    print("squashfs-tools is not installed/not on the $PATH. It is required for handling .snap/.squashfs files.")
# if is_ar_installed():
pack_str += " ar"
unpack_str += " deb ar a"
# else:
#     print("ar is not installed/not on the $PATH. It is required for handling .deb/.ar/.a files.")

with open("./supported_types/compression_types.txt", "w") as file:
    file.write(f"compressed\n{unpack_str} \n{pack_str} ")