#ifndef SELECT_VOICE_DIALOG
#define SELECT_VOICE_DIALOG

#include <QDialog>
#include <ChotrainerParser.h>

class SelectVoiceDialog : public QDialog {
	Q_OBJECT

	private:
		bool trackSet;
		size_t track;

	public:
		explicit SelectVoiceDialog(const std::vector<ChotrainerParser::Track> &namedTracks);
		size_t getTrack() const;
		static size_t getVoice(const std::vector<ChotrainerParser::Track> &namedTracks);

	public slots:
		void onSetTrack();
};

#endif //SELECT_VOICE_DIALOG
