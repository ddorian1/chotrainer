#include "MainWindow.h"

#include <QApplication>
//#include <QFile>
//#include <QTextStream>
#include <QTranslator>
//#include <QLocale>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setApplicationName("chotrain");
	QTranslator translator;

	//QString filename = QString("chordival_%1").arg(QLocale::system().name());
	//QString dir(":");
	//translator.load(filename, dir);
	app.installTranslator(&translator);

	//QString qss;
	//{
	//	QFile qssFile(":/main.qss");
	//	if (qssFile.open(QFile::ReadOnly)) {
	//		QTextStream qssStream(&qssFile);
	//		qss = qssStream.readAll();
	//	}
	//}

	MainWindow mainWindow("/home/johannes/Noten/Kugelmann/Ein_feste_Burg.midi", 2);
	//mainWindow.setStyleSheet(qss);
	mainWindow.show();
	return app.exec();
}
