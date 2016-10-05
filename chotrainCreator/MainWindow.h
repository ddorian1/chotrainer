#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Fluidsynth.h>
#include <MidiParser.h>

#include <QCheckBox>
#include <QMainWindow>
#include <QLineEdit>
#include <string>

class MainWindow : public QMainWindow {
	Q_OBJECT

	private:
		const std::string midiFilePath;
		std::vector<QLineEdit*> names;
		std::vector<QCheckBox*>	accompaniments;

	public:
		MainWindow(const std::string &midiFile);

	public slots:
		void onSave();
		void onAccompaniment();
};

#endif //MAIN_WINDOW_H
