TARGET = miniban
QT += xml

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

SOKOBAN_TEST {
	DEFINES += SOKOBAN_TEST
	QT += testlib
	modules = sokoban
} else {
	RESOURCES += sokoban.qrc
	HEADERS += abstractgamemode.h
	SOURCES += main.cpp
	modules = sokoban sokobanwidget
	modules += xpm sprites levelset
	modules += playingmode messagemode fademode
}

for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
