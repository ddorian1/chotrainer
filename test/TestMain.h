#ifndef TEST_MAIN_H
#define TEST_MAIN_H

#include <string>
#include <QtTest/QtTest>

class TestMain : public QObject {
	Q_OBJECT

	private slots:
		void midiParser();
		//void setForegroundVoice();
		//void muteVoice(size_t voice);
};

#endif //TEST_MAIN_H
