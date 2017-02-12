TEMPLATE = lib
TARGET = core
CONFIG += staticlib

include(../common.pri)

HEADERS +=	ChotrainParser.h \
			Exception.h \
			Fluidsynth.h \
			MidiParser.h

SOURCES +=	ChotrainParser.cpp \
			Exception.cpp \
			Fluidsynth.cpp \
			MidiParser.cpp

RESOURCES += res/core.qrc \
             ../intl/intl.qrc
