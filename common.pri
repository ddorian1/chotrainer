QT += core
#CONFIG += debug
QMAKE_CXXFLAGS_DEBUG += -g -Wall -Wextra -Werror
LIBS += -lfluidsynth
MOC_DIR = moc
OBJECTS_DIR = obj

RC_FILE = ../windowsicon/icon.rc
