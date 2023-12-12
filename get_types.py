#!/bin/python3

import subprocess, shutil, sys, re

# FFMPEG stuff
def is_ffmpeg_installed():
	try:
		# Run the 'ffmpeg' command with the '-version' flag
		# This will output the version information if FFmpeg is installed
		subprocess.check_output(['ffmpeg', '-version'])
		return True
	except subprocess.CalledProcessError:
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
			ffmpeg_types_content = "ffmpeg\n" + write_demux + "\n" + write_mux
			file1.write(ffmpeg_types_content)
else:
	print("FFmpeg is not installed/not on the $PATH. It is required for Audio and Video conversion.")
	with open("./supported_types/ffmpeg_types.txt", "w") as file:
		file.write("ffmpeg\n \n ")
	print("Setting FFmpegs supported in/output types to none. FFmpeg will not be used.")

# SquashFS stuff (for .snap/.squashfs -> .tar/.zip/.tgz)
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
if is_squashfs_installed():
	print("squashfs-tools is installed. Generating list of supported types...")
	with open("./supported_types/squashfs_decompress.txt", "w") as file:
		file.write("squashfs\n snap squashfs squ sqfs\n tar tgz") # snap/squashfs/squ/sqfs -> tar/tgz
	with open("./supported_types/squashfs_compress.txt", "w") as file:
		file.write("squashfs\n tar tgz \n squashfs") # tar/tgz -> squashfs
else:
	print("squashfs-tools is not installed/not on the $PATH. It is required for extracting .snap files.")
	with open("./supported_types/squashfs_decompress.txt", "w") as file:
		file.write("squashfs\n \n ")
	with open("./supported_types/squashfs_compress.txt", "w") as file:
		file.write("squashfs\n \n ")
	print("Setting squashfs-tools supported in/output types to none. Squashfs will not be used.")