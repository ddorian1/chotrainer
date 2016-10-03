#include "ChotrainParser.h"
#include "Exception.h"

#include <array>
#include <fstream>
#include <cstring>

static uint32_t btoh32(void* p) {
	uint32_t n = 0;
	n += *reinterpret_cast<size_t*>(p) << 24;
	n += *reinterpret_cast<size_t*>(p) << 16;
	n += *reinterpret_cast<size_t*>(p) << 8;
	n += *reinterpret_cast<size_t*>(p);
	return n;
}

static std::array<uint8_t, 4> htob32(uint32_t n) {
	std::array<uint8_t, 4> a;
	a[0] = (n & 0xFF000000) >> 24;
	a[1] = (n & 0x00FF0000) >> 16;
	a[2] = (n & 0x0000FF00) >> 8;
	a[3] = (n & 0x000000FF);
	return a;
}

ChotrainParser::ChotrainParser(const std::string &filePath) {
	std::ifstream f(filePath, std::ifstream::binary);
	if (!f) throw(Exception("Can't open file"));

	std::vector<uint8_t> begin(9);
	f.read(reinterpret_cast<char*>(begin.data()), 9);
	if (!f) throw(Exception("Can't read file"));

	const std::vector<uint8_t> magicChars = {'C', 'h', 'o', 'T'};
	if (memcmp(magicChars.data(), begin.data(), 4))
		throw(Exception("Doesn't seem to be a chotrian file"));
	if (begin[4] != 1)
		throw(Exception("No support for file version"));

	const uint32_t numberOfNamedTracks = btoh32(begin.data() + 5);
	namedTracks.resize(numberOfNamedTracks);

	for (size_t i = 0; i < numberOfNamedTracks; ++i) {
		std::vector<uint8_t> data1(8);
		f.read(reinterpret_cast<char*>(data1.data()), 8);
		if (!f) throw(Exception("Can't read file"));
		namedTracks[i].number = btoh32(data1.data());
		const uint32_t trackNameLength = btoh32(data1.data() + 4);

		std::vector<char> data2(trackNameLength + 1);
		f.read(data2.data(), trackNameLength);
		if (!f) throw(Exception("Can't read file"));
		data2[trackNameLength] = '\0';
		namedTracks[i].name = data2.data();
	}

	std::vector<uint8_t> data(4);
	f.read(reinterpret_cast<char*>(data.data()), 4);
	if (!f) throw(Exception("Can't read file"));
	const uint32_t midiFileLength = btoh32(data.data());

	midiFile.resize(midiFileLength);
	f.read(reinterpret_cast<char*>(midiFile.data()), midiFileLength);
	if (!f) throw(Exception("Can't read file"));
}	

void ChotrainParser::createNewFile(const std::vector<Track> &namedTracks, const std::vector<uint8_t> &midiFile, const std::string &filePath) {
	std::ofstream f(filePath, std::ofstream::binary);
	if (!f) throw(Exception("Can't open file"));

	const std::vector<uint8_t> begin = {'C', 'h', 'o', 'T', 1};
	f.write(reinterpret_cast<const char*>(begin.data()), begin.size());
	if (!f) throw(Exception("Can't write to file"));

	const auto numberOfNamedTracksB = htob32(namedTracks.size());
	f.write(reinterpret_cast<const char*>(numberOfNamedTracksB.data()), 4);
	if (!f) throw(Exception("Can't write to file"));

	for (const auto &t : namedTracks) {
		const auto trackNumberB = htob32(t.number);
		f.write(reinterpret_cast<const char*>(trackNumberB.data()), 4);
		if (!f) throw(Exception("Can't write to file"));

		const auto trackNameLengthB = htob32(t.name.size());
		f.write(reinterpret_cast<const char*>(trackNameLengthB.data()), 4);
		if (!f) throw(Exception("Can't write to file"));

		f.write(t.name.c_str(), t.name.size());
		if (!f) throw(Exception("Can't write to file"));
	}

	const auto midiFileLengthB = htob32(midiFile.size());
	f.write(reinterpret_cast<const char*>(midiFileLengthB.data()), 4);
	if (!f) throw(Exception("Can't write to file"));

	f.write(reinterpret_cast<const char*>(midiFile.data()), midiFile.size());
}

std::vector<ChotrainParser::Track> ChotrainParser::getNamedTracks() const {
	return namedTracks;
}

std::vector<uint8_t> ChotrainParser::getMidiFile() const {
	return midiFile;
}
