#include "dialogbuilddocumentation.h"
#include "ui_dialogbuilddocumentation.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QUrl>

DialogBuildDocumentation::DialogBuildDocumentation(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogBuildDocumentation) {
  ui->setupUi(this);
  doxygenProcess = new QProcess(this);
  connect(doxygenProcess, SIGNAL(error(QProcess::ProcessError)), this,
          SLOT(slotProcessError(QProcess::ProcessError)));
  connect(doxygenProcess, SIGNAL(finished(int)), this, SLOT(slotFinished(int)));
  connect(doxygenProcess, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(doxygenProcess, SIGNAL(readyReadStandardError()), this,
          SLOT(slotReadyRead()));

  setModal(true);
  configFile = new QTemporaryFile(this);
}

DialogBuildDocumentation::~DialogBuildDocumentation() { delete ui; }

bool DialogBuildDocumentation::build(cs8Application *application) {
  QSettings settings;
  // check requirements
  if (!QFile::exists(settings.value("doxygenBin").toString())) {
    QMessageBox::critical(
        this, tr("Critical Error"),
        tr("The doxygen executable could not be foud. Please make sure that "
           "doxygen is installed and check the settings!"));
    close();
    return false;
  }
  if (!QFile::exists(settings.value("hhcBin").toString())) {
    QMessageBox::critical(
        this, tr("Critical Error"),
        tr("The help file compiler hhc.exe could not be foud. Please make sure "
           "Microsoft Help Workshop is installed and check the settings!"));
    close();
    return false;
  }
  if (!QFile::exists(settings.value("graphviz").toString())) {
    QMessageBox::critical(
        this, tr("Critical Error"),
        tr("The Graphviz dot.exe could not be foud. Please make sure Graphviz "
           "is installed and check the settings!"));
    close();
    return false;
  }
  qApp->setOverrideCursor(Qt::WaitCursor);

#if QT_VERSION >= 0x050000
  QString tempPath =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Val3" +
      QDateTime::currentDateTime().toString("hh-mm-ss");
#else
  QString tempPath =
      QDesktopServices::storageLocation(QDesktopServices::TempLocation) +
      "/Val3" + QDateTime::currentDateTime().toString("hh-mm-ss");

#endif

  QDir dir;
  if (dir.mkpath(tempPath))

  {
    ui->plainTextEdit->appendPlainText("Exporting to C file");
    application->exportToCClass(tempPath);
    if (application->includeLibraryDocuments()) {
      foreach (cs8LibraryAlias *alias, application->libraryModel()->list()) {
        cs8Application app;
        app.setCellPath(application->cellPath() + "/usr/usrapp");
        app.open(alias->path());
        app.exportToCClass(tempPath);
      }
    }
    // generate doxygen file

    QDir templateDir(qApp->applicationDirPath() + "/doxygenTemplateData");
    QFile sourceFile(templateDir.absolutePath() + "/DoxyfileVal3");

    if (sourceFile.open(QFile::ReadOnly) && configFile->open()) {
      dir.setPath(application->getProjectPath());
      m_outputPath =
          QDir::toNativeSeparators(dir.absolutePath() + "/documentation");
      if (!dir.mkpath(m_outputPath)) {
        ui->plainTextEdit->appendPlainText(
            tr("Failed to create output path: %1").arg(m_outputPath));
        ui->buttonBox->setEnabled(true);
        qApp->restoreOverrideCursor();
      }
      QString outputFileName =
          m_outputPath + "/" + application->name() +
          (application->version().isEmpty() ? ""
                                            : "_" + application->version()) +
          ".chm";
      QFile file(outputFileName);

      if (file.remove() || !file.exists()) {
        QString configText = sourceFile.readAll();
        configText.replace("#Temp#", QDir::toNativeSeparators(tempPath));
        configText.replace("#OutputDir#", m_outputPath);
        configText.replace("#projectName#", application->name());
        configText.replace("#chmFile#", outputFileName);
        configText.replace("#templateDir#", templateDir.absolutePath() + "/");
        configText.replace("#Version#", application->version());
        configText.replace(
            "#graphvizPath#",
            QDir::fromNativeSeparators(settings.value("graphviz").toString()));
        configText.replace(
            "#hhcLocation#",
            QDir::fromNativeSeparators(settings.value("hhcBin").toString()));
        configFile->write(configText.toLatin1());
        // ui->plainTextEdit->appendPlainText(configText);
        configFile->close();
        ui->plainTextEdit->appendPlainText("Config file: " +
                                           configFile->fileName());
        doxygenProcess->start(
            settings.value("doxygenBin").toString(),
            QStringList() << QDir::toNativeSeparators(configFile->fileName()));
      } else {
        ui->plainTextEdit->appendPlainText(
            tr("Couldn't delete destination file: %1").arg(outputFileName));
        ui->buttonBox->setEnabled(true);
        qApp->restoreOverrideCursor();
      }

    } else {
      qDebug() << "Failed to open:" << sourceFile.fileName() << " or "
               << configFile->fileName();
    }

  } else {
    ui->plainTextEdit->appendPlainText(
        "Failed to generate temporary output path " + tempPath);
    ui->buttonBox->setEnabled(true);
    qApp->restoreOverrideCursor();
  }
  return true;
}

void DialogBuildDocumentation::slotProcessError(QProcess::ProcessError) {
  ui->plainTextEdit->appendPlainText("Error occured in callling doxygen:" +
                                     doxygenProcess->errorString());
  ui->buttonBox->setEnabled(true);
  qApp->restoreOverrideCursor();
}

void DialogBuildDocumentation::slotFinished(int) {
  if (doxygenProcess->error() != QProcess::UnknownError)
    ui->plainTextEdit->appendPlainText("doxygen finished: " +
                                       doxygenProcess->errorString());
  ui->buttonBox->setEnabled(true);
  qApp->restoreOverrideCursor();
  if (QMessageBox::question(
          this, tr("Question"),
          ("Would you like to open the path containing the documentation?"),
          QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    QString f =
        QString("file:///%1").arg(QDir::fromNativeSeparators(m_outputPath));
    if (!QDesktopServices::openUrl(QUrl(f)))
      qDebug() << "failed to open url " << f;
  }
}

void DialogBuildDocumentation::slotReadyRead() {
  while (doxygenProcess->canReadLine()) {
    ui->plainTextEdit->appendPlainText(doxygenProcess->readLine());
    qApp->processEvents();
  }
  ui->plainTextEdit->appendPlainText(doxygenProcess->readAllStandardError());
  qApp->processEvents();
}
