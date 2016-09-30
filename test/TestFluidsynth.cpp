#include "TestMain.cpp"

#include <Fluidsynth.h>
#include <QTemporaryFile>

void TestMain::fluidsynth() {
	QFile f(":/MidiParserSetInstrument1.midi");
	QTemporaryFile *tmpF = QTemporaryFile::createNativeFile(f);
	Fluidsynth fs;
	fs.play(tmpF->fileName().toStdString());
	QTest::qSleep(1000);
	delete tmpF;
}
