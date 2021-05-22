#ifndef CS8LINTER_H
#define CS8LINTER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

class cs8Linter : public QObject {
  Q_OBJECT
public:
  explicit cs8Linter(const QStringList &arguments,
                     const QString &val3checkExecutable, bool overrideIOCheck,
                     QObject *parent = nullptr);

signals:
  void finished();
  void sendOutput(const QByteArray &);

protected:
  QStringList m_arguments;
  QStringList m_applicationsToCheck;
  QString m_val3checkExecutable;
  QString m_cellPath;
  QFuture<void> m_future;
  QFutureWatcher<void> m_futureWatcher;

  bool m_val3checkDone;
  bool m_linterDone;
  int m_val3checkExitCode;

  void executeLinter();

  void initConnections();

protected slots:
  void executeVal3Check();
  void output(const QByteArray &out);
  void startLinterAndChecker();
};

#endif // CS8LINTER_H
