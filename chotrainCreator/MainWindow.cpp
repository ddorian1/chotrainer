#include "MainWindow.h"

#include <ChotrainParser.h>

#include <QCoreApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QFrame>

MainWindow::MainWindow(const std::string &midiFile)
:
	midiFilePath(midiFile)
{
	QWidget *centralWidget = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);

	MidiParser mp(midiFile);
	const size_t tracks = mp.getNumberOfMusicTracks();

	names.resize(tracks, nullptr);
	for (auto &n : names) n = new QLineEdit(this);
	accompaniments.resize(tracks, nullptr);
	for (auto &a : accompaniments) {
		a = new QCheckBox(this);
		QObject::connect(a, &QCheckBox::clicked, this, &MainWindow::onAccompaniment);
	}

	QGridLayout *grid = new QGridLayout();
	for (size_t i = 0; i < tracks; ++i) {
		grid->addWidget(names[i], i, 1);
		grid->addWidget(accompaniments[i], i, 2);
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

	QObject::connect(bCancel, &QPushButton::clicked, QCoreApplication::instance(), &QCoreApplication::quit);
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
