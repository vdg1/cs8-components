#include <QApplication>
#include <QDir>
#include <QDebug>
#include "cs8application.h"


//

void printUsage() {
    fprintf(stderr, "Usage:\n"
            "    \n");

}

int main(int argc, char ** argv) {

    QCoreApplication app(argc, argv);


    QString sourceFile;

    QStringList arguments = app.arguments();
    arguments.removeAt(0);
    //qDebug() << arguments;

    foreach (QString argument,arguments) {
        qDebug() << "argument: " << argument;
        if (argument == "-help") {
            printUsage();
            return 0;
        } else if (argument == "-version") {
            fprintf(stderr, "extractDoc version %s\n",
                    QT_VERSION_STR );
            return 0;
        } else
        {
            sourceFile=argument;
        }

    }
    if (sourceFile.isEmpty()) {
        printUsage();
        return 0;
    }

    qDebug() << "source: " << sourceFile;
    QFileInfo fileInfo(sourceFile);
    if (fileInfo.completeSuffix()=="pjx")
    {
        cs8Application application;
        if (application.open(sourceFile)) {
            qDebug() << (sourceFile + " open ok");

            QTextStream out(stdout);
            //out << application.exportToCClass();

        }
        qDebug() << "Done";

    }
    return 0;
}

