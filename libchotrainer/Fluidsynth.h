#ifndef FLUIDSYNTH_H
#define FLUIDSYNTH_H

#include <QObject>
#include <fluidsynth.h>
#include <string>

class QTimer;

class Fluidsynth : public QObject {
	Q_OBJECT;

	private:
		fluid_settings_t* settings;
		fluid_synth_t* synth;
		fluid_player_t* player;
		fluid_audio_driver_t* adriver;
		QTimer *timer;

	public:
		Fluidsynth();
		~Fluidsynth();
		void play(const std::string &filePath);
		void stop();

	private slots:
		void checkStopped();

	signals:
		void playbackStopped();
};

#endif //FLUIDSYNTH_H
