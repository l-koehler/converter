#!/bin/bash
rm Makefile
rm convert.o
rm Convert
rm .qmake.stash
moc ./convert.cpp > main.moc
qmake convert.pro
make
