Hello!
This is a python program made to convert files from any format into any other format.
For now, it supports various formats I added in ./supported_types. Any format mentioned on the second line of a file there can be converted into any format on the third line.

Almost all Audio/Video formats are supported thanks to FFmpeg.
Most common Document/Image/Slideshow/Spreadsheet formats are supported thanks to unoconv.

If you want to add your own converter, here is how to do it:

1: Create a new file in ./supported_types (your typefile). It can be named anything.

2: Create a new .py file (your converter) (recommended in ./converters, but can be anywhere). It can also be named anything.

3: In Line 1 of your typefile, put a path to your converter. This can either be an absolute path or a relative path. Relative paths start from ./, not from ./supported_types!

4: In Line 2 of your typefile, put a python list of formats your converter takes as input. Example: \['txt', 'png', 'mp3'\]

5: In Line 3 of your typefile, put a python list of formats your converter takes as output. It must be possible to convertany format in line 2 to any format in line 3! Use multiple files if that is not possible.

6: In your converter.py file, define a function called convert that takes two strings. These will be the input file path and the desired output file path.

7: In your converter.py file, ensure that the function returns either True (for success) or False (in case of failure).

8: If you used this to add a converter that works with file types the software does not cover yet, please make a pull request! I would love to add more formats to this!


I want to add this as a extension to the Dolphin file manager so you can convert files by just right-clicking on them and hope to add this feature soon.
