#ifndef FLUIDSYNTH_H
#define FLUIDSYNTH_H

#include <fluidsynth.h>
#include <string>

class Fluidsynth {
	private:
		fluid_settings_t* settings;
		fluid_synth_t* synth;
		fluid_player_t* player;
		fluid_audio_driver_t* adriver;

	public:
		Fluidsynth();
		~Fluidsynth();
		void play(const std::string &filePath);
		void stop();
};

#endif //FLUIDSYNTH_H
