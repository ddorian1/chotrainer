#include "MainWindow.h"

#include <QVBoxLayout>
#include <QFrame>

MainWindow::MainWindow(const std::vector<uint8_t> &midiData, size_t ownTrack)
:
	ownTrack(ownTrack),
	midiParser(midiData),
	playing(false),
	bOwnVoiceOnly(new QToolButton(this)),
	bOwnVoiceForeground(new QToolButton(this)),
	bNoForeground(new QToolButton(this)),
	bOwnVoiceMute(new QToolButton(this)),
	bPlayStop(new QPushButton(this))
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);
	
	QFrame *sep = new QFrame(this);
	sep->setFrameShape(QFrame::HLine);

	layout->addWidget(bOwnVoiceOnly);
	layout->addWidget(bOwnVoiceForeground);
	layout->addWidget(bNoForeground);
	layout->addWidget(bOwnVoiceMute);
	layout->addWidget(sep);
	layout->addWidget(bPlayStop);
	layout->setAlignment(bPlayStop, Qt::AlignHCenter);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	bOwnVoiceOnly->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	bOwnVoiceForeground->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	bNoForeground->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	bOwnVoiceMute->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	bOwnVoiceOnly->setText(tr("Only my voice"));
	bOwnVoiceForeground->setText(tr("Emphatize my voice"));
	bNoForeground->setText(tr("Don't emphatize my voice"));
	bOwnVoiceMute->setText(tr("Mute my voice"));
	bPlayStop->setText(tr("Play"));
	bPlayStop->setIcon(QIcon::fromTheme("media-playback-start"));

	QObject::connect(bOwnVoiceOnly, &QToolButton::clicked, this, &MainWindow::onOwnVoiceOnly);
	QObject::connect(bOwnVoiceForeground, &QToolButton::clicked, this, &MainWindow::onOwnVoiceForeground);
	QObject::connect(bNoForeground, &QToolButton::clicked, this, &MainWindow::onNoForeground);
	QObject::connect(bOwnVoiceMute, &QToolButton::clicked, this, &MainWindow::onOwnVoiceMute);
	QObject::connect(bPlayStop, &QPushButton::clicked, this, &MainWindow::onPlayStop);

	bOwnVoiceOnly->setCheckable(true);
	bOwnVoiceForeground->setCheckable(true);
	bNoForeground->setCheckable(true);
	bOwnVoiceMute->setCheckable(true);

	onOwnVoiceForeground();
}

void MainWindow::setChecked(const QToolButton *button) {
	bOwnVoiceOnly->setChecked(button == bOwnVoiceOnly);
	bOwnVoiceForeground->setChecked(button == bOwnVoiceForeground);
	bNoForeground->setChecked(button == bNoForeground);
	bOwnVoiceMute->setChecked(button == bOwnVoiceMute);
}

void MainWindow::onPlayStop() {
	if (playing) {
		fluidsynth.stop();
		bPlayStop->setText(tr("Play"));
		bPlayStop->setIcon(QIcon::fromTheme("media-playback-start"));
	} else {
		fluidsynth.play(midiFile->fileName().toStdString());
		bPlayStop->setText(tr("Stop"));
		bPlayStop->setIcon(QIcon::fromTheme("media-playback-stop"));
	}

	playing = !playing;
}

void MainWindow::onOwnVoiceOnly() {
	setChecked(bOwnVoiceOnly);
	midiFile = midiParser.withOnlyVoice(ownTrack);
}

void MainWindow::onOwnVoiceForeground() {
	setChecked(bOwnVoiceForeground);
	midiFile = midiParser.withForegroundVoice(ownTrack);
}

void MainWindow::onNoForeground() {
	setChecked(bNoForeground);
	midiFile = midiParser.withoutForegroundVoice();
}

void MainWindow::onOwnVoiceMute() {
	setChecked(bOwnVoiceMute);
	midiFile = midiParser.withoutVoice(ownTrack);
}
