QT += testlib
TEMPLATE = app
#CONFIG += testcase
TARGET = test
INCLUDEPATH += ../libchotrainer

win32 {
	LIBPATH += ../libchotrainer/release
	LIBS += -lchotrainer0
}
unix {
	LIBPATH += ../libchotrainer
	LIBS += -lchotrainer
}

include(../common.pri)

HEADERS +=	TestMain.h

SOURCES +=	TestChotrainerParser.cpp \
			TestFluidsynth.cpp \
			TestMain.cpp \
			TestMidiParser.cpp

RESOURCES += res/test.qrc
