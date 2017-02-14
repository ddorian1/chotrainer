#ifndef SELECT_VOICE_DIALOG
#define SELECT_VOICE_DIALOG

#include <QDialog>

class SelectVoiceDialog : public QDialog {
	Q_OBJECT

	private:
		bool trackSet;
		size_t track;

	public:
		SelectVoiceDialog(const std::string &chotrainerFilePath);
		size_t getTrack() const;
		static size_t getVoice(const std::string &midiFilePath);

	public slots:
		void onSetTrack();
};

#endif //SELECT_VOICE_DIALOG
