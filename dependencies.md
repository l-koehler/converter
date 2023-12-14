Only absolutely needed dependency:
1: Qt
    LICENSE: For this Software it is under the GPLv3.
    (https://www.qt.io/faq/tag/qt-open-source-licensing)
    Is probably already preinstalled. If it is not, use your
    systems package manager to install it. Needed only for the GUI,
    but the program will not compile without it.

All these dependencies are fully optional. Its just that the conversions using
the dependencies will not work.

2: FFmpeg
    LICENSE: LGPL 2.1+
    Available in most GNU/Linux package managers under the name
    'ffmpeg'. Needed for video and audio support, for example 
    .wav and .mp4 files.

3: LibreOffice
    LICENSE: MPL 2.0 (Mozilla Public License)
    Already preinstalled in some distros, including Ubuntu and Linux Mint
    Available in most GNU/Linux package managers under the name
    'libreoffice'. Needed for most office formats.

4: pandoc
    LICENSE: GPLv2+
    Available in most GNU/Linux package managers under the name
    'pandoc' or 'pandoc-cli.
    Can also be installed by downloading the latest binary from [github](https://github.com/jgm/pandoc/releases)
    , renaming it to pandoc, making it executable and putting it on the $PATH.
    Needed for markdown support, for
    example .html and .md files.

5: squashfs-tools
    LICENSE: GPLv2
    Available in most GNU/Linux package managers under the name
    'squashfs-tools'. Can also be compiled from [source (github)](https://github.com/plougher/squashfs-tools).
    Needed for squashfs and snap support.

Some more minor packages that are likely present
and needed for some things: 
tar
    LICENSE: GPLv3+
    Needed for everything involving .tar-files.
    Didnt bother checking for it yet, program simply assumes tar exists.
ar
    LICENSE: GPLv3+
    Needed for everything involving .ar files (including .deb).
    Checking is done in ./get_types.py.