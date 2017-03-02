QT += testlib
TEMPLATE = app
CONFIG += testcase
TARGET = test
INCLUDEPATH += ../core

win32 {
	LIBPATH += ../core/release
	LIBS += -lcore0
}
unix {
	LIBPATH += ../core
	LIBS += -lcore
}

include(../common.pri)

HEADERS +=	TestMain.h

SOURCES +=	TestChotrainerParser.cpp \
			TestFluidsynth.cpp \
			TestMain.cpp \
			TestMidiParser.cpp

RESOURCES += res/test.qrc
