IMPORTANT: DO NOT USE TO OVERWRITE FILES. This program isnt tested. I just occasionally run it to see if it crashes or not, but I did not try every conversion.

What does this do?

This is a program made to convert files from any format into any
other format. For now, it supports various formats I added in
./supported_types. Any format mentioned on the second line of a file
there can be converted into any format on the third line.

Features:

Almost all Audio/Video formats are supported thanks to FFmpeg.

Most common Document/Image/Slideshow/Spreadsheet formats are supported
thanks to unoconv.

Most common markup language formats are supported thanks to pandoc.

The converter can integrate with Dolphin, allowing you to convert files
in the context menu.

How to add a Converter:

1: Create a new file in ./supported_types (your typefile). It can be
named anything.

2: Create a new program (your converter). It can also
be named anything. It can be anything, as long as it is executable
by simply calling it over the command line. A language that should work
for most purposes here is python.

3: In Line 1 of your typefile, put a path to your converter. This should
be a relative path. Relative paths start from the converter (./), not from
./supported_types! Examples:
file/python3 ./converters/myconverter.py    -- Use the converter ./converters/myconverter.py with python3
file/./converter.exe                        -- Use the converter 'converter.exe' in the same directory as the compiled main converter.
It will be fed into the terminal directly with the command line args (see step 6), so you need to make sure your program works when called that way.

4: In Line 2 of your typefile, put a list of formats your
converter takes as input, separated by spaces.

5: In Line 3 of your typefile, put a list of formats your
converter can return as output, separated by spaces. It must be possible to convert any format in
line 2 to any format in line 3! Use multiple files if that is not
possible.

6: Your converter file needs to read the first and second command line argument.
These will be the input file path and the desiredoutput file path. Example for python:
import sys
input_file_path = sys.argv[1]
output_file_path = sys.argv[2]

7: In your converter file, ensure that the program exits with either code
0 (in case of success) or -1 (in case of failure).

If you added formats the software does not cover yet, please make a pull
request so I can add the converter!
