TARGET = miniban
DESTDIR = ../bin

OBJECTS_DIR = ../tmp
MOC_DIR = ../tmp
UI_DIR = ../tmp

src_dir = ../src
HEADERS += $${src_dir}/sokoban.h
SOURCES += $${src_dir}/sokoban.cpp

HEADERS += sokobanwidget.h mainwindow.h
SOURCES += main.cpp sokobanwidget.cpp mainwindow.cpp
