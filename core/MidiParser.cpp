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
	data = std::vector<uint8_t>(fileSize);
	file.read(reinterpret_cast<char*>(data.data()), fileSize);
	if (!file) throw(Exception("Can't read file"));

	//TODO test for file type 1

	size_t trackCount = 0;
	while (getTrackPos(trackCount) != nullptr) ++trackCount;
	voiceMuted = std::vector<bool>(trackCount, false);
}

uint8_t* MidiParser::getTrackPos(size_t track) {
	constexpr uint8_t trackMark[4] = {'M', 'T', 'r', 'k'};
	size_t nextTrack = 0;
	uint8_t *p = data.data();
	for (size_t i = 0; i < data.size() - 3; ++i, ++p) {
		if (!std::memcmp(trackMark, p, 4)) {
			if (nextTrack == track) return p;
			++nextTrack;
		}
	}
	return nullptr;
}

std::list<uint8_t*> MidiParser::getPosOfEvents(size_t track, uint8_t event, uint8_t mask) {
	std::list<uint8_t*> events;

	uint8_t *p = getTrackPos(track);
	size_t trackLen = 0;
	trackLen |= *(p + 4) << 24;
	trackLen |= *(p + 5) << 16;
	trackLen |= *(p + 6) << 8;
	trackLen |= *(p + 7);
	const uint8_t *nextTrack = getTrackPos(track + 1);
	const uint8_t *trackEnd = p + 8 + trackLen;
	if (nextTrack && nextTrack != trackEnd) throw(Exception("File inconsistent"));

	p += 8; //Skip "MTrk" and length
	while (p < trackEnd) {
		/* Skip delta_time */
		size_t vLengthBytes;
		std::tie(std::ignore, vLengthBytes) = sizeTFromVLength(p);
		p += vLengthBytes;
		if (p >= trackEnd) throw(Exception("Invalid file"));

		/* Check if event matches search */
		if (event == (*p & mask)) events.push_back(p);

		/* Skip event */
		switch (*p & 0xF0u) {
			case 0xF0u:
				switch (*p) {
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
				if (p >= trackEnd) throw(Exception("Invalid file"));
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

uint8_t* MidiParser::getInstrumentPos(size_t track) {
	std::list<uint8_t*> metaEvents = getPosOfEvents(track, 0xFFu);
	for (const auto &event : metaEvents) {
		if (*(event + 1) == 0x04u)
			return event;
	}
	return nullptr;
}

void MidiParser::setInstrument(size_t track, uint8_t instrumentId, const std::string &instrumentName) {
	setInstrumentName(track, instrumentName);
	setInstrumentId(track, instrumentId);
}

void MidiParser::setInstrumentId(size_t track, uint8_t instrumentId) {
	std::list<uint8_t*> programChangeEvents = getPosOfEvents(track, 0xC0u, 0xF0u);
	for (const auto &event : programChangeEvents)
		*(event + 1) = instrumentId;
}

void MidiParser::setInstrumentName(size_t track, const std::string &instrumentName) {
	uint8_t *instPos = getInstrumentPos(track);
	if (!instPos) throw(Exception("No such track"));

	size_t oldInstNameSize, oldVLengthBytes;
	std::tie(oldInstNameSize, oldVLengthBytes) = sizeTFromVLength(instPos + 2);

	const std::vector<uint8_t> newVLength = sizeTToVLength(instrumentName.size());

	if (oldInstNameSize + oldVLengthBytes < instrumentName.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrumentName.size() + newVLength.size());
		/* old pointer invalid after expanding size */
		instPos = getInstrumentPos(track);
	}

	uint8_t *copyTo = instPos + 2 + newVLength.size() + instrumentName.size();
	uint8_t *copyFrom = instPos + 2 + oldVLengthBytes + oldInstNameSize;
	const size_t bytesToCopy = std::min(getBytesTillEnd(copyFrom), getBytesTillEnd(copyTo));
	std::memmove(copyTo, copyFrom, bytesToCopy);

	if (oldInstNameSize + oldVLengthBytes > instrumentName.size() + newVLength.size()) {
		data.resize(data.size()
				- oldInstNameSize - oldVLengthBytes
				+ instrumentName.size() + newVLength.size());
	}

	std::memcpy(instPos + 2, newVLength.data(), newVLength.size());
	std::memcpy(instPos + 2 + newVLength.size(), instrumentName.data(), instrumentName.size());
			
	uint8_t *trackPos = getTrackPos(track);
	uint8_t *nextTrackPos = getTrackPos(track + 1);
	if (!nextTrackPos) nextTrackPos = data.data() + data.size();
	if (nextTrackPos - trackPos < 8) throw(Exception("Invalid file"));
	const unsigned long newTrackSize = static_cast<unsigned long>(nextTrackPos - trackPos - 8);	//TODO test for overflow
	*(trackPos + 4) = (newTrackSize & 0xFF000000lu) >> 24;
	*(trackPos + 5) = (newTrackSize & 0x00FF0000lu) >> 16;
	*(trackPos + 6) = (newTrackSize & 0x0000FF00lu) >> 8;
	*(trackPos + 7) = newTrackSize & 0x000000FFlu;
}

std::pair<size_t, size_t> MidiParser::sizeTFromVLength(const uint8_t *p) const {
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

std::vector<uint8_t> MidiParser::sizeTToVLength(size_t s) {
	std::list<uint8_t> l;
	l.push_front(s & 0x7F);
	s >>= 7;
	while (s) {
		l.push_front((s & 0x7F) | 0x80);
		s >>= 7;
	}
	std::vector<uint8_t> v(l.size());
	auto it = v.begin();
	while (!l.empty()) {
		*it = l.front();
		l.pop_front();
	}
	return v;
}

size_t MidiParser::getBytesTillEnd(const uint8_t *p) const {
	const uint8_t *end = data.data() + data.size();
	if (p - data.data() < 0 || end - p < 1) throw(Exception("p not in data"));
	return static_cast<size_t>(end - p);
}

size_t MidiParser::getBytesTillTrackEnd(const uint8_t *p) {
	const uint8_t *t;
	size_t i;
	for (i = 0, t = getTrackPos(i); t != nullptr; ++i, t = getTrackPos(i))
		if (t > p) return t - p;
	return getBytesTillEnd(p);
}

void MidiParser::setNoForegroundVoice() {
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		try {
			setInstrument(i, 0x00u, "acoustic grand");
		} catch (Exception &e) {}
	}
}

void MidiParser::setForegroundVoice(size_t track) {
	setNoForegroundVoice();
	setInstrument(track, 0x35u, "voice oohs");
}

std::shared_ptr<QTemporaryFile> MidiParser::withOnlyVoice(size_t track) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	const std::vector<uint8_t> header = {
		'M', 'T', 'h', 'd', //Type
		0x00, 0x00, 0x00, 0x06, //Length
		0x00, 0x01, //Format
		0x00, 0x01, //Tracks
		data.at(12), data.at(13) //Division
	};
	f->write(reinterpret_cast<const char*>(header.data()), header.size());
	if (!*f) throw(Exception("Can't write to file"));

	setNoForegroundVoice();
	const uint8_t *t = getTrackPos(track);
	size_t s = getBytesTillTrackEnd(t);
	f->write(reinterpret_cast<const char*>(t), s);
	if (!*f) throw(Exception("Can't write to file"));

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withForegroundVoice(size_t track) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	setForegroundVoice(track);
	f->write(reinterpret_cast<char*>(data.data()), data.size());
	if (!*f) throw(Exception("Can't write to file"));

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withoutForegroundVoice() {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	setNoForegroundVoice();
	f->write(reinterpret_cast<char*>(data.data()), data.size());
	if (!*f) throw(Exception("Can't write to file"));

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withoutVoice(size_t track) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	std::vector<uint8_t> header(14);
	std::memcpy(header.data(), data.data(), 14);
	if (header[11] == 0x00) {
		if (header[10] == 0x00) throw(Exception("0 tracks in file"));
		--header[10];
		header[11] = 0xFFu;
	} else {
		--header[11];
	}
	f->write(reinterpret_cast<char*>(header.data()), header.size());
	if (!*f) throw(Exception("Can't write to file"));

	setNoForegroundVoice();
	const uint8_t *t = getTrackPos(track);
	f->write(reinterpret_cast<char*>(data.data() + 14), t - (data.data() + 14));
	if (!*f) throw(Exception("Can't write to file"));

	const uint8_t *nextTrack = getTrackPos(track + 1);
	if (nextTrack) {
		f->write(reinterpret_cast<const char*>(nextTrack), getBytesTillEnd(nextTrack));
		if (!*f) throw(Exception("Can't write to file"));
	}

	return tmpFile;
}

std::pair<std::shared_ptr<QTemporaryFile>, std::shared_ptr<std::ofstream>> MidiParser::newTmpFile() const {
	auto tmpFile = std::make_shared<QTemporaryFile>(QDir::cleanPath(QString("%1/%2-XXXXXX.midi").arg(QDir::tempPath(), QCoreApplication::applicationName())));
	tmpFile->open();

	const std::string path = tmpFile->fileName().toStdString();

	auto f = std::make_shared<std::ofstream>(path, std::ofstream::binary);
	if (!*f) throw(Exception("Can't open file"));

	return std::make_pair(tmpFile, f);
}

uint16_t MidiParser::getNumberOfMusicTracks() {
	size_t tracks = 0;
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		if (!getPosOfEvents(i, 0xC0u, 0xF0u).empty()) ++tracks;
	}
	return tracks;
}
