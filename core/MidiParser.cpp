#include "Exception.h"
#include "MidiParser.h"

#include <QCoreApplication>
#include <QDir>

#include <cmath>
#include <cstring>
#include <limits>
#include <list>
#include <tuple>

std::vector<uint8_t> MidiParser::readFile(const std::string &filePath) {
	std::ifstream file(filePath, std::ifstream::binary);
	if (!file) throw(Exception(std::string("Can't open file ").append(filePath)));
	file.seekg(0, file.end);
	const size_t fileSize = file.tellg();
	file.seekg(0, file.beg);
	std::vector<uint8_t> data(fileSize);
	file.read(reinterpret_cast<char*>(data.data()), fileSize);
	if (!file) throw(Exception(std::string("Can't read file ").append(filePath)));
	return data;
}

MidiParser::MidiParser(const std::string &filePath)
:
	MidiParser(readFile(filePath), std::vector<ChotrainerParser::Track>())
{}

MidiParser::MidiParser(const std::vector<uint8_t> &midiData, const std::vector<ChotrainerParser::Track> &namedTracks)
:
	data(midiData),
	namedTracks(namedTracks)
{
	if (data.size() < 14) throw(Exception("Data to short"));
	if (data[8] != 0x00u || data[9] != 0x01u) throw(Exception("Only format 1 files are supported"));
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

std::list<MidiParser::Event> MidiParser::getEvents(size_t track, uint8_t event, uint8_t mask) {
	std::list<Event> events;

	uint8_t *p = getTrackPos(track);
	size_t trackLen = 0;
	trackLen |= *(p + 4) << 24;
	trackLen |= *(p + 5) << 16;
	trackLen |= *(p + 6) << 8;
	trackLen |= *(p + 7);
	const uint8_t *nextTrack = getTrackPos(track + 1);
	const uint8_t *trackEnd = p + 8 + trackLen;
	if (nextTrack) {
		if (nextTrack != trackEnd) throw(Exception("File inconsistent"));
	} else {
		if (getBytesTillEnd(trackEnd - 1) != 1)  throw(Exception("File inconsistent"));
	}

	p += 8; //Skip "MTrk" and length
	unsigned long long tick = 0;
	while (p < trackEnd) {
		/* Skip delta_time */
		size_t vLengthBytes;
		size_t deltaTicks;
		std::tie(deltaTicks, vLengthBytes) = sizeTFromVLength(p);
		p += vLengthBytes;
		if (p >= trackEnd) throw(Exception("Invalid file"));
		tick += deltaTicks;
		if (tick < deltaTicks) throw(Exception("Integer overflow (tick)"));

		/* Check if event matches search */
		if (event == (*p & mask)) events.emplace_back(p, tick, p - vLengthBytes);

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
	std::list<Event> metaEvents = getEvents(track, 0xFFu);
	for (const auto &event : metaEvents) {
		if (*(event.ptr + 1) == 0x04u)
			return event.ptr;
	}
	return nullptr;
}

unsigned long long MidiParser::ticksTillBar(size_t bar) {
	std::list<Event> metaEvents = getEvents(0, 0xFFu);
	metaEvents.emplace_back(nullptr, std::numeric_limits<unsigned long long>::max(), nullptr);

	Event lastTimeSignature(nullptr, 0, nullptr);
	unsigned long long ticks = 0;

	for (const auto &event : metaEvents) {
		if (event.ptr && (event.ptr[1] != 0x58u || event.ptr[2] != 0x04u)) continue;

		if (lastTimeSignature.ptr == nullptr) {
			if (event.tick != 0) throw(Exception("No time signature specified"));
			lastTimeSignature = event;
			continue;
		}

		uint8_t *p = lastTimeSignature.ptr;
		double quarterNotesPerBar = static_cast<double>(p[3]) * std::pow(2., 2. - static_cast<double>(p[4]));
		double ticksPerBarD = quarterNotesPerBar * getTicksPerQuarterNote();
		unsigned long long ticksPerBar = std::llrint(ticksPerBarD);
		if (static_cast<double>(ticksPerBar) != ticksPerBarD) {
			throw(Exception("ticksPerBarD is fractional"));
		}

		double barsInOldTimeD = static_cast<double>(event.tick - lastTimeSignature.tick) / ticksPerBarD;
		unsigned long long barsInOldTime = std::llrint(barsInOldTimeD);
		if (static_cast<double>(barsInOldTime) != barsInOldTimeD) {
			throw(Exception("barsInOldTimeD is fractional"));
		}

		if (barsInOldTime >= bar) {
			ticks += bar * ticksPerBar;
			return ticks;
		} else {
			bar -= barsInOldTime;
			ticks += barsInOldTime * ticksPerBar;
		}
	}
	throw(Exception("Should not be reached"));
}

void MidiParser::setInstrument(size_t track, uint8_t instrumentId, const std::string &instrumentName) {
	setInstrumentName(track, instrumentName);
	setInstrumentId(track, instrumentId);
}

void MidiParser::setInstrumentId(size_t track, uint8_t instrumentId) {
	std::list<Event> programChangeEvents = getEvents(track, 0xC0u, 0xF0u);
	for (const auto &event : programChangeEvents)
		*(event.ptr + 1) = instrumentId;
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
	const unsigned long newTrackSize = static_cast<unsigned long>(nextTrackPos - trackPos - 8);
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
	for (auto &i : v) {
		i = l.front();
		l.pop_front();
	}
	return v;
}

size_t MidiParser::getBytesTillEnd(const uint8_t *p) const {
	const uint8_t *end = data.data() + data.size();
	if (p - data.data() < 0 || end - p < 1) throw(Exception("p not in data"));
	return static_cast<size_t>(end - p);
}

unsigned int MidiParser::getTicksPerQuarterNote() {
	if (data[12] & 0x80u) throw(Exception("Format not supported"));
	unsigned int ticks = data[12];
	ticks <<= 8;
	ticks += data[13];
	return ticks;
}

size_t MidiParser::getBytesTillTrackEnd(const uint8_t *p) {
	const uint8_t *t;
	size_t i;
	for (i = 0, t = getTrackPos(i); t != nullptr; ++i, t = getTrackPos(i))
		if (t > p) return t - p;
	return getBytesTillEnd(p);
}

void MidiParser::setNoForegroundVoice() {
	std::vector<ChotrainerParser::Track> namTmp = namedTracks;
	std::sort(namTmp.begin(), namTmp.end(), [](ChotrainerParser::Track i, ChotrainerParser::Track j){return i.number > j.number;});
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		try {
			if (i == namTmp.back().number) {
				setInstrument(i, 0x35u, "voice oohs");
				namTmp.pop_back();
			} else {
				setInstrument(i, 0x00u, "acoustic grand");
			}
		} catch (const Exception &e) {}
	}
}

void MidiParser::setForegroundVoice(size_t track) {
	setNoForegroundVoice();
	setInstrument(track, 0x38u, "trumpet");
	//setInstrument(track, 0x34u, "choir aahs");
}

std::shared_ptr<QTemporaryFile> MidiParser::withOnlyVoice(size_t track, size_t fromBar) {
	if (track == 0) throw(Exception("Only Control track makes no sence?"));

	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	const std::vector<uint8_t> header = {
		'M', 'T', 'h', 'd', //Type
		0x00, 0x00, 0x00, 0x06, //Length
		0x00, 0x01, //Format
		0x00, 0x02, //Tracks
		data.at(12), data.at(13) //Division
	};
	f->write(reinterpret_cast<const char*>(header.data()), header.size());
	if (!*f) throw(Exception("Can't write to file"));

	std::vector<size_t> tracks = {0, track};
	setInstrument(track, 0x00u, "acoustic grand");
	for (const auto &tr : tracks) writeTrack(f, tr, fromBar);

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withForegroundVoice(size_t track, size_t fromBar) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	setForegroundVoice(track);
	f->write(reinterpret_cast<char*>(data.data()), 14); //TODO read length from Header
	if (!*f) throw(Exception("Can't write to file"));
	for (size_t i = 0; getTrackPos(i); ++i) writeTrack(f, i, fromBar);

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withoutForegroundVoice(size_t fromBar) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	setNoForegroundVoice();
	f->write(reinterpret_cast<char*>(data.data()), 14); //TODO read length from Header
	if (!*f) throw(Exception("Can't write to file"));
	for (size_t i = 0; getTrackPos(i); ++i) writeTrack(f, i, fromBar);

	return tmpFile;
}

std::shared_ptr<QTemporaryFile> MidiParser::withoutVoice(size_t track, size_t fromBar) {
	std::shared_ptr<QTemporaryFile> tmpFile;
	std::shared_ptr<std::ofstream> f;
	tie(tmpFile, f) = newTmpFile();

	std::vector<uint8_t> header(14); //TODO read length from header
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
	for (size_t i = 0; getTrackPos(i); ++i) {
		if (i == track) continue;
		writeTrack(f, i, fromBar);
	}

	return tmpFile;
}

void MidiParser::writeTrack(std::shared_ptr<std::ofstream> f, size_t track, size_t fromBar) {
	uint8_t *data = getTrackPos(track);
	size_t length = getBytesTillTrackEnd(data);

	std::vector<uint8_t> tmp;
	if (fromBar != 0) {
		unsigned long long fromTick = ticksTillBar(fromBar);
		tmp = std::vector<uint8_t>({'M', 'T', 'r', 'k', 0x00u, 0x00u, 0x00u, 0x00u});
		tmp.reserve(length);
		std::list<Event> events = getEvents(track, 0x00u, 0x00u);
		for (auto event = events.begin(); event != events.end(); ++event) {
			/* Skip note on events */
			if ((*(event->ptr) & 0xF0u) == 0x90u) {
				auto nextEvent = event;
				++nextEvent;
				if (nextEvent->tick <= fromTick) continue;
				//TODO check if nextEvent is note on/off event?
			}

			/* Add delta time */
			if (event->tick < fromTick) {
				tmp.push_back(0x00u);
			} else {
				size_t delta, vLengthSize;
				std::tie(delta, vLengthSize) = sizeTFromVLength(event->deltaTick);
				if (event->tick - delta < fromTick) {
					for (const auto &i : sizeTToVLength(event->tick - fromTick)) tmp.push_back(i);
				} else {
					for (size_t i = 0; i < vLengthSize; ++i) tmp.push_back(event->deltaTick[i]);
				}
			}

			/* Add event */
			size_t eventSize;
			auto nextEvent = event;
			++nextEvent;
			if (nextEvent != events.end()) {
				eventSize = nextEvent->deltaTick - event->ptr;
			} else {
				eventSize = getBytesTillTrackEnd(event->ptr);
			}
			for (size_t i = 0; i < eventSize; ++i) tmp.push_back(event->ptr[i]);
		}

		data = tmp.data();
		length = tmp.size();

		unsigned long trackLength = length - 8;
		if (trackLength > length) throw(Exception("Integer overflow (trackLength)"));
		*(data + 4) = (trackLength & 0xFF000000lu) >> 24;
		*(data + 5) = (trackLength & 0x00FF0000lu) >> 16;
		*(data + 6) = (trackLength & 0x0000FF00lu) >> 8;
		*(data + 7) = trackLength & 0x000000FFlu;
	}

	f->write(reinterpret_cast<char*>(data), length);
	if (!*f) throw(Exception("Can't write to file"));
}

std::pair<std::shared_ptr<QTemporaryFile>, std::shared_ptr<std::ofstream>> MidiParser::newTmpFile() const {
	auto tmpFile = std::make_shared<QTemporaryFile>(QDir::cleanPath(QString("%1/%2-XXXXXX.midi").arg(QDir::tempPath(), QCoreApplication::applicationName())));
	tmpFile->open();

	const std::string path = tmpFile->fileName().toStdString();

	auto f = std::make_shared<std::ofstream>(path, std::ofstream::binary);
	if (!*f) throw(Exception("Can't open file"));

	return std::make_pair(tmpFile, f);
}

std::list<size_t> MidiParser::getMusicTracks() {
	std::list<size_t> l;
	for (size_t i = 0; getTrackPos(i) != nullptr; ++i) {
		if (!getEvents(i, 0xC0u, 0xF0u).empty()) l.push_back(i);
	}
	return l;
}

std::vector<uint8_t> MidiParser::getData() const {
	return data;
}
