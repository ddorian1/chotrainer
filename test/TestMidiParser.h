#ifndef TEST_MIDI_PARSER_H
#define TEST_MIDI_PARSER_H

#include <string>

class TestMidiParser : public QObject {
	Q_OBJECT
	private:
		static bool compareFiles(const std::string &path1, const std::string &path2);

	private slots:
		void setInstrument();
		//void setForegroundVoice();
		//void muteVoice(size_t voice);
		void reset();
};

#endif //TEST_MIDI_PARSER_H
