#include <QApplication>

#include "mainwindow.h"

#include <QDir>

#include <QCommandLineParser>
#include <cs8application.h>

struct Options {
  QStringList sourcePaths;
  QString prefixPattern;
  QString rootPath;
  QString outputPath;
  bool copyToProjectPath;
  bool compactMode;
};

enum CommandLineParseResult {
  CommandLineOk,
  CommandLineError,
  CommandLineVersionRequested,
  CommandLineHelpRequested
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser,
                                        Options *options,
                                        QString *errorMessage) {
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser.addOptions(
      {{{"rootPath", "r"}, "Root path", "RootPath"},
       {{"copy", "c"}, "Copy API library to root folder of project"},
       {{"compactMode", "m"}, "Store API library in compact mode"},
       {{"outputPath", "o"},
        "Output path of API library and IO library",
        "OutputPath"}});

  parser.addPositionalArgument("source", "Source path to scan", "SourcePath");
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();

  if (!parser.parse(QCoreApplication::arguments())) {
    *errorMessage = parser.errorText();
    return CommandLineError;
  }

  if (parser.isSet(versionOption))
    return CommandLineVersionRequested;

  if (parser.isSet(helpOption))
    return CommandLineHelpRequested;

  if (parser.positionalArguments().count() == 0) {
    *errorMessage = "No source paths given";
    return CommandLineError;
  }

  options->rootPath =
      parser.isSet("rootPath") ? parser.value("rootPath") : QDir::currentPath();
  options->sourcePaths = parser.positionalArguments();
  options->copyToProjectPath = parser.isSet("copy");
  options->outputPath = parser.value("outputPath");
  options->compactMode = parser.isSet("compactMode");

  return CommandLineOk;
}

int main(int argc, char *argv[])

{
  QApplication a(argc, argv);
  if (qApp->arguments().count() > 1) {
    QCommandLineParser parser;
    parser.setApplicationDescription("Help text");
    Options options;
    QString errorMessage;
    switch (parseCommandLine(parser, &options, &errorMessage)) {
    case CommandLineOk:
      break;
    case CommandLineError:
      fputs(qPrintable(errorMessage), stderr);
      fputs("\n\n", stderr);
      fputs(qPrintable(parser.helpText()), stderr);
      return 1;
    case CommandLineVersionRequested:
      printf("%s %s\n", qPrintable(QCoreApplication::applicationName()),
             qPrintable(QCoreApplication::applicationVersion()));
      return 0;
    case CommandLineHelpRequested:
      parser.showHelp();
      Q_UNREACHABLE();
    }

    MainWindow w;
    QStringList dirs;

    QDir dir;
    for (const auto &dirTxt : qAsConst(options.sourcePaths)) {
      dir.setPath(options.rootPath + "/" + dirTxt);
      qDebug() << "Scanning dir: " << dir.absolutePath();
        for (const QString &pth : dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
        dirs << dir.absolutePath() + "/" + pth;
    }

    QList<cs8Application *> cs8SourceApps;
    for (const auto &pth : qAsConst(dirs)) {
      cs8Application *cs8SourceApp = new cs8Application();
      if (cs8SourceApp->openFromPathName(pth)) {
        cs8SourceApps.append(cs8SourceApp);
      } else {
        delete cs8SourceApp;
      }
    }
    w.createAPIs(cs8SourceApps, options.copyToProjectPath, options.compactMode,
                 options.rootPath + "/" + options.outputPath);
    return 0;
  } else {
    a.setApplicationName("Val3 Create API");
    MainWindow w;
    w.show();
    return a.exec();
  }
}
