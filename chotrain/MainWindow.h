#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QPushButton>

class MainWindow : public QMainWindow {
	Q_OBJECT

	private:
		const size_t ownTrack;
		bool playing;

		QToolButton *bOwnVoiceForeground;
		QToolButton *bNoForeground;
		QToolButton *bOwnVoiceMute;
		QPushButton *bPlayStop;

	public:
		MainWindow(size_t ownTrack);

	public slots:
		void onPlayStop();
		void onOwnVoiceForeground();
		void onNoForeground();
		void onOwnVoiceMute();
};

#endif //MAIN_WINDOW_H
