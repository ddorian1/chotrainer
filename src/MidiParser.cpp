#include "MidiParser.h"
#include "Exception.h"

#include <cstring>
#include <list>
#include <tuple>

MidiParser::MidiParser(const std::string &filePath)
:
	file(filePath, std::ifstream::binary)
{
	if (!file) throw(Exception("Can't open file"));
	file.seekg(0, file.end);
	const size_t fileSize = file.tellg();
	file.seekg(0, file.beg);
	data = std::vector<char>(fileSize);
	file.read(data.data(), fileSize);
	if (!file) throw(Exception("Can't read file"));

	size_t trackCount = 0;
	while (getTrackPos(trackCount) != nullptr) ++trackCount;
	voiceMuted = std::vector<bool>(trackCount, false);
}

char* MidiParser::getTrackPos(size_t track) {
	constexpr char trackMark[4] = {'M', 'T', 'r', 'k'};
	size_t nextTrack = 0;
	char *p = data.data();
	for (size_t i = 0; i < data.size() - 3; ++i, ++p) {
		if (!std::memcmp(trackMark, p, 4)) {
			if (nextTrack == track) return p;
			++nextTrack;
		}
	}
	return nullptr;
}

char* MidiParser::getInstrumentPos(size_t track) {
	constexpr unsigned char instMark[2] = {0xFF, 0x04}; //??
	char *trackEnd = getTrackPos(track + 1);
	if (!trackEnd) trackEnd = data.data() + data.size();

	for (char *p = getTrackPos(track); p != trackEnd - 1; ++p)
		if (!std::memcmp(instMark, p, 2)) return p;
	return nullptr;
}

void MidiParser::setInstrument(size_t track, const std::string &instrument) {
	char *instPos = getInstrumentPos(track);
	if (!instPos) throw(Exception("No such track"));

	size_t oldInstNameSize, oldVLengthBytes;
	std::tie(oldInstNameSize, oldVLengthBytes) = sizeTFromVLength(instPos + 2);

	const std::vector<unsigned char> newVLength = sizeTToVLength(instrument.size());

	if (oldInstNameSize + oldVLengthBytes < instrument.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrument.size() + newVLength.size());
		instPos = getInstrumentPos(track);
	}

	char *copyTo = instPos + 2 + newVLength.size() + instrument.size();
	char *copyFrom = instPos + 2 + oldVLengthBytes + oldInstNameSize;
	const size_t bytesToCopy = std::min(getBytesTillEnd(copyFrom), getBytesTillEnd(copyTo));
	std::memmove(copyTo, copyFrom, bytesToCopy);

	if (oldInstNameSize + oldVLengthBytes > instrument.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrument.size() + newVLength.size());
		instPos = getInstrumentPos(track);
	}

	std::memcpy(instPos + 2, newVLength.data(), newVLength.size());
	std::memcpy(instPos + 2 + newVLength.size(), instrument.data(), instrument.size());
			
	char *trackPos = getTrackPos(track);
	char *nextTrackPos = getTrackPos(track + 1);
	if (!nextTrackPos) nextTrackPos = data.data() + data.size();
	if (nextTrackPos - trackPos < 8) throw(Exception("Invalid file"));
	const unsigned long newTrackSize = static_cast<unsigned long>(nextTrackPos - trackPos - 8);	//TODO test for overflow
	*(trackPos + 4) = (newTrackSize & 0xFF000000lu) >> 24;
	*(trackPos + 5) = (newTrackSize & 0x00FF0000lu) >> 16;
	*(trackPos + 6) = (newTrackSize & 0x0000FF00lu) >> 8;
	*(trackPos + 7) = newTrackSize & 0x000000FFlu;
}

std::pair<size_t, size_t> MidiParser::sizeTFromVLength(const char *p) const {
	size_t s = 0;
	size_t bytes = 1;
	do {
		s <<= 7;
		s |= *p & 0x7F;
		if ((*p & 0x80) == 0) return std::make_pair(s, bytes);
		++bytes;
		++p;
	} while (getBytesTillEnd(p));
	throw(Exception("End of file reached"));
}

std::vector<unsigned char> MidiParser::sizeTToVLength(size_t s) {
	std::list<unsigned char> l;
	l.push_front(s & 0x7F);
	s >>= 7;
	while (s) {
		l.push_front((s & 0x7F) | 0x80);
		s >>= 7;
	}
	std::vector<unsigned char> v(l.size());
	auto it = v.begin();
	while (!l.empty()) {
		*it = l.front();
		l.pop_front();
	}
	return v;
}

size_t MidiParser::getBytesTillEnd(const char *p) const {
	const char *end = data.data() + data.size();
	if (p - data.data() < 0 || end - p < 1) throw(Exception("p not in data"));
	return static_cast<size_t>(end - p);
}

void MidiParser::setForegroundVoice(size_t track) {
	setInstrument(track, "voice oohs");
	setInstrument(1, "acoustic grand");
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		if (i != track) {
			try {
				setInstrument(i, "acoustic grand");
			} catch (Exception &e) {}
		}
	}
}

std::string MidiParser::getTmpFilePath() {
	tmpFile.reset(new QTemporaryFile("XXXXXX.midi"));
	tmpFile->open();

	const std::string path = tmpFile->fileName().toStdString();

	std::ofstream f(path, std::ofstream::binary);
	if (!f) throw(Exception("Can't open file"));
	f.write(data.data(), data.size());
	if (!f) throw(Exception("Can't write to file"));
	
	return path;
}
