#ifndef CS8LINTER_H
#define CS8LINTER_H

#include "cs8application.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

class QSystemTrayIcon;
class cs8Linter : public QObject {
  Q_OBJECT
public:
  explicit cs8Linter(const QStringList &arguments,
                     const QString &val3checkExecutable, bool overrideIOCheck,
                     QObject *parent = nullptr);

signals:
  void finished(int, int, int);
  void sendOutput(const QByteArray &);
  void allDoneAndExit(int, int, int, int);

protected:
  QStringList m_arguments;
  QStringList m_applicationsToCheck;
  QString m_val3checkExecutable;
  QString m_cellPath;
  QFuture<void> m_future;
  QFutureWatcher<void> m_futureWatcher;
  QByteArray m_val3CheckOutput;

  bool m_val3checkDone;
  bool m_linterDone;
  int m_val3checkExitCode;
  int m_totalErrors;
  int m_totalWarnings;
  int m_totalInfos;

  void executeLinter();
  void executeGITIntegration(const cs8Application &app);

  void initConnections();

protected slots:
  void executeVal3Check();
  void output(const QByteArray &out);
  void startLinterAndChecker();
  void exitProgram(int exitCode, int totalInfos, int totalWarnings,
                   int totalErrors);
  void showToastNotification(const QString &msg, const QString &info, bool longDuration);
};

#endif // CS8LINTER_H
