QT += widgets
TEMPLATE = app
TARGET = chotrainer
INCLUDEPATH += ../core
LIBPATH += ../core ../core/release
LIBS += -lcore

include(../common.pri)

HEADERS +=	MainWindow.h \
			SelectVoiceDialog.h

SOURCES +=	Main.cpp \
			MainWindow.cpp \
			SelectVoiceDialog.cpp