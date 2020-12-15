#ifndef CS9ONLINELOGGER_H
#define CS9ONLINELOGGER_H

#include "cs8loginterface.h"

#include <QProcess>

class cs9OnlineLogger : public cs8LogInterface {
  Q_OBJECT
public:
  cs9OnlineLogger(QObject *parent = nullptr);

  bool areYouThereTimerEnabled() const override;
  void setAreYouThereTimerEnabled(bool areYouThereTimerEnabled) override;

  bool autoReconnect() const override;
  void setAutoReconnect(bool autoReconnect) override;
  QString errorString() const override;
  QAbstractSocket::SocketState state() const override;
  QString host() const override;
  quint16 port() const override;

  QString keyFilePath() const;
  void setKeyFilePath(const QString &keyFilePath);

  QString plinkExecutable() const;
  void setPlinkExecutable(const QString &plinkExecutable);

public slots:
  void connectToHost(const QString &address, quint16 = 23) override;
  void close() override;
protected slots:
  void slotStateChanged(QProcess::ProcessState state);

  void errorOccurred(QProcess::ProcessError error);
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  void readyReadStandardError();
  void readyReadStandardOutput();
  void started();

protected:
  bool m_autoReconnect;
  QString m_lastError;
  QString m_host;
  quint16 m_port;

  QString m_keyFilePath;
  QString m_plinkExecutable;
  QProcess *m_plinkProcess;
  QAbstractSocket::SocketState m_connectionState;
};

#endif // CS9ONLINELOGGER_H
