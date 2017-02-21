#include "MainWindow.h"

#include <Exception.h>

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(core);
	Q_INIT_RESOURCE(intl);

	QApplication app(argc, argv);
	app.setApplicationName("Chotrainer Creator");
	app.setWindowIcon(QIcon(":/icon"));
	QTranslator translator;

	QString filename = QString("chotrainer_%1").arg(QLocale::system().name());
	QString dir(":");
	translator.load(filename, dir);
	app.installTranslator(&translator);

	QStringList arguments = app.arguments();
	std::string filePath;
	if (arguments.length() < 2) {
		filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"), QDir::homePath(), QString("%1 (*.midi *.mid)").arg(QObject::tr("Midi file"))).toStdString();
		if (filePath == "") return EXIT_FAILURE;
	} else {
		filePath = arguments.at(1).toStdString();
	}

	try {
		MainWindow mainWindow(filePath);
		mainWindow.show();
		return app.exec();
	} catch (const Exception &e) {
		QMessageBox::critical(nullptr, QObject::tr("Critical Error"), QObject::tr("Can't process midi file:\n%1").arg(e.what()));
	}

	return EXIT_FAILURE;
}
