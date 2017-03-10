#include "MainWindow.h"
#include "SelectVoiceDialog.h"

#include <ChotrainerParser.h>
#include <Exception.h>

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QFileDialog>
#include <QMessageBox>

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(chotrainer);
	Q_INIT_RESOURCE(intl);

	QApplication app(argc, argv);
	app.setApplicationName("Chotrainer");
	app.setWindowIcon(QIcon(":/icon"));
	QTranslator translator;

	QString filename = QString("chotrainer_%1").arg(QLocale::system().name());
	QString dir(":");
	translator.load(filename, dir);
	app.installTranslator(&translator);

	QStringList arguments = app.arguments();
	std::string filePath;
	if (arguments.length() < 2) {
		filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"), QDir::homePath(), QString("%1 (*.chotrainer)").arg(QObject::tr("Chotrainer file"))).toStdString();
		if (filePath == "") return EXIT_FAILURE;
	} else {
		filePath = arguments.at(1).toStdString();
	}

	try {
		ChotrainerParser cp(filePath);
		const ChotrainerParser::Track ownTrack = SelectVoiceDialog::getVoice(cp.getNamedTracks());
		MainWindow mainWindow(cp, ownTrack);
		mainWindow.show();
		return app.exec();
	} catch (const Exception &e) {
		QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Can't process Chotrainer file:\n%1").arg(e.what()));
		return EXIT_FAILURE;
	}

	return EXIT_FAILURE;
}
