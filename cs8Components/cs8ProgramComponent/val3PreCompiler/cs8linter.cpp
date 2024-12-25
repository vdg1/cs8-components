#include "cs8linter.h"
#include "../common/versionInfo.h"

#include "cs8validationresult.h"
#include "qglobal.h"
#include "qnamespace.h"
#include "qregularexpression.h"
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
    // qDebug() << "standard output:" << output;
    // QTextStream(stdout)  << qUtf8Printable(output) ;
    emit sendOutput(output);
  });

  QObject::connect(proc, &QProcess::readyReadStandardError, []() {
    // QByteArray output = proc->readAllStandardError().trimmed();
    // qDebug() << "error output:" << output;
    // emit sendOutput("error output:" + output);
  });

  QObject::connect(proc, &QProcess::errorOccurred,
                   [=](QProcess::ProcessError error) {
                     qDebug() << __FUNCTION__ << error;
                     m_val3checkDone = true;
                     m_val3checkExitCode = 1;
                     emit finished(0, 0, 0);
                   });

  QObject::connect(
      proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
      [this](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "val3check completed:" << exitCode << exitStatus;
        qDebug() << "val3check output: " << m_val3CheckOutput;

        output(m_val3CheckOutput);
        m_val3checkDone = true;
        m_val3checkExitCode = exitCode;
        emit finished(0, 0, 0);
      });

  qDebug() << "Start original val3 checker:" << m_val3checkExecutable;
  proc->start(m_val3checkExecutable, m_arguments);
}

void cs8Linter::output(const QByteArray &out) {
  std::string l = out.toStdString();
  QRegularExpression re(R"RX(--\n(\d+) error\(s\),(\d+) warning\(s\))RX");
  QRegularExpressionMatch match = re.match(l.c_str());
  if (match.hasMatch()) {
    m_totalErrors += match.captured(1).toInt();
    m_totalWarnings += match.captured(2).toInt();
  }
  auto list = QString(out).split(QRegExp("[\\n\\r]"));
  for (const auto &i : qAsConst(list)) {
    if (i.length() > 2) {
      qDebug() << "out: " << i;
      std::cout << i.simplified().toStdString() << std::endl;
    }
  }
}

void cs8Linter::executeLinter() {
  QString l;
  for (auto &&e : m_applicationsToCheck) {
    l += e.split("\\").last() + "\n";
  }
  showToastNotification("Val3 syntax check started for:\n", l, false);
  QString productName, productVersion;
  getProductName(qApp->applicationFilePath(), productName, productVersion);
  QString headerMessage = "SAXE: Val3 Linter " + productVersion;
  qDebug() << headerMessage;
  emit sendOutput(qUtf8Printable(headerMessage));

  connect(&m_futureWatcher, &QFutureWatcher<void>::finished,
          []() { qDebug() << "Linter completed"; });

  //
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
        cs8ValidationResult result = validator.runValidation(&app, 1);
        emit sendOutput(qUtf8Printable(result.messages.join("\n") + "\n"));
        m_totalInfos += result.totalInfos;
        m_totalErrors += result.totalErrors;
        m_totalWarnings += result.totalWarnings;
        executeGITIntegration(app);
      }
    }
    qDebug() << "Ended linter";
    m_linterDone = true;
    emit finished(m_totalInfos, m_totalWarnings, m_totalErrors);
  });
}

void cs8Linter::executeGITIntegration(const cs8Application &app)
{
    // get all project files
    auto projectFiles = app.projectFileList();
    // check if git is enabled
    QSettings settings;
    bool gitEnabled = settings.value("gitEnabled", false).toBool();
    if (gitEnabled) {
        QString gitPath = settings.value("gitPath", "").toString();
        if (gitPath.isEmpty()) {
            emit sendOutput(qUtf8Printable("Git path not set in settings\n"));
            return;
        }
        QFileInfo gitFile(gitPath);
        if (!gitFile.exists()) {
            emit sendOutput(qUtf8Printable("Git executable not found\n"));
            return;
        }
        if (gitFile.fileName() != "git.exe") {
            emit sendOutput(qUtf8Printable("Git executable not found\n"));
            return;
        }
        // retrive list of files that are not in git
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);
        process.setProgram(gitPath);
        process.setArguments(QStringList() << "ls-files");
        process.setWorkingDirectory(app.projectPath());
        process.start();
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        // check if output starts with "fatal: "
        if (!output.startsWith("fatal: ")) {
            QStringList filesInGit = output.split("\n");
            // prepend path to filesInGit
            for (auto &f : filesInGit) {
                f = app.projectPath() + f;
            }
            for (const auto &f : qAsConst(projectFiles)) {
                if (!filesInGit.contains(f.absoluteFilePath())) {
                    // <level>%level%<CLASS>PRG<P1>%progName%<P2>CODE<line>%lineNumber%<msg>Program %progName% is not called<file>%fileName%
                    QString filePath = f.absoluteFilePath().replace(app.cellPath() + "usr/usrapp/",
                                                                    "Disk://");
                    QString msg = QString("<level>ERROR<CLASS>PRG<P1>%1<P2>CODE<line>0<msg>File "
                                          "not in git<file>%2")
                                      .arg(f.baseName())
                                      .arg(filePath);
                    m_totalErrors++;
                    emit sendOutput(qUtf8Printable(msg));
                }
            }
        }
    }
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
      emit allDoneAndExit(m_val3checkExitCode, m_totalInfos, m_totalWarnings,
                          m_totalErrors);
    }
  });
}

void cs8Linter::startLinterAndChecker() {
  executeVal3Check();
  QSettings settings;
  bool linterEnabled = settings.value("linterEnabled", true).toBool();
  if (linterEnabled)
      executeLinter();
  else {
      m_linterDone = true;
      emit sendOutput(qUtf8Printable("SAXE Val3 Linter globally disabled\n"));
  }
}

void cs8Linter::exitProgram(int exitCode, int totalInfos, int totalWarnings,
                            int totalErrors) {
    showToastNotification("Val3 syntax check completed",
                          QString("%1 Warning(s), %2 Error(s)").arg(totalWarnings).arg(totalErrors),
                          totalErrors > 0);
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
        //std::wcout << L"The toast has timed out" << std::endl;
        break;
    default:
      std::wcout << L"Toast not activated" << std::endl;
      break;
    }
  }
};

void cs8Linter::showToastNotification(const QString &msg, const QString &info,
                                      bool longDuration) {
  WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
  QString text = msg;
  templ.setTextField(text.toStdWString(), WinToastTemplate::FirstLine);
  templ.setTextField(info.toStdWString(), WinToastTemplate::SecondLine);
  // templ.setExpiration(1000);
  if (longDuration)
    templ.setDuration(WinToastLib::WinToastTemplate::Long);
  else
    templ.setDuration(WinToastLib::WinToastTemplate::Short);
  if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
    qDebug() << "Toast failed: ";
  }
}

cs8Linter::cs8Linter(const QStringList &arguments,
                     const QString &val3checkExecutable, bool overrideIOCheck,
                     QObject *parent)
    : QObject(parent), m_val3checkDone(false), m_linterDone(false),
      m_val3checkExitCode(0), m_totalErrors(0), m_totalWarnings(0){
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
