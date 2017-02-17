QT += testlib
TEMPLATE = app
TARGET = test
INCLUDEPATH += ../core
LIBPATH += ../core ../core/release
LIBS += -lcore
CONFIG += testcase

include(../common.pri)

HEADERS +=	TestMain.h

SOURCES +=	TestChotrainerParser.cpp \
			TestFluidsynth.cpp \
			TestMain.cpp \
			TestMidiParser.cpp

RESOURCES += res/test.qrc
