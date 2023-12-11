#!/bin/bash

# Clean previous builds
# This might fail. That's fine, that just means the file was already not there.
echo "--- Deleting previous build ---"
rm ./Makefile ./convert.o ./convert ./.qmake.stash ./converter_specific.o ./functions.o ./main.moc

# Compile
echo "--- Running 'moc ./convert.cpp > main.moc' ---"
moc ./convert.cpp > main.moc
echo "--- Running 'qmake convert.pro'"
qmake convert.pro
echo "--- Running 'make'"
make
