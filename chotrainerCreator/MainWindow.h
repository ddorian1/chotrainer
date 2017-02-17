#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fluidsynth.h>
#include <MidiParser.h>

#include <QCheckBox>
#include <QMainWindow>
#include <QLineEdit>
#include <string>

class TrackName;

class MainWindow : public QMainWindow {
	Q_OBJECT

	private:
		bool playing;
		const std::string midiFilePath;
		MidiParser midiParser;
		Fluidsynth fluidsynth;
		std::shared_ptr<QTemporaryFile> midiFile;
		std::vector<TrackName*> names;
		std::vector<QCheckBox*>	accompaniments;

		void closeEvent(QCloseEvent *event) override;

	public:
		MainWindow(const std::string &midiFile);

	public slots:
		void onSave();
		void onAccompaniment();
		void onPlayStop();

	signals:
		void playbackStopped();
		void playbackStarted(const QObject *clickedButton);
};

#endif //MAIN_WINDOW_H
