#include "SelectVoiceDialog.h"

#include <ChotrainParser.h>
#include <Exception.h>

#include <QPushButton>
#include <QVBoxLayout>

class VoiceButton : public QPushButton {
	private:
		const size_t track;

	public:
		VoiceButton(const QString &text, QWidget *parent, size_t track)
			: QPushButton(text, parent), track(track) {}
		size_t getTrack() const {return track;};
};

SelectVoiceDialog::SelectVoiceDialog(const std::string &chotrainFilePath)
:
	trackSet(false)
{
	QVBoxLayout *layout = new QVBoxLayout;

	ChotrainParser cp(chotrainFilePath);
	const std::vector<ChotrainParser::Track> namedTracks = cp.getNamedTracks();
	for (const auto &track : namedTracks) {
		VoiceButton *b = new VoiceButton(QString::fromStdString(track.name), this, track.number);
		QObject::connect(b, &QPushButton::clicked, this, &SelectVoiceDialog::onSetTrack);
		layout->addWidget(b);
	}

	QPushButton *bClose = new QPushButton(tr("Close"));
	bClose->setIcon(QIcon::fromTheme("application-exit"));
	QObject::connect(bClose, &QPushButton::clicked, this, &QDialog::close);
	layout->addWidget(bClose);

	setLayout(layout);
}

size_t SelectVoiceDialog::getTrack() const {
	if (!trackSet) throw(Exception("Track not set"));
	return track;
}

size_t SelectVoiceDialog::getVoice(const std::string &midiFilePath) {
	SelectVoiceDialog *d = new SelectVoiceDialog(midiFilePath);
	d->exec();
	return d->getTrack();
}

void SelectVoiceDialog::onSetTrack() {
	trackSet = true;
	track = dynamic_cast<VoiceButton*>(QObject::sender())->getTrack();
	close();
};
