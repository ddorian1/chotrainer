QT += widgets
TEMPLATE = app
TARGET = chotrainer
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

HEADERS +=	MainWindow.h \
			SelectVoiceDialog.h

SOURCES +=	Main.cpp \
			MainWindow.cpp \
			SelectVoiceDialog.cpp

target.path = /usr/bin
INSTALLS += target
