#include "MainWindow.h"

#include <ChotrainParser.h>

#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QFileDialog>
#include <QFrame>

class PlayButton : public QToolButton {
	private:
		const size_t track;

	public:
		PlayButton(QWidget *parent, size_t track) : QToolButton(parent), track(track) {onPlaybackStopped();};
		size_t getTrack() const {return track;};
		void onPlaybackStarted() {setIcon(QIcon::fromTheme("media-playback-stop"));};

	public slots:
		void onPlaybackStopped() {setIcon(QIcon::fromTheme("media-playback-start"));};
};

MainWindow::MainWindow(const std::string &midiFile)
:
	playing(false),
	midiFilePath(midiFile),
	midiParser(midiFile)
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);

	const std::list<size_t> musicTracks = midiParser.getMusicTracks();

	names.resize(musicTracks.size(), nullptr);
	for (auto &n : names) n = new QLineEdit(this);
	accompaniments.resize(musicTracks.size(), nullptr);
	for (auto &a : accompaniments) {
		a = new QCheckBox(this);
		QObject::connect(a, &QCheckBox::clicked, this, &MainWindow::onAccompaniment);
	}

	QGridLayout *grid = new QGridLayout();
	grid->addWidget(new QLabel(tr("Name"), this), 0, 1);
	grid->addWidget(new QLabel(tr("Is accompaniment"), this), 0, 2);
	auto track = musicTracks.begin();
	for (size_t i = 0; i < musicTracks.size(); ++i) {
		PlayButton *pb = new PlayButton(this, *track);
		++track;
		QObject::connect(pb, &QToolButton::clicked, this, &MainWindow::onPlayStop);
		QObject::connect(this, &MainWindow::playbackStopped, pb, &PlayButton::onPlaybackStopped);

		grid->addWidget(names[i], i + 1, 1);
		grid->addWidget(accompaniments[i], i + 1, 2);
		grid->addWidget(pb, i + 1, 3);
	}
	layout->addLayout(grid);
	
	QFrame *sep = new QFrame(this);
	sep->setFrameShape(QFrame::HLine);
	layout->addWidget(sep);

	QHBoxLayout *l = new QHBoxLayout();
	QPushButton *bCancel = new QPushButton(tr("Cancel"));
	QPushButton *bSave = new QPushButton(tr("Save"));
	l->addWidget(bCancel);
	l->addWidget(bSave);
	layout->addLayout(l);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	QObject::connect(bCancel, &QPushButton::clicked, this, &QMainWindow::close);
	QObject::connect(bSave, &QPushButton::clicked, this, &MainWindow::onSave);
}

void MainWindow::onSave() {
	const std::string filePath = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save File"), QDir::homePath(), QString("%1 (*.midi)").arg(QObject::tr("Midi file"))).toStdString();
	if (filePath == "") return;

	std::vector<ChotrainParser::Track> namedTracks;
	for (size_t i = 0; i < names.size(); ++i) {
		if (accompaniments.at(i)->isChecked()) continue;
		namedTracks.emplace_back(i, names.at(i)->text().toStdString());
	}

	//TODO get from midiParser
	std::ifstream f(midiFilePath, std::ifstream::binary);
	f.seekg(0, f.end);
	std::vector<uint8_t> data(f.tellg());
	f.seekg(0, f.beg);
	f.read(reinterpret_cast<char*>(data.data()), data.size());

	ChotrainParser::createNewFile(namedTracks, data, filePath);
}

void MainWindow::onAccompaniment() {
	for (size_t i = 0; i < names.size(); ++i) {
		if (accompaniments.at(i)->isChecked()) {
			names.at(i)->setEnabled(false);
		} else {
			names.at(i)->setEnabled(true);
		}
	}
}

void MainWindow::onPlayStop() {
	if (playing) {
		emit playbackStopped();
		fluidsynth.stop();
	} else {
		PlayButton *pb = dynamic_cast<PlayButton*>(QObject::sender());
		pb->onPlaybackStarted();
		midiFile = midiParser.withOnlyVoice(pb->getTrack());
		fluidsynth.play(midiFile->fileName().toStdString());
	}

	playing = !playing;
}

void MainWindow::closeEvent(QCloseEvent *event) {
	event->accept();
	QCoreApplication::instance()->quit();
}

