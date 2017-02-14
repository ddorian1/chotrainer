QT += testlib
TEMPLATE = app
TARGET = test
INCLUDEPATH += ../core
LIBS += ../core/libcore.a
CONFIG += testcase

include(../common.pri)

HEADERS +=	TestMain.h

SOURCES +=	TestChotrainerParser.cpp \
			TestFluidsynth.cpp \
			TestMain.cpp \
			TestMidiParser.cpp

RESOURCES += res/test.qrc
