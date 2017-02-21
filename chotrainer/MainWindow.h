#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fluidsynth.h>
#include <MidiParser.h>

#include <ChotrainerParser.h>
#include <QMainWindow>
#include <QPushButton>
#include <QSpinBox>
#include <QTemporaryFile>
#include <QToolButton>
#include <memory>
#include <string>

class MainWindow : public QMainWindow {
	Q_OBJECT

	private:
		const size_t ownTrack;
		MidiParser midiParser;
		Fluidsynth fluidsynth;
		std::shared_ptr<QTemporaryFile> midiFile;
		bool playing;

		QToolButton *bOwnVoiceOnly;
		QToolButton *bOwnVoiceForeground;
		QToolButton *bNoForeground;
		QToolButton *bOwnVoiceMute;
		QSpinBox *sBar;
		QPushButton *bPlayStop;
		
		void setChecked(const QToolButton *button);
		void updateMidiFile();

		void closeEvent(QCloseEvent *event) override;

	public:
		MainWindow(const ChotrainerParser &cp, const ChotrainerParser::Track &ownTrack);

	public slots:
		void onPlayStop();
		void onOwnVoiceOnly();
		void onOwnVoiceForeground();
		void onNoForeground();
		void onOwnVoiceMute();
};

#endif //MAIN_WINDOW_H
