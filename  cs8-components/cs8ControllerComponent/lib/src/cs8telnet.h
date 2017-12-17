#ifndef CS8TELNET_H
#define CS8TELNET_H

#include "cs8loginterface.h"

#include <QAbstractSocket>
#include <QObject>

class QtTelnet;
class QTimer;

class cs8Telnet : public cs8LogInterface {
  Q_OBJECT
public:
  explicit cs8Telnet(QObject *parent = nullptr);

  bool areYouThereTimerEnabled() const;
  void setAreYouThereTimerEnabled(bool areYouThereTimerEnabled);

  bool autoReconnect() const;
  void setAutoReconnect(bool autoReconnect);

  QAbstractSocket::SocketState state() const;
  QString errorString() const;
  QString host() const;
  /*
 signals:
  void telnetLine(const QString &);
  void loginFailed();
  void loggedIn();
  void loggedOut();
  void connectionError(QAbstractSocket::SocketError error);
  void stateChanged(QAbstractSocket::SocketState socketState);
*/

public slots:
  void connectToHost(const QString &address, quint16 port = 23);
  void close();

protected slots:
  void slotTelnetMessage(const QString &text);
  void slotTelnetLoginRequired();
  void slotTelnetLoginFailed();
  void slotTelnetLoggedOut();
  void slotTelnetLoggedIn();
  void slotTelnetConnectionError(QAbstractSocket::SocketError error);
  void slotTelnetStateChanged(QAbstractSocket::SocketState socketState);
  void slotAreYouThereTimeout();
  void slotAutoReconnectTimeout();

private:
  bool m_areYouThereTimerEnabled;
  bool m_autoReconnect;
  QString m_host;
  quint16 m_port;
  QtTelnet *m_connection;
  QTimer *m_areYouThereTimer;
  QTimer *m_autoReconnectTimer;
};

#endif // CS8TELNET_H
