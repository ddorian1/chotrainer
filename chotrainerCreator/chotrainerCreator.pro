QT += widgets
TEMPLATE = app
TARGET = chotrainerCreator
INCLUDEPATH += ../core
LIBPATH += ../core
LIBS += -lcore

include(../common.pri)

HEADERS +=	MainWindow.h

SOURCES +=	Main.cpp \
			MainWindow.cpp
