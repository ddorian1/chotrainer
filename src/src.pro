TEMPLATE = app
TARGET = chotrain
INCLUDEPATH += .

HEADERS += MidiParser.h
SOURCES +=	main.cpp \
			MidiParser.cpp

QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror
MOC_DIR = moc
OBJECTS_DIR = obj
