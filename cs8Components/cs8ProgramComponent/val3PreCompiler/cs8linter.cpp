#include "cs8linter.h"
#include "../common/versionInfo.h"

#include "windows.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QTimer>
#include <QtConcurrent>
#include <cs8codevalidation.h>
#include <iostream>

void cs8Linter::executeVal3Check() {
  QProcess *proc = new QProcess(this);
  QObject::connect(proc, &QProcess::readyReadStandardOutput, [this, proc]() {
    QByteArray output = proc->readAllStandardOutput();
    // qDebug() << "standard output:" << output;
    emit sendOutput(output);
  });

  QObject::connect(proc, &QProcess::readyReadStandardError, [this, proc]() {
    QByteArray output = proc->readAllStandardError().simplified();
    // qDebug() << "error output:" << output;
    emit sendOutput(output);
  });

  QObject::connect(proc, &QProcess::errorOccurred,
                   [=](QProcess::ProcessError error) {
                     qDebug() << __FUNCTION__ << error;
                     m_val3checkDone = true;
                     m_val3checkExitCode = 1;
                     emit finished();
                   });

  QObject::connect(
      proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [this](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "val3check completed:" << exitCode << exitStatus;
        m_val3checkDone = true;
        m_val3checkExitCode = exitCode;
        emit finished();
      });

  qDebug() << "Start original val3 checker:" << m_val3checkExecutable;
  proc->start(m_val3checkExecutable, m_arguments);
}

void cs8Linter::output(const QByteArray &out) {
  qDebug() << "out: " << out;
  std::cout << qUtf8Printable(out);
}

void cs8Linter::executeLinter() {
  QString productName, productVersion;
  getProductName(qApp->applicationFilePath(), productName, productVersion);
  QString headerMessage = "SAXE: Val3 Linter " + productVersion;
  qDebug() << headerMessage;
  std::cout << qPrintable(headerMessage + "\n");

  connect(&m_futureWatcher, &QFutureWatcher<void>::finished,
          []() { qDebug() << "Linter completed"; });
  m_future = QtConcurrent::run([=]() {
    qDebug() << "Start linter";
    cs8CodeValidation validator;
    if (validator.loadRuleFile(":/rules/compilerRules.xml")) {
      qDebug() << "Rules loaded, executing linter for " << m_arguments;
      foreach (const auto arg, m_applicationsToCheck) {
        qDebug() << "Linting now: " << arg;
        cs8Application app;
        app.setCellPath(m_cellPath);
        app.openFromPathName(arg);
        emit sendOutput(
            QString(validator.runValidation(&app, 1).join("\r\n") + "\r\n")
                .toUtf8());
      }
    }
    qDebug() << "Ended linter";
    m_linterDone = true;
    emit finished();
  });
}

void cs8Linter::initConnections() {
  connect(this, &cs8Linter::sendOutput, this, &cs8Linter::output,
          Qt::QueuedConnection);

  QObject::connect(this, &cs8Linter::finished, [=]() {
    qDebug() << __FUNCTION__ << "val3check: " << m_val3checkDone
             << "code: " << m_val3checkExitCode << ": linter" << m_linterDone;
    if (m_val3checkDone && m_linterDone) {
      qDebug() << "quit linter:" << m_val3checkExitCode;
      exit(m_val3checkExitCode);
      // QCoreApplication::quit();
    }
  });
}

void cs8Linter::startLinterAndChecker() {
  executeVal3Check();
  executeLinter();
}

cs8Linter::cs8Linter(const QStringList &arguments,
                     const QString &val3checkExecutable, bool overrideIOCheck,
                     QObject *parent)
    : QObject(parent), m_val3checkDone(false), m_linterDone(false),
      m_val3checkExitCode(0) {
  m_arguments = arguments;
  m_val3checkExecutable = val3checkExecutable;

  QMutableListIterator<QString> i(m_arguments);
  while (i.hasNext()) {
    QString arg = i.next();

    qDebug() << "A" << arg;
    if (!arg.startsWith("-")) {
      m_applicationsToCheck << arg;
    }
    if (arg.startsWith("-R")) {
      m_cellPath = arg;
      m_cellPath = m_cellPath.remove("\"").remove("-R").trimmed();
    }
    if (arg == "-I+" && overrideIOCheck)
      i.setValue("-I-");
  }
  // filter application list for linter
  // only lint applications that are in usrapp/ if multiple applications are to
  // be checked
  if (m_applicationsToCheck.count() > 1) {
    QStringList n;
    foreach (const auto p, m_applicationsToCheck) {
      QStringList e = p.split("\\");
      if (e.count() > 2 && e.at(e.count() - 2) == "usrapp")
        n << p;
    }
    m_applicationsToCheck = n;
  }

  initConnections();

  qDebug() << "Arguments:" << m_arguments;
  qDebug() << "Cell Path: " << m_cellPath;

  QTimer::singleShot(0, this, SLOT(startLinterAndChecker()));
}
