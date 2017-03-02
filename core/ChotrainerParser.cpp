#include "ChotrainerParser.h"
#include "Exception.h"

#include <array>
#include <fstream>
#include <cstring>

static uint32_t btoh32(void* v) {
	uint8_t *p = reinterpret_cast<uint8_t*>(v);
	uint32_t n = 0;
	n += static_cast<size_t>(*p) << 24;
	n += static_cast<size_t>(*(p + 1)) << 16;
	n += static_cast<size_t>(*(p + 2)) << 8;
	n += static_cast<size_t>(*(p + 3));
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

ChotrainerParser::ChotrainerParser(const std::string &filePath) {
	std::ifstream f(filePath, std::ifstream::binary);
	if (!f) throw(Exception("Can't open file"));

	std::vector<uint8_t> begin(5);
	f.read(reinterpret_cast<char*>(begin.data()), 5);
	if (!f) throw(Exception("Can't read file"));

	const std::vector<uint8_t> magicChars = {'C', 'h', 'o', 'T'};
	if (memcmp(magicChars.data(), begin.data(), 4))
		throw(Exception("Doesn't seem to be a chotrian file"));
	if (begin[4] != 1)
		throw(Exception("No support for file version"));

	std::vector<uint8_t> tmp(4);
	f.read(reinterpret_cast<char*>(tmp.data()), 4);
	if (!f) throw(Exception("Can't read file"));

	const uint32_t pieceNameLength = btoh32(tmp.data());
	try {
		pieceName.resize(pieceNameLength);
	} catch (const std::bad_alloc &e) {
		throw(Exception("Can't allocate enough memory (pieceName)"));
	}
	f.read(&pieceName[0], pieceNameLength);
	if (!f) throw(Exception("Can't read file"));

	f.read(reinterpret_cast<char*>(tmp.data()), 4);
	if (!f) throw(Exception("Can't read file"));

	const uint32_t numberOfNamedTracks = btoh32(tmp.data());
	for (size_t i = 0; i < numberOfNamedTracks; ++i) {
		std::vector<uint8_t> header(8);
		f.read(reinterpret_cast<char*>(header.data()), 8);
		if (!f) throw(Exception("Can't read file"));

		const uint32_t trackNameLength = btoh32(header.data() + 4);
		std::string trackName;
		try {
			trackName.resize(trackNameLength);
		} catch (const std::bad_alloc &e) {
			throw(Exception("Can't allocate enough memory (trackName)"));
		}
		f.read(&trackName[0], trackNameLength);
		if (!f) throw(Exception("Can't read file"));

		namedTracks.emplace_back(btoh32(header.data()), trackName);
	}

	std::vector<uint8_t> data(4);
	f.read(reinterpret_cast<char*>(data.data()), 4);
	if (!f) throw(Exception("Can't read file"));
	const uint32_t midiFileLength = btoh32(data.data());

	midiFile.resize(midiFileLength);
	f.read(reinterpret_cast<char*>(midiFile.data()), midiFileLength);
	if (!f) throw(Exception("Can't read file"));
}	

void ChotrainerParser::createNewFile(const std::vector<Track> &namedTracks, const std::string &pieceName, const std::vector<uint8_t> &midiFile, const std::string &filePath) {
	std::ofstream f(filePath, std::ofstream::binary);
	if (!f) throw(Exception("Can't open file"));

	const std::vector<uint8_t> begin = {'C', 'h', 'o', 'T', 1};
	f.write(reinterpret_cast<const char*>(begin.data()), begin.size());
	if (!f) throw(Exception("Can't write to file"));

	const auto lengthOfPieceNameB = htob32(pieceName.size());
	f.write(reinterpret_cast<const char*>(lengthOfPieceNameB.data()), 4);
	if (!f) throw(Exception("Can't write to file"));

	f.write(pieceName.c_str(), pieceName.size());
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

std::string ChotrainerParser::getPieceName() const {
	return pieceName;
}

std::vector<ChotrainerParser::Track> ChotrainerParser::getNamedTracks() const {
	return namedTracks;
}

std::vector<uint8_t> ChotrainerParser::getMidiFile() const {
	return midiFile;
}
