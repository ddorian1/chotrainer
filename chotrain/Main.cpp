#include "MainWindow.h"
#include "SelectVoiceDialog.h"

#include <ChotrainParser.h>
#include <Exception.h>

#include <QApplication>
//#include <QFile>
//#include <QTextStream>
#include <QTranslator>
#include <QLocale>
#include <QFileDialog>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("chotrain");
	app.setWindowIcon(QIcon(":/icon"));
	QTranslator translator;

	Q_INIT_RESOURCE(intl);
	QString filename = QString("chotrain_%1").arg(QLocale::system().name());
	QString dir(":");
	translator.load(filename, dir);
	app.installTranslator(&translator);

	//QString qss;
	//{
	//	QFile qssFile(":/main.qss");
	//	if (qssFile.open(QFile::ReadOnly)) {
	//		QTextStream qssStream(&qssFile);
	//		qss = qssStream.readAll();
	//	}
	//}
	const std::string filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"), QDir::homePath(), QString("%1 (*.chotrain)").arg(QObject::tr("Chotrain file"))).toStdString();
	if (filePath == "") return EXIT_FAILURE;

	size_t ownVoice;
	try {
		ownVoice = SelectVoiceDialog::getVoice(filePath);
	} catch (const Exception &e) {
		return EXIT_FAILURE;
	}

	ChotrainParser cp(filePath);

	MainWindow mainWindow(cp.getMidiFile(), ownVoice);
	//mainWindow.setStyleSheet(qss);
	mainWindow.show();
	return app.exec();
}
