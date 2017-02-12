#include "MainWindow.h"

#include <QApplication>
//#include <QFile>
//#include <QTextStream>
#include <QTranslator>
#include <QLocale>

#include <QFileDialog>
#include <QDir>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("chotrain creator");
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
	
	/*
	std::string filePath;
	QFileDialog fd(nullptr, QObject::tr("Open File"), QDir::homePath(), QString("%1 (*.midi)").arg(QObject::tr("Midi file")));
	fd.setFileMode(QFileDialog::ExistingFile);
	if (!fd.exec()) {
		return EXIT_FAILURE;
	}
	QStringList f = fd->selectedFiles();
	*/
	const std::string filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"), QDir::homePath(), QString("%1 (*.midi)").arg(QObject::tr("Midi file"))).toStdString();
	if (filePath == "") return EXIT_FAILURE;

	MainWindow mainWindow(filePath);
	//mainWindow.setStyleSheet(qss);
	mainWindow.show();
	return app.exec();
}
