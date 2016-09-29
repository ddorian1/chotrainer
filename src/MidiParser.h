#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H

#include <string>
#include <vector>
//#include <QtTempFile>

class MidiParser {
	private:
		std::ifstream file;
		std::vector<char> data;
		std::vector<bool> voiceMuted;
		//QTempfile tmpFile;

		/**
		 * Set Instrument of track, counting tracks from 0.
		 * Throws in case of error.
		 */
		void setInstrument(size_t track, const std::string &instrument);

		/**
		 * Get start of track, counting from 0.
		 * Returns nullptr if track can't be found.
		 */
		char* getTrackStart(size_t track) const;

		/**
		 * Get pos of instrument meta event in track, counting tracks from 0.
		 * Returns nullptr if not found.
		 */
		char* getInstrumentPos(size_t track) const;

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

	public:
		/**
		 * Throws if file can't be read.
		 */
		MidiParser(const std::string &filePath);
		void setForegroundVoice(size_t track);
		void muteVoice(size_t track);
		void unmuteAll();
};

#endif //MIDI_PARSER_H
