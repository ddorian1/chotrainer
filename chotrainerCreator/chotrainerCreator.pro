QT += widgets
TEMPLATE = app
TARGET = chotrainerCreator
INCLUDEPATH += ../core

win32 {
	LIBPATH += ../core/release
	LIBS += -lcore0
}
unix {
	LIBPATH += ../core
	LIBS += -lcore
}

include(../common.pri)

HEADERS +=	MainWindow.h

SOURCES +=	Main.cpp \
			MainWindow.cpp

target.path = /usr/bin
INSTALLS += target
