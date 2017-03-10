QT += widgets
TEMPLATE = app
TARGET = chotrainerCreator
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

HEADERS +=	MainWindow.h

SOURCES +=	Main.cpp \
			MainWindow.cpp

target.path = /usr/bin
INSTALLS += target
