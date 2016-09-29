#include "TestMain.h"
#include "../src/MidiParser.h"

#include <cstring>

static bool compareFiles(const std::string &path1, const std::string &path2) {
	std::ifstream f1(path1, std::ifstream::binary);

	std::ifstream f2(path2, std::ifstream::binary);
	return true;
}

void TestMain::midiParser() {
}
