#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fluidsynth.h>
#include <MidiParser.h>

#include <memory>
#include <QMainWindow>
#include <QPushButton>
#include <QTemporaryFile>
#include <QToolButton>
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
		QPushButton *bPlayStop;
		
		void setChecked(const QToolButton *button);

	public:
		MainWindow(const std::vector<uint8_t> &midiData, size_t ownTrack);

	public slots:
		void onPlayStop();
		void onOwnVoiceOnly();
		void onOwnVoiceForeground();
		void onNoForeground();
		void onOwnVoiceMute();
};

#endif //MAIN_WINDOW_H
