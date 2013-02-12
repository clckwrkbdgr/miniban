TARGET = miniban
QT += xml

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp

HEADERS += abstractgamemode.h
SOURCES += main.cpp levels.cpp
modules = sokoban mainwindow sokobanwidget
modules += xpm sprites levelset
modules += playingmode messagemode fademode
for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
