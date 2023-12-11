#### What is this:  
  
This program wraps several converters (ffmpeg, pandoc, unoconv) to be able  
to covert most common files into whatever other file type.  
You can use either a Qt GUI or the terminal to convert files,  
or you can add the program to the Dolphin file manager.  
  
#### Compatibility:  
  
* This should work fully on all GNU/Linux distributions with a GUI.  
* On GNU/Linux without a GUI, you will need to either modify the source or  
  install Qt to compile the program, then you will be able to use the  
  Console-only mode by passing it -c or --console.  
* Windows not supported.  
  
#### How to use:  
  
* If you want to use ffmpeg (Video/Audio support), run  
  ```python3 ./get_ffmpeg_types.py```. This script will check what  
  file types are supported by your ffmpeg installation.  
* Compile the program using ./compile.sh.  
* Move the resulting ./convert binary and the ./supported_types folder  
   to somewhere where you can keep and execute these files, commonly  
  $HOME/.local/bin/converter.  
* If you want to be able to just run ```convert``` in the terminal, add the  
  folder with the binary to the $PATH
* If you want dolphin integration, also move ./add_to_dolphin.sh to the folder  
  with the binary. Then run ```sudo ./add_to_dolphin.sh``` to install and  
  uninstall.  
  
#### Dependencies:  
  
Listed in ./dependencies.md. Most are optional, but you will run into trouble  
compiling without Qt.  
  
#### How to add own converter:
  
Example for converting .pdf and .bat files into .AppImage and .h files:  
Add a file to ./supported_types.  
  
* Line 1 of this file must be ```!command```.  
  Replace command with an actual command on your $PATH.  
  This can be any executable that takes argv[1] as a path to the input file  
  and argv[2] as a output path. It should exit with code 0 on successful  
  conversion and with something else otherwise.  
* Line 2 must be a space-separated list of allowed input formats,  
  in this example: ``` pdf bat```. Note: The line must start with a space.  
* Line 3 is like line 2, just for output formats: ``` appimage h```  
  
It must be possible to convert any input format into any output format.  
If this is not possible, use multiple files.  
