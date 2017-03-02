#ifndef CHOTRAINER_PARSER_H
#define CHOTRAINER_PARSER_H

#include <string>
#include <vector>

class ChotrainerParser {
	public:
		struct Track {
			size_t number;
			std::string name;
			Track() = default;
			Track(size_t a, const std::string &b) : number(a), name(b) {}
			bool operator==(const Track &t) const {
				return (t.number == number && t.name == name);
			}
		};

	private:
		std::string pieceName;
		std::vector<Track> namedTracks;
		std::vector<uint8_t> midiFile;

	public:
		explicit ChotrainerParser(const std::string &filePath);
		std::string getPieceName() const;
		std::vector<Track> getNamedTracks() const;
		std::vector<uint8_t> getMidiFile() const;
		static void createNewFile(const std::vector<Track> &namedTracks, const std::string &pieceName, const std::vector<uint8_t> &midiFile, const std::string &filePath);
};

#endif //CHOTRAINER_PARSER_H
