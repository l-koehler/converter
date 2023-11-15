#!/bin/bash

# Clean previous builds
# This might fail. That's fine, that just means the file was already not there.
rm ./Makefile ./convert.o ./Convert ./.qmake.stash

# Compile
moc ./convert.cpp > main.moc
qmake convert.pro
make
