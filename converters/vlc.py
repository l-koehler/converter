#!/bin/python3
import os
try:
    import vlc
except ImportError:
    print("Error: The 'vlc' package is not installed. Install it using PIP by running this in the Terminal: 'python3 -m pip install python-vlc'")

def convert(input_file_path, output_file_path):
    # Get the extensions
    input_file_extension = input_file_path.split('.')[-1]
    output_file_extension = output_file_path.split('.')[-1]

    # Create an instance of VLC
    player = vlc.MediaPlayer()

    # Convert the file
    media = vlc.Media(input_file_path)
    options = f"--sout=#transcode{{vcodec=theo,vb=800}}:std{{access=file,mux=ogg,dst='{output_file_path}'}}"
    player.set_mrl(media.get_mrl(), options)
    print("Conversion started. This might take a while, depending on your Hardware.")

    # Wait for the conversion to complete
    player.play()
    while player.get_state() not in [vlc.State.Ended, vlc.State.Error]:
        continue
    player.release()
