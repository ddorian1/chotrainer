#include "TestMain.h"

#include <ChotrainerParser.h>

#include <vector>

void TestMain::chotrainerParser() {
	QFile f(":/MidiParserSetInstrument1.midi");
	QVERIFY(f.open(QIODevice::ReadOnly));
		
	//QTemporaryFile *tmpF = QTemporaryFile::createNativeFile(f1);

	std::vector<uint8_t> midiFile(f.size());
	f.read(reinterpret_cast<char*>(midiFile.data()), midiFile.size());

	QTemporaryFile tmp;
	tmp.open();
	const ChotrainerParser::Track t1 = {1, "Sopran"};
	const ChotrainerParser::Track t2 = {2, "Alt"};
	const std::vector<ChotrainerParser::Track> namedTracks = {t1, t2};
	ChotrainerParser::createNewFile(namedTracks, midiFile, tmp.fileName().toStdString());

	ChotrainerParser cp(tmp.fileName().toStdString());

	QVERIFY(namedTracks == cp.getNamedTracks());
	QVERIFY(midiFile == cp.getMidiFile());
}
