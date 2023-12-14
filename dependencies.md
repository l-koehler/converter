#### Required dependencies:  

1. Qt  
    LICENSE: For this Software it is under the GPLv3.  
    (https://www.qt.io/faq/tag/qt-open-source-licensing)  
    Is probably already preinstalled. If it is not, use your  
    systems package manager to install it. Needed only for the GUI,  
    but the program will not compile without it.  

2. Python 3  
    (Not needed for running the program, only for generating the list of  
    supported types. You can copy those from the github repo, but then  
    conversions will fail if the optional dependency is missing.)  
    (license refers to cpython, you can use any interpreter)  
    LICENSE: Python Software Foundation License Version 2, GPLv3 compatible.  
    Is probably already preinstalled. In your package manager under the  
    name 'python3'.  

#### All the other stuff this program uses:
All these dependencies are fully optional. Its just that the conversions using  
the dependencies will not work.  

3. FFmpeg  
    LICENSE: LGPL 2.1+  
    Available in most GNU/Linux package managers under the name  
    'ffmpeg'. Needed for video and audio support, for example   
    .wav and .mp4 files.  

4. LibreOffice  
    LICENSE: MPL 2.0 (Mozilla Public License)  
    Already preinstalled in some distros, including Ubuntu and Linux Mint  
    Available in most GNU/Linux package managers under the name  
    'libreoffice'. Needed for most office formats.  

5. pandoc  
    LICENSE: GPLv2+  
    Available in most GNU/Linux package managers under the name  
    'pandoc' or 'pandoc-cli.  
    Can also be installed by downloading the latest binary from [github](https://github.com/jgm/pandoc/releases)  
    , renaming it to pandoc, making it executable and putting it on the $PATH.  
    Needed for markdown support, for  
    example .html and .md files.  

6. squashfs-tools  
    LICENSE: GPLv2  
    Available in most GNU/Linux package managers under the name  
    'squashfs-tools'. Can also be compiled from [source (github)](https://github.com/plougher/squashfs-tools).  
    Needed for squashfs and snap support.  

7. imagemagick  
    LICENSE: https://imagemagick.org/script/license.php, GPLv3 compatible.  
    Available in most GNU/Linux package managers under the name  
    'magick' or 'imagemagick'. Needed for a bunch of image/video formats.  
    [Likely already installed (see hover text)](https://xkcd.com/2347/)  

Some more minor packages that are likely present  
and needed for some things:  

* tar  
    LICENSE: GPLv3+  
    Needed for everything involving .tar-files.  
    Didnt bother checking for it yet, program simply assumes tar exists.  

* ar  
    LICENSE: GPLv3+  
    Needed for everything involving .ar files (including .deb).  
    Checking is done in ./get_types.py.  