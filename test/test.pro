######################################################################
# Automatically generated by qmake (3.0) Fr. Sep. 30 01:02:19 2016
######################################################################

QT += testlib
TEMPLATE = app
TARGET = test
INCLUDEPATH += .
CONFIG += testcase

HEADERS +=	TestMain.h \
			../src/Exception.h \
			../src/MidiParser.h

SOURCES +=	TestMain.cpp \
			TestMidiParser.cpp

OBJECTS +=	../src/obj/Exception.o \
			../src/obj/MidiParser.o

RESOURCES += res/res.qrc

QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror
MOC_DIR = moc
OBJECTS_DIR = obj
