#ifndef CHOTRAIN_PARSER_H
#define CHOTRAIN_PARSER_H

#include <string>
#include <vector>

class ChotrainParser {
	public:
		struct Track {
			size_t number;
			std::string name;
		};

	private:
		std::vector<Track> namedTracks;
		std::vector<uint8_t> midiFile;

	public:
		ChotrainParser(const std::string &filePath);
		std::vector<Track> getNamedTracks() const;
		std::vector<uint8_t> getMidiFile() const;
		static void createNewFile(const std::vector<Track> &namedTracks, const std::vector<uint8_t> &midiFile, const std::string &filePath);
};

#endif //CHOTRAIN_PARSER_H
