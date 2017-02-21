#include "SelectVoiceDialog.h"

#include <Exception.h>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class VoiceButton : public QPushButton {
	private:
		const ChotrainerParser::Track track;

	public:
		VoiceButton(const QString &text, QWidget *parent, ChotrainerParser::Track track)
			: QPushButton(text, parent), track(track) {}
		ChotrainerParser::Track getTrack() const {return track;};
};

SelectVoiceDialog::SelectVoiceDialog(const std::vector<ChotrainerParser::Track> &namedTracks)
:
	trackSet(false)
{
	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(new QLabel(tr("Please select your voice"), this));

	for (const auto &track : namedTracks) {
		VoiceButton *b = new VoiceButton(QString::fromStdString(track.name), this, track);
		QObject::connect(b, &QPushButton::clicked, this, &SelectVoiceDialog::onSetTrack);
		layout->addWidget(b);
	}
	setLayout(layout);
}

ChotrainerParser::Track SelectVoiceDialog::getTrack() const {
	if (!trackSet) throw(Exception("No track selected"));
	return track;
}

ChotrainerParser::Track SelectVoiceDialog::getVoice(const std::vector<ChotrainerParser::Track> &namedTracks) {
	SelectVoiceDialog *d = new SelectVoiceDialog(namedTracks);
	d->exec();
	return d->getTrack();
}

void SelectVoiceDialog::onSetTrack() {
	trackSet = true;
	track = dynamic_cast<VoiceButton*>(QObject::sender())->getTrack();
	close();
};
