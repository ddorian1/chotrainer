TEMPLATE = app
TARGET = chotrain
INCLUDEPATH += .
CONFIG += debug

HEADERS +=	Exception.h \
			MidiParser.h

SOURCES +=	Exception.cpp \
			main.cpp \
			MidiParser.cpp

QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror
MOC_DIR = moc
OBJECTS_DIR = obj
