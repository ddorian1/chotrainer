######################################################################
# Automatically generated by qmake (3.0) Fr. Sep. 30 01:02:19 2016
######################################################################

QT += testlib
TEMPLATE = app
TARGET = test
INCLUDEPATH += ../core
LIBS += ../core/libcore.a
CONFIG += testcase

include(../common.pri)

HEADERS +=	TestMain.h

SOURCES +=	TestMain.cpp \
			TestMidiParser.cpp

RESOURCES += res/res.qrc
