All these dependencies are fully optional. Its just that the conversions using
the dependencies will not work.

1: FFmpeg
    LICENSE: LGPL 2.1+
    Available in most GNU/Linux package managers under the name
    'ffmpeg'. Needed for video and audio support, for example 
    .wav and .mp4 files.

2: unoconv
    LICENSE: GPLv2
    Available in most GNU/Linux package managers under the name
    'unoconv'. Needed for office formats like .docx, .ppt and .xls.

    2.1: LibreOffice
        LICENSE: MPL 2.0 (Mozilla Public License)
        Unoconv depends on LibreOffice/OpenOffice (LibreOffice is recommended)
        Already preinstalled in some distros, including Ubuntu and Linux Mint
        Available in most GNU/Linux package managers under the name
        'libreoffice'. Needed for unoconv to work.

3: pandoc
    LICENSE: GPLv2+
    Available in most GNU/Linux package managers under the name
    'pandoc' or 'pandoc-cli.
    Can also be installed by downloading the latest binary from [github](https://github.com/jgm/pandoc/releases)
    , renaming it to pandoc, making it executable and putting it on the $PATH.
    Needed for markdown support, for
    example .html and .md files.


4: Qt
    LICENSE: For this Software it is under the GPLv3.
    (https://www.qt.io/faq/tag/qt-open-source-licensing)
    Is probably already preinstalled. If it is not, use your
    systems package manager to install it. Needed only for the GUI,
    but the program will not compile without it.
    If you do not want the GUI, just delete all code referring qt.
