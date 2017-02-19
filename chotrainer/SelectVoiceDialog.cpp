#include "SelectVoiceDialog.h"

#include <Exception.h>

#include <QFrame>
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

SelectVoiceDialog::SelectVoiceDialog(const std::vector<ChotrainerParser::Track> &namedTracks)
:
	trackSet(false)
{
	QVBoxLayout *layout = new QVBoxLayout;
	for (const auto &track : namedTracks) {
		VoiceButton *b = new VoiceButton(QString::fromStdString(track.name), this, track.number);
		QObject::connect(b, &QPushButton::clicked, this, &SelectVoiceDialog::onSetTrack);
		layout->addWidget(b);
	}
	setLayout(layout);
}

size_t SelectVoiceDialog::getTrack() const {
	if (!trackSet) throw(Exception("No track selected"));
	return track;
}

size_t SelectVoiceDialog::getVoice(const std::vector<ChotrainerParser::Track> &namedTracks) {
	SelectVoiceDialog *d = new SelectVoiceDialog(namedTracks);
	d->exec();
	return d->getTrack();
}

void SelectVoiceDialog::onSetTrack() {
	trackSet = true;
	track = dynamic_cast<VoiceButton*>(QObject::sender())->getTrack();
	close();
};
