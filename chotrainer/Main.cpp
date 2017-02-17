#include "MainWindow.h"
#include "SelectVoiceDialog.h"

#include <ChotrainerParser.h>
#include <Exception.h>

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QFileDialog>

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(core);
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

	size_t ownVoice;
	try {
		ownVoice = SelectVoiceDialog::getVoice(filePath);
	} catch (const Exception &e) {
		//TODO show error
		return EXIT_FAILURE;
	}

	ChotrainerParser cp(filePath);

	MainWindow mainWindow(cp.getMidiFile(), ownVoice);
	mainWindow.show();
	return app.exec();
}
