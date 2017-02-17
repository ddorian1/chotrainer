#include "Fluidsynth.h"
#include "Exception.h"

#include <QTemporaryFile>
#include <QTimer>

Fluidsynth::Fluidsynth()
:
	settings(new_fluid_settings()),
	synth(new_fluid_synth(settings)),
	player(new_fluid_player(synth)),
	adriver(new_fluid_audio_driver(settings, synth)),
	timer(nullptr)
{
	Q_INIT_RESOURCE(core);
	QFile f(":/TimGM6mb.sf2");
	QTemporaryFile *tmpSf = QTemporaryFile::createNativeFile(f);
	const std::string path = tmpSf->fileName().toStdString();
	if (!fluid_is_soundfont(path.c_str())) throw(Exception("Soundfont is invalid"));
	fluid_synth_sfload(synth, path.c_str(), 1);
	delete tmpSf;
}

Fluidsynth::~Fluidsynth() {
	delete_fluid_player(player);
	delete_fluid_audio_driver(adriver);
	delete_fluid_synth(synth);
	delete_fluid_settings(settings);
}

void Fluidsynth::play(const std::string &filePath) {
	if (fluid_player_get_status(player) == FLUID_PLAYER_PLAYING) stop();

	if (!fluid_is_midifile(filePath.c_str())) throw(Exception("File is no midi file"));
	fluid_player_add(player, filePath.c_str());
	fluid_player_play(player);

	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &Fluidsynth::checkStopped);
	timer->start(250);
}

void Fluidsynth::stop() {
	if (timer) {
		delete timer;
		timer = nullptr;
	}
	fluid_player_stop(player);
	fluid_synth_system_reset(synth);
	delete_fluid_player(player);
	player = new_fluid_player(synth);
}

void Fluidsynth::checkStopped() {
	if (fluid_player_get_status(player) != FLUID_PLAYER_PLAYING) {
		stop();
		emit playbackStopped();
	}
}
