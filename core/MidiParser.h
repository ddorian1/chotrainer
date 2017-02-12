#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H

#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <QTemporaryFile>

class MidiParser {
	private:
		/**
		 * Representing an midi event.
		 * ptr points to the event in data,
		 * tick is the absolute tick the event occures at.
		 * deltaTick is a pointer to the begin of events deltatick
		 */
		struct Event {
			uint8_t *ptr;
			unsigned long long tick;
			uint8_t *deltaTick;
			Event(uint8_t *p, unsigned long long t, uint8_t *dt) : ptr(p), tick(t), deltaTick(dt) {}
		};

		std::vector<uint8_t> data;

		/**
		 * Set Instrument of track, counting tracks from 0.
		 * Throws in case of error.
		 */
		void setInstrument(size_t track, uint8_t instrumentId, const std::string &instrument);

		void setInstrumentId(size_t track, uint8_t instrumentId);

		void setInstrumentName(size_t track, const std::string &instrumentName);

		/**
		 * Get start of track, counting from 0.
		 * Returns nullptr if track can't be found.
		 */
		uint8_t* getTrackPos(size_t track);

		/**
		 * Get midi ticks till given bar.
		 * Bars are counted from 0?
		 * Partials aren't supported jet.
		 * Warning: returned tick may be after end of midi file!
		 */
		unsigned long long ticksTillBar(size_t bar);

		/**
		 * Read ticks per quarter note from header.
		 */
		unsigned int getTicksPerQuarterNote();

		/**
		 * Write track to file, possiblie skipping bars.
		 */
		void writeTrack(std::shared_ptr<std::ofstream> f, size_t track, size_t fromBar);

		/**
		 * Get pos of instrument meta event in track, counting tracks from 0.
		 * Returns nullptr if not found.
		 */
		uint8_t* getInstrumentPos(size_t track);

		std::list<Event> getEvents(size_t track, uint8_t event, uint8_t mask = 0xFFu);

		/**
		 * Interpret v_length value pointed to by p.
		 * Returns length as first value,
		 * number of bytes used by v_length as second value of pair.
		 * Throws if EOF is reached.
		 */
		std::pair<size_t, size_t> sizeTFromVLength(const uint8_t *p) const;

		/**
		 * Interpret v_length value pointed to by p.
		 * Returns length as first value,
		 * number of bytes used by v_length as second value of pair.
		 */
		static std::vector<uint8_t> sizeTToVLength(size_t length);

		/**
		 * Get bytes till end of data, including byte pointed to.
		 * Throws if pointer doesn't point into data.
		 */
		size_t getBytesTillEnd(const uint8_t *p) const;

		size_t getBytesTillTrackEnd(const uint8_t *p); //TODO const

		void setNoForegroundVoice();
		void setForegroundVoice(size_t track);

		std::pair<std::shared_ptr<QTemporaryFile>, std::shared_ptr<std::ofstream>> newTmpFile() const;

		/**
		 * Read file and return content.
		 */
		static std::vector<uint8_t> readFile(const std::string &filePath);

	public:
		/**
		 * Throws if file can't be read.
		 */
		MidiParser(const std::string &filePath);
		MidiParser(const std::vector<uint8_t> &midiData);

		std::shared_ptr<QTemporaryFile> withOnlyVoice(size_t track, size_t fromBar = 0);
		std::shared_ptr<QTemporaryFile> withForegroundVoice(size_t track, size_t fromBar = 0);
		std::shared_ptr<QTemporaryFile> withoutForegroundVoice(size_t fromBar = 0);
		std::shared_ptr<QTemporaryFile> withoutVoice(size_t track, size_t fromBar = 0);
		std::list<size_t> getMusicTracks();
};

#endif //MIDI_PARSER_H
