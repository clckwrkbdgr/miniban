TARGET = miniban
DESTDIR = ../bin

OBJECTS_DIR = ../tmp
MOC_DIR = ../tmp
UI_DIR = ../tmp

src_dir = ../src
INCLUDEPATH += $$src_dir
HEADERS += abstractgamemode.h
SOURCES += main.cpp levels.cpp
modules = $${src_dir}/sokoban mainwindow sokobanwidget
modules += xpm sprites levelset
modules += playingmode messagemode
for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
