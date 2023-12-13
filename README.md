#### What is this:  
  
This program wraps several converters (ffmpeg, pandoc, unoconv) to be able  
to covert most common files into whatever other file type.  
You can use either a Qt GUI or the terminal to convert files,  
or you can add the program to the Dolphin file manager.  
  
#### Compatibility:  
  
* This should work fully on all GNU/Linux distributions with a GUI.  
* On GNU/Linux without a GUI, you will need to either modify the source or  
  install Qt to compile the program, then you will be able to use the  
  Console-only mode by passing it `-c` or `--console`.  
* Windows not supported.  
  
#### How to compile and use:  
  
* Clone this repository on your computer and cd into it.  
* First, run `python3 ./get_types.py`. This script will check  
  what conversions are already supported by packages you have installed  
  and will suggest optional packages to install to convert certain files.  
  If you download any of these packages, re-run the script after.  
* Compile the program using `./compile.sh`.  
* Now move the files `./converter-bin` and `./add_to_dolphin.sh` and the  
  directory `./supported_types` to a folder where you can keep it,  
  for example `/opt/converter/`.  
* You should now add `/opt/converter/` to your $PATH. Do this by adding  
  `export PATH="/opt/converter:$PATH"` to your `~/.bashrc`.  
* If you want dolphin integration, now run  
  `sudo add_to_dolphin`  
  
#### Dependencies:  
  
Listed in ./dependencies.md. Most are optional,  
but Qt and some coreutils are needed.  
  
#### How to add own converter:
  
Example for converting .pdf and .bat files into .AppImage and .h files:  
Add a file to ./supported_types.  
  
* Line 1 of this file must be `!command`.  
  Replace command with an actual command on your $PATH.  
  This can be any executable that takes argv[1] as a path to the input file  
  and argv[2] as a output path. It should exit with code 0 on successful  
  conversion and with something else otherwise.  
* Line 2 must be a space-separated list of allowed input formats,  
  in this example: ` pdf bat `. Note: The line must start/end with a space.  
* Line 3 is like line 2, just for output formats: ` appimage h `  
  
It must be possible to convert any input format into any output format.  
If this is not possible, use multiple files.  

#### Supported converters:

This program does not bundle any converters, but instead uses installed  
packages. This helps keep the size down and the program simple.  
The following packages are being checked for:  

* ffmpeg - Almost all Music/Video files.  
* pandoc - Almost all Markdown files, some office formats.  
* soffice - LibreOffice, almost all office formats, some image formats.  
* squashfs-tools - Needed for .snap and .squashfs/.sqfs files.  
* ar - Old file packing tool, needed for .deb and .a/.ar files.  
* More to be added soon  
