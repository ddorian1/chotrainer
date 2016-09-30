#include "MidiParser.h"
#include "Exception.h"

#include <QDir>
#include <QCoreApplication>

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

	//TODO test for file type 1

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

std::list<char*> MidiParser::getPosOfEvents(size_t track, unsigned char event, unsigned char mask) {
	std::list<char*> events;

	char *p = getTrackPos(track);
	const char *nextTrack = getTrackPos(track + 1);
	if (!nextTrack) nextTrack = data.data() + data.size();

	p += 8; //Skip "MTrk" and length
	while (nextTrack - p > 0) {
		/* Skip delta_time */
		size_t vLengthBytes;
		std::tie(std::ignore, vLengthBytes) = sizeTFromVLength(p);
		p += vLengthBytes;
		if (nextTrack - p <= 0) throw(Exception("Invalid file"));

		/* Check if event matches search */
		if (event == (*reinterpret_cast<unsigned char*>(p) & mask)) events.push_back(p);

		/* Skip event */
		switch (*reinterpret_cast<unsigned char*>(p) & 0xF0u) {
			case 0xF0u:
				switch (*reinterpret_cast<unsigned char*>(p)) {
					case 0xFFu:
						/* Meta event */
						p += 2;
						break;
					case 0xF0u:
					case 0xF7u:
						/* Sysex event */
						++p;
						break;
					default:
						throw(Exception("Unknown event"));
				}
				if (nextTrack - p <= 0) throw(Exception("Invalid file"));
				size_t length;
				std::tie(length, vLengthBytes) = sizeTFromVLength(p);
				p += vLengthBytes + length;
				break;
			case 0xC0u:
			case 0xD0u:
				/* One Byte of data */
				p += 2;
				break;
			case 0x80u:
			case 0x90u:
			case 0xA0u:
			case 0xB0u:
			case 0xE0u:
				/* Two Bytes of data */
				p += 3;
				break;
			default:
				throw(Exception("Unknown event"));
		}
	}

	return events;
}

char* MidiParser::getInstrumentPos(size_t track) {
	std::list<char*> metaEvents = getPosOfEvents(track, 0xFFu);
	for (const auto &event : metaEvents) {
		if (*reinterpret_cast<unsigned char*>(event + 1) == 0x04u)
			return event;
	}
	return nullptr;
}

void MidiParser::setInstrument(size_t track, unsigned char instrumentId, const std::string &instrumentName) {
	setInstrumentName(track, instrumentName);
	setInstrumentId(track, instrumentId);
}

void MidiParser::setInstrumentId(size_t track, unsigned char instrumentId) {
	std::list<char*> programChangeEvents = getPosOfEvents(track, 0xC0u, 0xF0u);
	for (const auto &event : programChangeEvents)
		*reinterpret_cast<unsigned char*>(event + 1) = instrumentId;
}

void MidiParser::setInstrumentName(size_t track, const std::string &instrumentName) {
	char *instPos = getInstrumentPos(track);
	if (!instPos) throw(Exception("No such track"));

	size_t oldInstNameSize, oldVLengthBytes;
	std::tie(oldInstNameSize, oldVLengthBytes) = sizeTFromVLength(instPos + 2);

	const std::vector<unsigned char> newVLength = sizeTToVLength(instrumentName.size());

	if (oldInstNameSize + oldVLengthBytes < instrumentName.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrumentName.size() + newVLength.size());
		instPos = getInstrumentPos(track);
	}

	char *copyTo = instPos + 2 + newVLength.size() + instrumentName.size();
	char *copyFrom = instPos + 2 + oldVLengthBytes + oldInstNameSize;
	const size_t bytesToCopy = std::min(getBytesTillEnd(copyFrom), getBytesTillEnd(copyTo));
	std::memmove(copyTo, copyFrom, bytesToCopy);

	if (oldInstNameSize + oldVLengthBytes > instrumentName.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrumentName.size() + newVLength.size());
		instPos = getInstrumentPos(track);
	}

	std::memcpy(instPos + 2, newVLength.data(), newVLength.size());
	std::memcpy(instPos + 2 + newVLength.size(), instrumentName.data(), instrumentName.size());
			
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
	setInstrument(track, 0x35u, "voice oohs");
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		if (i != track) {
			try {
				setInstrument(i, 0x00u, "acoustic grand");
			} catch (Exception &e) {}
		}
	}
}

std::shared_ptr<QTemporaryFile> MidiParser::getTmpFile() const {
	std::shared_ptr<QTemporaryFile> tmpFile(new QTemporaryFile(QDir::cleanPath(QString("%1/%2-XXXXXX.midi").arg(QDir::tempPath(), QCoreApplication::applicationName()))));
	tmpFile->open();

	const std::string path = tmpFile->fileName().toStdString();

	std::ofstream f(path, std::ofstream::binary);
	if (!f) throw(Exception("Can't open file"));
	f.write(data.data(), data.size());
	if (!f) throw(Exception("Can't write to file"));
	
	return tmpFile;
}
