TEMPLATE = lib
TARGET = core
CONFIG += staticlib

include(../common.pri)

HEADERS +=	ChotrainerParser.h \
			Exception.h \
			Fluidsynth.h \
			MidiParser.h

SOURCES +=	ChotrainerParser.cpp \
			Exception.cpp \
			Fluidsynth.cpp \
			MidiParser.cpp

RESOURCES += res/core.qrc \
             ../intl/intl.qrc
