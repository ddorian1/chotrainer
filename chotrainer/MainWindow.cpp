#include "MainWindow.h"

#include <Exception.h>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(const ChotrainerParser &cp, const ChotrainerParser::Track &ownTrack)
:
	ownTrack(ownTrack.number),
	midiParser(cp.getMidiFile(), cp.getNamedTracks()),
	playing(false),
	bOwnVoiceOnly(new QToolButton(this)),
	bOwnVoiceForeground(new QToolButton(this)),
	bNoForeground(new QToolButton(this)),
	bOwnVoiceMute(new QToolButton(this)),
	sBar(new QSpinBox(this)),
	bPlayStop(new QPushButton(this))
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);

	QLabel *pieceName = new QLabel(QString::fromStdString(cp.getPieceName()), this);
	QLabel *voiceName = new QLabel(QString::fromStdString(ownTrack.name), this);
	pieceName->setAlignment(Qt::AlignCenter);
	voiceName->setAlignment(Qt::AlignCenter);
	pieceName->setStyleSheet(QString("font-size: %1pt;").arg(QFont().pointSize()*1.5));
	voiceName->setStyleSheet("font-style: italic;");
	
	QFrame *sep = new QFrame(this);
	sep->setFrameShape(QFrame::HLine);

	QHBoxLayout *barLayout = new QHBoxLayout();
	QLabel *barLabel = new QLabel(tr("From bar"), this);
	barLabel->setAlignment(Qt::AlignRight);
	barLayout->addWidget(barLabel, Qt::AlignRight);
	barLayout->addWidget(sBar, Qt::AlignLeft);
	sBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	barLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	layout->addWidget(pieceName);
	layout->addWidget(voiceName);
	layout->addWidget(bOwnVoiceOnly);
	layout->addWidget(bOwnVoiceForeground);
	layout->addWidget(bNoForeground);
	layout->addWidget(bOwnVoiceMute);
	layout->addLayout(barLayout);
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
	bPlayStop->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/icons/media-playback-start")));

	QObject::connect(bOwnVoiceOnly, &QToolButton::clicked, this, &MainWindow::onOwnVoiceOnly);
	QObject::connect(bOwnVoiceForeground, &QToolButton::clicked, this, &MainWindow::onOwnVoiceForeground);
	QObject::connect(bNoForeground, &QToolButton::clicked, this, &MainWindow::onNoForeground);
	QObject::connect(bOwnVoiceMute, &QToolButton::clicked, this, &MainWindow::onOwnVoiceMute);
	QObject::connect(bPlayStop, &QPushButton::clicked, this, &MainWindow::onPlayStop);
	connect(&fluidsynth, &Fluidsynth::playbackStopped, this, &MainWindow::onPlayStop);

	bOwnVoiceOnly->setCheckable(true);
	bOwnVoiceForeground->setCheckable(true);
	bNoForeground->setCheckable(true);
	bOwnVoiceMute->setCheckable(true);

	sBar->setRange(1, 9999);
	sBar->setValue(1);

	onOwnVoiceForeground();
	QTimer::singleShot(0, this, [=](){setFixedSize(size());});
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
		bPlayStop->setIcon(QIcon::fromTheme("media-playback-start", QIcon(":/icons/media-playback-start")));
	} else {
		try {
			updateMidiFile();
		} catch (const Exception &e) {
			QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Can't create midi file:\n%1").arg(e.what()));
			QCoreApplication::instance()->quit();
		}
		fluidsynth.play(midiFile->fileName().toStdString());
		bPlayStop->setText(tr("Stop"));
		bPlayStop->setIcon(QIcon::fromTheme("media-playback-stop", QIcon(":/icons/media-plaback-stop")));
	}

	playing = !playing;

	bOwnVoiceOnly->setEnabled(!playing);
	bOwnVoiceForeground->setEnabled(!playing);
	bNoForeground->setEnabled(!playing);
	bOwnVoiceMute->setEnabled(!playing);
	sBar->setEnabled(!playing);
}

void MainWindow::updateMidiFile() {
	static QToolButton *lastChecked = nullptr;
	static int lastBar = -1;

	if (lastChecked) {
		if (lastChecked->isChecked() && lastBar == sBar->value()) return;
	}
	lastBar = sBar->value();

	if (bOwnVoiceOnly->isChecked()) {
		midiFile = midiParser.withOnlyVoice(ownTrack, sBar->value() - 1);
		lastChecked = bOwnVoiceOnly;
	} else if (bOwnVoiceForeground->isChecked()) {
		midiFile = midiParser.withForegroundVoice(ownTrack, sBar->value() - 1);
		lastChecked = bOwnVoiceForeground;
	} else if (bNoForeground->isChecked()) {
		midiFile = midiParser.withoutForegroundVoice(sBar->value() - 1);
		lastChecked = bNoForeground;
	} else if (bOwnVoiceMute->isChecked()) {
		midiFile = midiParser.withoutVoice(ownTrack, sBar->value() - 1);
		lastChecked = bOwnVoiceMute;
	}
}

void MainWindow::onOwnVoiceOnly() {
	setChecked(bOwnVoiceOnly);
}

void MainWindow::onOwnVoiceForeground() {
	setChecked(bOwnVoiceForeground);
}

void MainWindow::onNoForeground() {
	setChecked(bNoForeground);
}

void MainWindow::onOwnVoiceMute() {
	setChecked(bOwnVoiceMute);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	event->accept();
	QCoreApplication::instance()->quit();
}
