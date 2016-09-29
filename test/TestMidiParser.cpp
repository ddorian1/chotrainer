#include "TestMidiParser.h"
#include "../src/MidiParser.h"

#include <cstring>

bool TestMidiParser::compareFiles(const std::string &path1, const std::string &path2) {
	std::ifstream f1(path1, std::ifstream::binary);

	std::ifstream f2(path2, std::ifstream::binary);
	return true;
}

QTEST_MAIN(TestMidiParser)
