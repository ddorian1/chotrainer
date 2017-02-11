QT += widgets
TEMPLATE = app
TARGET = chotrain
INCLUDEPATH += ../core
LIBS += ../core/libcore.a

include(../common.pri)

HEADERS +=	MainWindow.h \
			SelectVoiceDialog.h

SOURCES +=	Main.cpp \
			MainWindow.cpp \
			SelectVoiceDialog.cpp
