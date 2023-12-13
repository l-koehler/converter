# needed for compilation process

QT += widgets

SOURCES += convert.cpp \
           functions.cpp \
           converter_specific.cpp
HEADERS += functions.h \
           converter_specific.h

TARGET = converter-bin
