#include "TestMain.cpp"

#include <Fluidsynth.h>
#include <QTemporaryFile>

void TestMain::fluidsynth() {
	QFile f1(":/MidiParserSetInstrument1.midi");
	QTemporaryFile *tmpF1 = QTemporaryFile::createNativeFile(f1);
	QFile f2(":/MidiParserSetInstrument2.midi");
	QTemporaryFile *tmpF2 = QTemporaryFile::createNativeFile(f2);

	Fluidsynth fs;
	fs.play(tmpF1->fileName().toStdString());
	QTest::qSleep(1000);
	fs.stop();
	QTest::qSleep(1000);
	fs.play(tmpF2->fileName().toStdString());
	QTest::qSleep(1000);

	delete tmpF1;
	delete tmpF2;
}
