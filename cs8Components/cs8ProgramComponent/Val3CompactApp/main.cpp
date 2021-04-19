#include "cs8application.h"
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

struct Options {
  QStringList sourcePaths;
  bool recursive;
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
      {{{"recursive", "r"}, "Crawl through all subfolders"},
       {{"compactMode", "c"}, "Store API library in compact mode"}});

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

  options->sourcePaths = parser.positionalArguments();
  options->recursive = parser.isSet("recursive");
  options->compactMode = parser.isSet("compactMode");

  return CommandLineOk;
}

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

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

  QStringList apps;
  for (const auto &path : options.sourcePaths) {
    QFileInfo i(path);
    QString s = path + QDir::separator() + i.baseName() + ".pjx";
    if (QFile::exists(s))
      apps << s;
    //
    if (options.recursive) {
      QDirIterator it(path, QStringList() << "*.pjx",
                      QDir::Files | QDir::NoDotAndDotDot,
                      QDirIterator::Subdirectories);
      while (it.hasNext())
        apps << it.next();
    }
  }

  for (const auto &appPath : apps) {
    cs8Application app;
    app.open(appPath);
    app.save(options.compactMode);
  }

  return 0;
}
