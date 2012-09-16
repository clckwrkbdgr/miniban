TARGET = miniban
DESTDIR = ../bin

OBJECTS_DIR = ../tmp
MOC_DIR = ../tmp
UI_DIR = ../tmp

src_dir = ../src
modules = $${src_dir}/sokoban mainwindow sokobanwidget xpm sprites

INCLUDEPATH += $$src_dir
SOURCES += main.cpp
for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}