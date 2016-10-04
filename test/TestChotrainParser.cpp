#include "TestMain.h"

#include <ChotrainParser.h>

#include <vector>

void TestMain::chotrainParser() {
	QFile f(":/MidiParserSetInstrument1.midi");
	QVERIFY(f.open(QIODevice::ReadOnly));
		
	//QTemporaryFile *tmpF = QTemporaryFile::createNativeFile(f1);

	std::vector<uint8_t> midiFile(f.size());
	f.read(reinterpret_cast<char*>(midiFile.data()), midiFile.size());

	QTemporaryFile tmp;
	tmp.open();
	const ChotrainParser::Track t1 = {1, "Sopran"};
	const ChotrainParser::Track t2 = {2, "Alt"};
	const std::vector<ChotrainParser::Track> namedTracks = {t1, t2};
	ChotrainParser::createNewFile(namedTracks, midiFile, tmp.fileName().toStdString());

	ChotrainParser cp(tmp.fileName().toStdString());

	QVERIFY(namedTracks == cp.getNamedTracks());
	QVERIFY(midiFile == cp.getMidiFile());
}
