#include "TestMain.h"

#include <MidiParser.h>

#include <cstring>

static bool compareFiles(const std::string &path1, const std::string &path2) {
	std::vector< std::vector<char> > contents;
	std::vector<std::string> pathes = {path1, path2};

	for (const auto &path : pathes) {
		std::ifstream f(path, std::ifstream::binary);
		if (!f) return false; //TODO

		f.seekg(0, f.end);
		size_t len = f.tellg();
		f.seekg(0, f.beg);
		
		std::vector<char> c(len);
		f.read(c.data(), len);
		if (!f) return false; //TODO

		contents.push_back(std::move(c));
	}

	if (contents[0].size() != contents[1].size()) return false;
	return !std::memcmp(contents[0].data(), contents[1].data(), contents[0].size());
}

void TestMain::midiParser() {
	QFile f1(":/MidiParserSetInstrument1.midi");
	QTemporaryFile *tmpF1 = QTemporaryFile::createNativeFile(f1);
	QFile f2(":/MidiParserSetInstrument2.midi");
	QTemporaryFile *tmpF2 = QTemporaryFile::createNativeFile(f2);

	MidiParser mp(tmpF1->fileName().toStdString());
	std::shared_ptr<QTemporaryFile> tmpFile = mp.withForegroundVoice(2);

	QVERIFY(compareFiles(tmpFile->fileName().toStdString(), tmpF2->fileName().toStdString()));

	const std::list<size_t> musicTracks = mp.getMusicTracks();
	const std::list<size_t> musicTracksShouldBe = {1, 2};
	QVERIFY(musicTracks == musicTracksShouldBe);

	delete tmpF1;
	delete tmpF2;
}
