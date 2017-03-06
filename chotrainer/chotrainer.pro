QT += widgets
TEMPLATE = app
TARGET = chotrainer
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

HEADERS +=	MainWindow.h \
			SelectVoiceDialog.h

SOURCES +=	Main.cpp \
			MainWindow.cpp \
			SelectVoiceDialog.cpp

target.path = /usr/bin
INSTALLS += target
