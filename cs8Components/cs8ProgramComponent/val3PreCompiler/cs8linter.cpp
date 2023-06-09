#include "cs8linter.h"
#include "../common/versionInfo.h"

#include "qglobal.h"
#include "qnamespace.h"
#include "windows.h"
#include "wintoastlib.h"

#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QProcess>
#include <QTimer>
#include <QtConcurrent>
#include <cs8codevalidation.h>
#include <iostream>

using namespace WinToastLib;

void cs8Linter::executeVal3Check() {
  QProcess *proc = new QProcess(this);
  QObject::connect(proc, &QProcess::readyReadStandardOutput, [this, proc]() {
    QByteArray output = proc->readAllStandardOutput().trimmed();
    output.replace("\r", "\n");
    output.replace("\n\n", "\n");
    // m_val3CheckOutput.append(output + "\n");
    //  qDebug() << "standard output:" << output;
    // QTextStream(stdout)  << qUtf8Printable(output) ;
    emit sendOutput(output);
  });

  QObject::connect(proc, &QProcess::readyReadStandardError, [this, proc]() {
    // QByteArray output = proc->readAllStandardError().trimmed();
    // qDebug() << "error output:" << output;
    // emit sendOutput("error output:" + output);
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
      [this, proc](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "val3check completed:" << exitCode << exitStatus;
        qDebug() << "val3check output: " << m_val3CheckOutput;

        output(m_val3CheckOutput);
        m_val3checkDone = true;
        m_val3checkExitCode = exitCode;
        emit finished();
      });

  qDebug() << "Start original val3 checker:" << m_val3checkExecutable;
  proc->start(m_val3checkExecutable, m_arguments);
}

void cs8Linter::output(const QByteArray &out) {
  std::string l = out.toStdString();
  auto list = QString(out).split(QRegExp("[\\n\\r]"));
  for (const auto &i : list) {
    if (i.length() > 2) {
      // qDebug() << "out: " << i;
      std::cout << i.simplified().toStdString() << std::endl;
    }
  }
}

void cs8Linter::executeLinter() {
  QString l;
  for (auto &&e : m_applicationsToCheck) {
    l += e.split("\\").last() + "\n";
  }
  showToastNotification("Val3 syntax check started for:\n", l);
  QString productName, productVersion;
  getProductName(qApp->applicationFilePath(), productName, productVersion);
  QString headerMessage = "SAXE: Val3 Linter " + productVersion;
  qDebug() << headerMessage;
  emit sendOutput(qUtf8Printable(headerMessage));

  connect(&m_futureWatcher, &QFutureWatcher<void>::finished,
          []() { qDebug() << "Linter completed"; });

  m_future = QtConcurrent::run([=]() {
    // emit sendOutput("Start linter");
    cs8CodeValidation validator;
    if (validator.loadRuleFile(":/rules/compilerRules.xml")) {
      qDebug() << "Rules loaded, executing linter for " << m_arguments;
      foreach (const auto arg, m_applicationsToCheck) {
        qDebug() << "Linting now: " << arg;
        emit sendOutput(qUtf8Printable("Linting now:" + arg + "\n"));
        cs8Application app;
        app.setCellPath(m_cellPath);
        app.openFromPathName(arg);
        emit sendOutput(
            qUtf8Printable(validator.runValidation(&app, 1).join("\n") + "\n"));
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

  connect(this, &cs8Linter::allDoneAndExit, this, &cs8Linter::exitProgram,
          Qt::QueuedConnection);

  QObject::connect(this, &cs8Linter::finished, [=]() {
    qDebug() << __FUNCTION__ << "val3Check finished: " << m_val3checkDone
             << "code: " << m_val3checkExitCode << ": linter" << m_linterDone;

    if (m_val3checkDone && m_linterDone) {
      // std::cout << std::endl;
      // exit(m_val3checkExitCode);

      emit allDoneAndExit(m_val3checkExitCode);
    }
  });
}

void cs8Linter::startLinterAndChecker() {
  executeVal3Check();
  executeLinter();
}

void cs8Linter::exitProgram(int exitCode) {
  showToastNotification("Val3 syntax check completed","");
  qDebug() << "quit linter:" << m_val3checkExitCode;
  QTimer::singleShot(1000, [=]() { exit(m_val3checkExitCode); });
  // exit(m_val3checkExitCode);
}

class CustomHandler : public IWinToastHandler {
public:
  void toastActivated() const {
    std::wcout << L"The user clicked in this toast" << std::endl;
  }

  void toastActivated(int actionIndex) const {
    std::wcout << L"The user clicked on button #" << actionIndex
               << L" in this toast" << std::endl;
  }

  void toastFailed() const {
    std::wcout << L"Error showing current toast" << std::endl;
  }
  void toastDismissed(WinToastDismissalReason state) const {
    switch (state) {
    case UserCanceled:
      std::wcout << L"The user dismissed this toast" << std::endl;
      break;
    case ApplicationHidden:
      std::wcout << L"The application hid the toast using ToastNotifier.hide()"
                 << std::endl;
      break;
    case TimedOut:
      std::wcout << L"The toast has timed out" << std::endl;
      break;
    default:
      std::wcout << L"Toast not activated" << std::endl;
      break;
    }
  }
};

void cs8Linter::showToastNotification(const QString &msg, const QString &info) {
  WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
  QString text = msg;
  templ.setTextField(text.toStdWString(), WinToastTemplate::FirstLine);
  templ.setTextField(info.toStdWString(), WinToastTemplate::SecondLine);
  // templ.setExpiration(1000);
  templ.setDuration(WinToastLib::WinToastTemplate::Short);
  if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
    qDebug() << "Toast failed: ";
  }
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
  QSettings settings;
  QString activeProfile =
      settings.value("activeProfile", "Application programmer").toString();
  if (activeProfile == "Application programmer") {
    QMutableStringListIterator i(m_applicationsToCheck);
    while (i.hasNext()) {
      QString val = i.next();
      if (val.contains("\\SAXEAutomation\\") || val.contains("\\z") ||
          val.contains("/z")) {
        i.remove();
      }
    }
  }

  /*
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
*/

  initConnections();

  qDebug() << "Arguments:" << m_arguments;
  qDebug() << "Cell Path: " << m_cellPath;

  WinToast::instance()->setAppName(L"SAXE Val3 Linter");
  WinToast::instance()->setAppUserModelId(WinToast::configureAUMI(
      L"SAXE", L"Val3 Tools", L"val3Linter", L"20220304"));
  if (!WinToast::instance()->initialize()) {
    qDebug() << "Error, your system in not compatible!";
  }
  QTimer::singleShot(0, this, SLOT(startLinterAndChecker()));
}
