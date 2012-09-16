CONFIG += qtestlib
TARGET = test
DESTDIR = ../bin

OBJECTS_DIR = ../tmp
MOC_DIR = ../tmp

srcdir = ../src
INCLUDEPATH += $${srcdir}
HEADERS += $${srcdir}/sokoban.h
SOURCES += $${srcdir}/sokoban.cpp
SOURCES += test.cpp
