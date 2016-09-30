TEMPLATE = lib
TARGET = core
CONFIG += staticlib

include(../common.pri)

HEADERS +=	Exception.h \
			Fluidsynth.h \
			MidiParser.h

SOURCES +=	Exception.cpp \
			Fluidsynth.cpp \
			MidiParser.cpp
