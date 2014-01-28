TARGET = miniban
QT = core
QT += xml

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
LIBS += -lchthon -lSDL2
QMAKE_CXXFLAGS += -std=c++0x

SOKOBAN_TEST {
	modules = sokoban
	SOURCES += sokoban_test.cpp
} else {
	SOURCES += main.cpp
	modules = sokoban sokobanwidget
	modules += sprites levelset counter
	modules += playingmode message
}

for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
