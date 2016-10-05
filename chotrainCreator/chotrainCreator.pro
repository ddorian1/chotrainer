QT += widgets
TEMPLATE = app
TARGET = chotrainCreator
INCLUDEPATH += ../core
LIBS += ../core/libcore.a

include(../common.pri)

HEADERS +=	MainWindow.h

SOURCES +=	Main.cpp \
			MainWindow.cpp
