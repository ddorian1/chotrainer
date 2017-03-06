TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS =	core \
			chotrainer \
			chotrainerCreator \
			test

TRANSLATIONS += intl/chotrainer_de.ts

chotrainer.depends = core
chotrainerCreator.depends = core
test.depends = core
