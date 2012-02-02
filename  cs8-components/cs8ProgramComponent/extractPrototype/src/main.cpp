#include <QApplication>
#include <QDir>
#include <QDebug>
#include "cs8application.h"
#include "mainwindowimpl.h"

//

void printUsage() {
	fprintf(stderr, "Usage:\n"
		"    \n");

}

int main(int argc, char ** argv) {

	//if (argc > 1) {
		QApplication app(argc, argv);

		MainWindowImpl win;
		win.show();
		app.connect(&app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );
		return app.exec();
	/*} else {
		QCoreApplication app(argc, argv);

		QStringList sourceFiles;
		QString sourceDir;
		QString destDir;
		bool sourceFlag = false;
		bool destFlag = false;

		QStringList arguments = app.arguments();
		//qDebug() << arguments;

		foreach (QString argument,arguments) {
				if (!sourceFlag && !destFlag) {
					if (argument == "-help") {
						printUsage();
						return 0;
					} else if (argument == "-version") {
						fprintf(stderr, "extractPrototype version %s\n",
								QT_VERSION_STR );
						return 0;
					} else if (argument == "-source") {
						sourceFlag = true;
					} else if (argument == "-dest") {
						destFlag = true;
					}
				} else {
					if (sourceFlag && !argument.startsWith("-")) {
						sourceDir = argument;
						sourceFlag = false;
					}
					if (destFlag && !argument.startsWith("-")) {
						destDir = argument;
						destFlag = false;
					}
				}
			}
		if (destDir.isEmpty() || sourceDir.isEmpty()) {
			printUsage();
			return 0;
		}

		qDebug() << "source: " << sourceDir;
		qDebug() << "dest:   " << destDir;

		QDir dir(sourceDir);
		cs8Application application;
		sourceFiles = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		qDebug() << "applications: " << sourceFiles;
		foreach(QString sourceApp,sourceFiles) {

				if (application.openFromPathName(sourceDir + "/" + sourceApp)) {
					qDebug() << sourceApp << " open ok";
					application.exportInterfacePrototype(destDir);
				} else
					qDebug() << sourceApp << "failed";
			}
		return 0;
	}
	*/
}
