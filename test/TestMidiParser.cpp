#include "TestMidiParser.h"
#include "../src/MidiParser.h"

#include <cstring>

bool TestMidiParser::compareFiles(const std::string &path1, const std::string &path2) {
	std::ifstream f1(path1, std::ifstream::binary);

	std::ifstream f2(path2, std::ifstream::binary);

void TestMidiParser::setInstrument() {
	MidiParser m1("");
	m1.setInstrument(3, "church organ");
	std::string f1 = m1.getTmpFilePath();
	QVERIFY(compareFiles(f1, ""));
}

void MidiParser::reset() {
	MidiParser m1("");
	m1.setInstrument(2, "foo baa");
	m1.setInstrument(3, "baa foo");
	m1.reset();
	std::string f1 = m1.getTmpFilePath();
	QVERIFY(compareFiles(f1, ""));
}
