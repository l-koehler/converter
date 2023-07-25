#!/bin/python3
import os, subprocess

def convert(input_file_path, output_file_path):
    # Extract the extensions from the input and output file paths
    original_extension = os.path.splitext(input_file_path)[-1]
    target_extension = os.path.splitext(output_file_path)[-1]

    # Set up the VLC command for video and audio conversion with caching disabled
    vlc_command = [
        "vlc",
        "--no-video",  # Disable video output
        "--intf", "dummy",  # Dummy interface to prevent GUI
        "--sout-keep", "--no-sout-all",  # Disable caching
        "--sout=#transcode{vcodec=theo,vb=800,acodec=vorb}:std{access=file,mux=ogg,dst='" + output_file_path + "'}",
        input_file_path
    ]

    # Start the video conversion process and redirect standard streams
    with subprocess.Popen(vlc_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE) as process:
        stdout, stderr = process.communicate()
