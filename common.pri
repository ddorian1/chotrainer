VERSION = 0.1.0

QT += core
#CONFIG += debug
CONFIG += c++11
QMAKE_CXXFLAGS_DEBUG += -g -Wall -Wextra -Werror
LIBS += -lfluidsynth
MOC_DIR = moc
OBJECTS_DIR = obj

RC_FILE = ../windowsicon/icon.rc
