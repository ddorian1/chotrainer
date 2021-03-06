TEMPLATE = lib
TARGET = chotrainer
CONFIG += lib

include(../common.pri)

HEADERS +=	ChotrainerParser.h \
			Exception.h \
			Fluidsynth.h \
			MidiParser.h

SOURCES +=	ChotrainerParser.cpp \
			Exception.cpp \
			Fluidsynth.cpp \
			MidiParser.cpp

RESOURCES += res/chotrainer.qrc \
             ../intl/intl.qrc

target.path = /usr/lib
INSTALLS += target

desktop.path = /usr/share/applications
desktop.files = ../chotrainer.desktop ../chotrainerCreator.desktop
INSTALLS += desktop

appicon.path = /usr/share/pixmaps
appicon.files = res/chotrainer.svg
INSTALLS += appicon

## Seems to be broken with qt <= 5.6
#mimeicontarget.target = res/application-chotrainer.svg
#mimeicontarget.commands = cp res/chotrainer.svg res/application-chotrainer.svg
#QMAKE_EXTRA_TARGETS += mimeicontarget
#POST_TARGETDEPS += res/application-chotrainer.svg

mimeicon.path = /usr/share/icons/hicolor/scalable/mimetypes
mimeicon.files = res/application-chotrainer.svg
INSTALLS += mimeicon

mime.path = /usr/share/mime/packages
mime.files = ../chotrainer-mime.xml
INSTALLS += mime
