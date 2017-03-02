#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fluidsynth.h>
#include <MidiParser.h>

#include <QMainWindow>
#include <string>

class QCheckBox;
class QLineEdit;
class TrackName;

class MainWindow : public QMainWindow {
	Q_OBJECT

	private:
		QLineEdit *pieceName;
		bool playing;
		const std::string midiFilePath;
		MidiParser midiParser;
		Fluidsynth fluidsynth;
		std::shared_ptr<QTemporaryFile> midiFile;
		std::vector<TrackName*> names;
		std::vector<QCheckBox*>	accompaniments;

		void closeEvent(QCloseEvent *event) override;

	public:
		explicit MainWindow(const std::string &midiFile);

	public slots:
		void onSave();
		void onAccompaniment();
		void onPlayStop();

	signals:
		void playbackStopped();
		void playbackStarted(const QObject *clickedButton);
};

#endif //MAIN_WINDOW_H
