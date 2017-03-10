TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS =	libchotrainer \
			chotrainer \
			chotrainerCreator \
			test

TRANSLATIONS += intl/chotrainer_de.ts

chotrainer.depends = libchotrainer
chotrainerCreator.depends = libchotrainer
test.depends = libchotrainer
