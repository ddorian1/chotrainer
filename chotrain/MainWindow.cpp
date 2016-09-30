#include "MainWindow.h"

#include <QVBoxLayout>
#include <QFrame>

MainWindow::MainWindow(size_t ownTrack)
:
	ownTrack(ownTrack),
	playing(false),
	bOwnVoiceForeground(new QToolButton(this)),
	bNoForeground(new QToolButton(this)),
	bOwnVoiceMute(new QToolButton(this)),
	bPlayStop(new QPushButton(this))
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);
	
	QFrame *sep = new QFrame(this);
	sep->setFrameShape(QFrame::HLine);

	layout->addWidget(bOwnVoiceForeground);
	layout->addWidget(bNoForeground);
	layout->addWidget(bOwnVoiceMute);
	layout->addWidget(sep);
	layout->addWidget(bPlayStop);
	layout->setAlignment(bPlayStop, Qt::AlignHCenter);

	centralWidget->setLayout(layout);
	setCentralWidget(centralWidget);

	bOwnVoiceForeground->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	bNoForeground->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	bOwnVoiceMute->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	bOwnVoiceForeground->setText(tr("Emphatize my voice"));
	bNoForeground->setText(tr("Don't emphatize my voice"));
	bOwnVoiceMute->setText(tr("Mute my voice"));
	bPlayStop->setText(tr("Play"));
	bPlayStop->setIcon(QIcon::fromTheme("media-playback-start"));

	QObject::connect(bOwnVoiceForeground, SIGNAL(clicked()), this, SLOT(onOwnVoiceForeground));
	QObject::connect(bNoForeground, SIGNAL(clicked()), this, SLOT(onNoForeground));
	QObject::connect(bOwnVoiceMute, SIGNAL(clicked()), this, SLOT(onOwnVoiceMute));
	QObject::connect(bPlayStop, SIGNAL(clicked()), this, SLOT(onPlayStop));

	bOwnVoiceForeground->setCheckable(true);
	bNoForeground->setCheckable(true);
	bOwnVoiceMute->setCheckable(true);
	bOwnVoiceForeground->setChecked(true);
}

void MainWindow::onPlayStop() {
}

void MainWindow::onOwnVoiceForeground() {
}

void MainWindow::onNoForeground() {
}

void MainWindow::onOwnVoiceMute() {
}
