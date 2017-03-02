#include "MainWindow.h"

#include <ChotrainerParser.h>
#include <Exception.h>

#include <QCheckBox>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

class PlayButton : public QToolButton {
	private:
		const size_t track;

	public:
		PlayButton(QWidget *parent, size_t track) : QToolButton(parent), track(track) {onPlaybackStopped();};
		size_t getTrack() const {return track;};

	public slots:
		void onPlaybackStarted(const QObject *clickedButton) {
			if (dynamic_cast<const PlayButton*>(clickedButton) == this) {
				setIcon(QIcon::fromTheme("media-playback-stop", QIcon(":/icons/media-playback-stop")));
			} else {
				setEnabled(false);
			}
		};

		void onPlaybackStopped() {
			setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/icons/media-playback-start")));
			setEnabled(true);
		};
};

class TrackName : public QLineEdit {
	private:
		const size_t track;

	public:
		TrackName(QWidget *parent, size_t track) : QLineEdit(parent), track(track) {};
		size_t getTrack() const {return track;};
};

MainWindow::MainWindow(const std::string &midiFile)
:
	pieceName(new QLineEdit(this)),
	playing(false),
	midiFilePath(midiFile),
	midiParser(midiFile)
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);

	layout->addWidget(new QLabel(tr("Name of piece"), this));
	layout->addWidget(pieceName);

	QFrame *sep1 = new QFrame(this);
	sep1->setFrameShape(QFrame::HLine);
	layout->addWidget(sep1);

	const std::list<size_t> musicTracks = midiParser.getMusicTracks();

	names.reserve(musicTracks.size());
	for (const auto &track : musicTracks) {
		auto n = new TrackName(this, track);
		n->setToolTip(tr("The name of this voice"));
		names.push_back(n);
	}
	accompaniments.resize(musicTracks.size(), nullptr);
	for (auto &a : accompaniments) {
		a = new QCheckBox(this);
		a->setToolTip(tr("Check if this voice is part of the accompaniment"));
		QObject::connect(a, &QCheckBox::clicked, this, &MainWindow::onAccompaniment);
	}

	QGridLayout *grid = new QGridLayout();
	grid->addWidget(new QLabel(tr("Name of voice"), this), 0, 1);
	grid->addWidget(new QLabel(tr("Accompaniment"), this), 0, 2);
	auto track = musicTracks.begin();
	for (size_t i = 0; i < musicTracks.size(); ++i) {
		PlayButton *pb = new PlayButton(this, *track);
		++track;
		pb->setToolTip(tr("Play this voice"));
		QObject::connect(pb, &QToolButton::clicked, this, &MainWindow::onPlayStop);
		QObject::connect(this, &MainWindow::playbackStarted, pb, &PlayButton::onPlaybackStarted);
		QObject::connect(this, &MainWindow::playbackStopped, pb, &PlayButton::onPlaybackStopped);

		grid->addWidget(names[i], i + 1, 1);
		grid->addWidget(accompaniments[i], i + 1, 2);
		grid->addWidget(pb, i + 1, 3);
	}
	layout->addLayout(grid);
	
	QFrame *sep2 = new QFrame(this);
	sep2->setFrameShape(QFrame::HLine);
	layout->addWidget(sep2);

	QHBoxLayout *l = new QHBoxLayout();
	QPushButton *bClose = new QPushButton(tr("Close"));
	QPushButton *bSave = new QPushButton(tr("Save"));
	bClose->setIcon(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit")));
	bSave->setIcon(QIcon::fromTheme("document-save", QIcon(":icons/document-save")));
	l->addWidget(bClose);
	l->addWidget(bSave);
	layout->addLayout(l);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	QObject::connect(bClose, &QPushButton::clicked, this, &QMainWindow::close);
	QObject::connect(bSave, &QPushButton::clicked, this, &MainWindow::onSave);
	QObject::connect(&fluidsynth, &Fluidsynth::playbackStopped, this, &MainWindow::onPlayStop);

	QTimer::singleShot(0, this, [=](){setFixedSize(size());});
}

void MainWindow::onSave() {
	const std::string filePath = QFileDialog::getSaveFileName(this, QObject::tr("Save File"), QString("%1/%2").arg(QDir::homePath(), pieceName->text()), QString("%1 (*.chotrainer)").arg(QObject::tr("Chotrainer file"))).toStdString();
	if (filePath == "") return;

	std::vector<ChotrainerParser::Track> namedTracks;
	for (size_t i = 0; i < names.size(); ++i) {
		if (accompaniments.at(i)->isChecked()) continue;
		namedTracks.emplace_back(names.at(i)->getTrack(), names.at(i)->text().toStdString());
	}

	try {
		ChotrainerParser::createNewFile(namedTracks, pieceName->text().toStdString(), midiParser.getData(), filePath);
	} catch (const Exception &e) {
		QMessageBox::warning(this, QObject::tr("Warning"), QObject::tr("Can't save to file:\n%1").arg(e.what()));
	}
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
		QObject *clickedObject = QObject::sender();
		PlayButton *clickedButton = dynamic_cast<PlayButton*>(clickedObject);
		emit playbackStarted(clickedObject);
		try {
			midiFile = midiParser.withOnlyVoice(clickedButton->getTrack());
		} catch (const Exception &e) {
			QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Can't create midi file:\n%1").arg(e.what()));
			QCoreApplication::instance()->quit();
		}
		fluidsynth.play(midiFile->fileName().toStdString());
	}

	playing = !playing;
}

void MainWindow::closeEvent(QCloseEvent *event) {
	event->accept();
	QCoreApplication::instance()->quit();
}
