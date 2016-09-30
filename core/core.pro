TEMPLATE = lib
TARGET = core
CONFIG += staticlib

include(../common.pri)

HEADERS +=	Exception.h \
			MidiParser.h

SOURCES +=	Exception.cpp \
			MidiParser.cpp
