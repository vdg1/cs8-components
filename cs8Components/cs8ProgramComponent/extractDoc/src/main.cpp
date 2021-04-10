#include "cs8application.h"
#include <QApplication>
#include <QCommandLineParser>
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
  QCommandLineParser parser;
  parser.setApplicationDescription("Val3 doc extractor");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption sourceDirOption(
      "source", QCoreApplication::translate("main", "Source directory."),
      QCoreApplication::translate("main", "directory"));
  parser.addOption(sourceDirOption);

  QCommandLineOption destDirOption(
      "dest", QCoreApplication::translate("main", "Destination directory."),
      QCoreApplication::translate("main", "directory"));
  parser.addOption(destDirOption);
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser.process(app);

  QString destDir = parser.value(destDirOption);

  QStringList sourceDirs = parser.values(sourceDirOption);

  qDebug() << "source: " << sourceDirs;
  qDebug() << "dest:   " << destDir;

  QString nameFilter = "*.pjx";

  QStringList sourceFiles;

  // read source dirs
  for (const auto &source : qAsConst(sourceDirs)) {
    if (!source.isEmpty()) {
      QFileInfo info(source);
      if (info.isDir()) {
        QDirIterator dirIterator(source, QStringList() << nameFilter,
                                 QDir::Files, QDirIterator::Subdirectories);
        while (dirIterator.hasNext()) {
          // QString filePath = dirIterator.next();
          QFileInfo info = dirIterator.fileInfo();
          qDebug() << "check: " << info;
          if (info.fileName().startsWith("z") ||
              info.fileName().startsWith("io")) {
            sourceFiles.append(info.absoluteFilePath());
          }
          // qDebug() << "next: " << dirIterator.next();
        }
      } else if (info.isFile() && info.fileName().endsWith(".pjx")) {
        sourceFiles.append(info.absoluteFilePath());
      }
    }
  }

  cs8Application application;
  sourceFiles += "";

  qDebug() << ("applications: " + sourceFiles.join(", "));
  foreach (QString sourceApp, sourceFiles) {
    // qDebug() << sourceDir + "/" + sourceApp;
    if (application.open(sourceApp)) {
      // qDebug() << (sourceApp + " open ok");
      application.exportToCClass(destDir);
    }
    qDebug() << "Done";
  }
  return 0;
}
