#ifndef SELECT_VOICE_DIALOG
#define SELECT_VOICE_DIALOG

#include <QDialog>
#include <ChotrainerParser.h>

class SelectVoiceDialog : public QDialog {
	Q_OBJECT

	private:
		bool trackSet;
		ChotrainerParser::Track track;

	public:
		explicit SelectVoiceDialog(const std::vector<ChotrainerParser::Track> &namedTracks);
		ChotrainerParser::Track getTrack() const;
		static ChotrainerParser::Track getVoice(const std::vector<ChotrainerParser::Track> &namedTracks);

	public slots:
		void onSetTrack();
};

#endif //SELECT_VOICE_DIALOG
