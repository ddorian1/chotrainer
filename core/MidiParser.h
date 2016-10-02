#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <QTemporaryFile>

class MidiParser {
	private:
		std::ifstream file;
		std::vector<char> data;
		std::vector<bool> voiceMuted;

		/**
		 * Set Instrument of track, counting tracks from 0.
		 * Throws in case of error.
		 */
		void setInstrument(size_t track, unsigned char instrumentId, const std::string &instrument);

		void setInstrumentId(size_t track, unsigned char instrumentId);

		void setInstrumentName(size_t track, const std::string &instrumentName);

		/**
		 * Get start of track, counting from 0.
		 * Returns nullptr if track can't be found.
		 */
		char* getTrackPos(size_t track);

		/**
		 * Get pos of instrument meta event in track, counting tracks from 0.
		 * Returns nullptr if not found.
		 */
		char* getInstrumentPos(size_t track);

		std::list<char*> getPosOfEvents(size_t track, unsigned char event, unsigned char mask = 0xFFu);

		/**
		 * Interpret v_length value pointed to by p.
		 * Returns length as first value,
		 * number of bytes used by v_length as second value of pair.
		 * Throws if EOF is reached.
		 */
		std::pair<size_t, size_t> sizeTFromVLength(const char *p) const;

		/**
		 * Interpret v_length value pointed to by p.
		 * Returns length as first value,
		 * number of bytes used by v_length as second value of pair.
		 */
		static std::vector<unsigned char> sizeTToVLength(size_t length);

		/**
		 * Get bytes till end of data, including byte pointed to.
		 * Throws if pointer doesn't point into data.
		 */
		size_t getBytesTillEnd(const char *p) const;

		size_t getBytesTillTrackEnd(const char *p); //TODO const

		void setNoForegroundVoice();
		void setForegroundVoice(size_t track);

		std::pair<std::shared_ptr<QTemporaryFile>, std::shared_ptr<std::ofstream>> newTmpFile() const;

	public:
		/**
		 * Throws if file can't be read.
		 */
		MidiParser(const std::string &filePath);

		std::shared_ptr<QTemporaryFile> withOnlyVoice(size_t track);
		std::shared_ptr<QTemporaryFile> withForegroundVoice(size_t track);
		std::shared_ptr<QTemporaryFile> withoutForegroundVoice();
		std::shared_ptr<QTemporaryFile> withoutVoice(size_t track);
};

#endif //MIDI_PARSER_H
