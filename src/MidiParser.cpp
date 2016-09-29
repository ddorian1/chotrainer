#include "MidiParser.h"

#include <cstring>
#include <list>
#include <vector>
#include <tuple>

MidiParser::MidiParser(const std::string &filePath)
:
	file(filePath, std::ifstream::binary)
{
	size_t trackCount = 0;
	while (getTrackStart(trackCount) != nullptr) ++trackCount;
	voiceMuted = std::vector<bool>(trackCount, false);

	if (!file) throw("Can't open file"); //TODO
	file.seekg(0, file.end);
	const size_t fileSize = file.pos();
	file.seekg(0, file.beg);
	data = std::vector<char>(fileSize);
	file.read(data.data(), fileSize);
	if (!file) throw("Can't read file");
}

char* MidiParser::getTrackStart(size_t track) const {
	constexpr char trackMark[4] = {'M', 'T', 'r', 'k'};
	size_t nextTrack = 0;
	char* p = data.data();
	for (size_t i = 0; i < data.size() - 3; ++i, ++p) {
		if (!std::memcmp(trackMark, p, 4)) {
			if (nextTrack == track) return p;
			++nextTrack;
		}
	}
	return nullptr;
}

char* MidiParser::getInstrumentPos(size_t track) const {
	constexpr unsigned char instMark[2] = {0xFF, 0x04}; //??
	char* p = getTrackStart(track);
	char* trackEnd = getTrackStart(track + 1);
	if (!trackEnd) trackEnd = data.data() + data.size();

	while (p != trackEnd - 1) if (!std::memcmp(instMark, p, 2)) return p;
	return nullptr;
}

void MidiParser::setInstrument(size_t track, const std::string &instrument) {
	char const* instPos = getInstrumentPos(track);
	if (!instPos) throw(""); //TODO

	size_t oldInstNameSize, oldVLengthBytes;
	std::tie(oldInstNameSize, oldVLengthBytes) = getVLength(instPos + 2);

	const std::vector<unsigned char> newVLength = sizeTToVLength(instrument.size());

	char const* copyTo = instPos + 2 + newVLength.size() + instrument.size();
	char const* copyFrom = instPos + 2 + oldVLengthBytes + oldInstNameSize;
	const size_t bytesToCopy = getBytesTillEnd(copyFrom);
	if (oldInstNameSize + oldVLengthBytes < instrument.size() + newVLength.size())
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrument.size() + newVLength.size());
	if (getBytesTillEnd(copyTo) < bytesToCopy) throw("Integer overflow"); //TODO
	std::memmove(copyTo, copyFrom, bytesToCopy);
	if (oldInstNameSize + oldVLengthBytes > instrument.size() + newVLength.size())
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrument.size() + newVLength.size());

	std::memcpy(instPos + 2, newVLength.data(), newVLength.size());
	std::memcpy(instPos + 2 + newVLength.size(), instrument.data(), instrument.size());
			
	char const *trackPos = getTrackPos(track);
	char const *nextTrackPos = getTrackPos(track + 1);
	if (!nextTrackPos) nextTrackPos = data.data() + data.size();
	if (nextTrackPos - trackPos < 8) throw(""); //TODO
	const unsigned long newTrackSize = static_cast<unsigned long>(nextTrackPos - trackPos - 8);
	*(trackPos + 4) = (*nextTrackPos & 0xFF000000lu) >> 24;
	*(trackPos + 5) = (*nextTrackPos & 0x00FF0000lu) >> 16;
	*(trackPos + 6) = (*nextTrackPos & 0x0000FF00lu) >> 8;
	*(trackPos + 7) = *nextTrackPos & 0x000000FFlu;
}

std::pair<size_t, size_t> MidiParser::sizeTFromVLength(const char *p) const {
	size_t s = 0;
	size_t bytes = 1;
	do {
		s << 7;
		s |= *p & 0x7F;
		if (*p & 0x70) return std::make_pair(s, bytes);
		++bytes;
		++p;
	} while (getBytesTillEnd(p));
	throw("End of file reached"); //TODO
}

std::vector<unsigned char> MidiParser::sizeTToVLength(size_t s) {
	std::list<unsigned char> l;
	v.push_front(s & 0x7F);
	s >> 7;
	while (s) {
		v.push_front((s & 0x7F) | 0x80);
		s >> 7;
	}
	std::vector<unsigned char> v(l.size());
	auto it v.begin();
	while (!l.empty()) {
		*it = l.front();
		l.pop_front();
	}
	return v;
}

