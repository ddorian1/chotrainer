#ifndef CHOTRAINER_PARSER_H
#define CHOTRAINER_PARSER_H

#include <string>
#include <vector>

class ChotrainerParser {
	public:
		struct Track {
			size_t number;
			std::string name;
			Track(size_t a, const std::string &b) : number(a), name(b) {}
			bool operator==(const Track &t) const {
				return (t.number == number && t.name == name);
			}
		};

	private:
		std::vector<Track> namedTracks;
		std::vector<uint8_t> midiFile;

	public:
		ChotrainerParser(const std::string &filePath);
		std::vector<Track> getNamedTracks() const;
		std::vector<uint8_t> getMidiFile() const;
		static void createNewFile(const std::vector<Track> &namedTracks, const std::vector<uint8_t> &midiFile, const std::string &filePath);
};

#endif //CHOTRAINER_PARSER_H
