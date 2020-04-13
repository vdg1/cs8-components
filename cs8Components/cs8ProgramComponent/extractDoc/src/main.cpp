#include "cs8application.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

//

void printUsage() {
  fprintf(stderr, "Usage:\n"
                  "    \n");
}

int main(int argc, char **argv) {

  QCoreApplication app(argc, argv);

  QString sourceDir;
  QString destDir;
  bool sourceFlag = false;
  bool destFlag = false;

  QStringList arguments = app.arguments();
  // qDebug() << arguments;

  foreach (QString argument, arguments) {
    if (!sourceFlag && !destFlag) {
      if (argument == "-help") {
        printUsage();
        return 0;
      } else if (argument == "-version") {
        fprintf(stderr, "extractDoc version %s\n", QT_VERSION_STR);
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

  QString nameFilter = "*.pjx";
  QDirIterator dirIterator(sourceDir, QStringList() << nameFilter, QDir::Files,
                           QDirIterator::Subdirectories);
  QString str("folder");
  QStringList sourceFiles;

  while (dirIterator.hasNext()) {
    if (dirIterator.fileName().startsWith("z") ||
        dirIterator.fileName().startsWith("io")) {

      sourceFiles.append(dirIterator.filePath());
    }
    dirIterator.next();
  }

  cs8Application application;
  sourceFiles += "";

  qDebug() << ("applications: " + sourceFiles.join(", "));
  foreach (QString sourceApp, sourceFiles) {
    qDebug() << sourceDir + "/" + sourceApp;
    if (application.open(sourceApp)) {
      qDebug() << (sourceApp + " open ok");
      application.exportToCClass(destDir);
    }
    qDebug() << "Done";
  }
  return 0;
}
