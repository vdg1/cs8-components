#include "cs8telnet.h"
#include "qttelnet.h"

#include <QTimer>

cs8Telnet::cs8Telnet(QObject *parent) : cs8LogInterface(parent) {
  m_connection = new QtTelnet(this);
  connect(m_connection, &QtTelnet::message, this,
          &cs8Telnet::slotTelnetMessage);
  connect(m_connection, &QtTelnet::loginRequired, this,
          &cs8Telnet::slotTelnetLoginRequired);
  connect(m_connection, &QtTelnet::loginFailed, this,
          &cs8Telnet::slotTelnetLoginFailed);
  connect(m_connection, &QtTelnet::loggedOut, this,
          &cs8Telnet::slotTelnetLoggedOut);
  connect(m_connection, &QtTelnet::loggedIn, this,
          &cs8Telnet::slotTelnetLoggedIn);
  connect(m_connection, &QtTelnet::connectionError, this,
          &cs8Telnet::slotTelnetConnectionError);
  connect(m_connection, &QtTelnet::stateChanged, this,
          &cs8Telnet::slotTelnetStateChanged);

  m_areYouThereTimer = new QTimer(this);
  m_areYouThereTimer->setInterval(10000);
  connect(m_areYouThereTimer, &QTimer::timeout, this,
          &cs8Telnet::slotAreYouThereTimeout);

  m_autoReconnectTimer = new QTimer(this);
  m_autoReconnectTimer->setInterval(5000);
  connect(m_autoReconnectTimer, &QTimer::timeout, this,
          &cs8Telnet::slotAutoReconnectTimeout);

  setAreYouThereTimerEnabled(true);
  setAutoReconnect(true);
}

void cs8Telnet::connectToHost(const QString &address, quint16 port) {
  m_host = address;
  m_port = port;
  m_connection->connectToHost(address, port);
  if (m_autoReconnect)
      m_autoReconnectTimer->start();
}

void cs8Telnet::close()
{
 m_connection->socket ()->close ();
}

void cs8Telnet::slotTelnetMessage(const QString &text) {
  emit telnetLine(text);
  if (m_areYouThereTimerEnabled) {
    m_areYouThereTimer->stop();
    m_areYouThereTimer->start();
  }
}

void cs8Telnet::slotTelnetLoginRequired() {
  m_connection->login("default", "");
}

void cs8Telnet::slotTelnetLoginFailed() { emit loginFailed(); }

void cs8Telnet::slotTelnetLoggedOut() { emit loggedOut(); }

void cs8Telnet::slotTelnetLoggedIn() { emit loggedIn(); }

void cs8Telnet::slotTelnetConnectionError(QAbstractSocket::SocketError error) {
  emit connectionError(error);
}

void cs8Telnet::slotTelnetStateChanged(
    QAbstractSocket::SocketState socketState) {
  emit stateChanged(socketState);
  if (m_areYouThereTimerEnabled &&
      m_connection->socketState() == QAbstractSocket::ConnectedState)
    m_areYouThereTimer->start();
  if (!m_areYouThereTimerEnabled ||
      m_connection->socketState() == QAbstractSocket::UnconnectedState)
    m_areYouThereTimer->stop();
  if (m_autoReconnect &&
      m_connection->socketState() == QAbstractSocket::UnconnectedState)
    m_autoReconnectTimer->start();
  if (m_connection->socketState() != QAbstractSocket::UnconnectedState)
    m_autoReconnectTimer->stop();
}

void cs8Telnet::slotAreYouThereTimeout() { m_connection->sendAYT(); }

void cs8Telnet::slotAutoReconnectTimeout() {
  if (m_connection->socketState() == QAbstractSocket::UnconnectedState)
    connectToHost(m_host, m_port);
}

bool cs8Telnet::autoReconnect() const { return m_autoReconnect; }

void cs8Telnet::setAutoReconnect(bool autoReconnect) {
  m_autoReconnect = autoReconnect;
}

QAbstractSocket::SocketState cs8Telnet::state() const {
  return m_connection->socketState();
}

QString cs8Telnet::errorString() const {
    return m_connection->socket()->errorString();
}

QString cs8Telnet::host() const
{
    return m_connection->socket ()->peerName ();
}

bool cs8Telnet::areYouThereTimerEnabled() const {
  return m_areYouThereTimerEnabled;
}

void cs8Telnet::setAreYouThereTimerEnabled(bool areYouThereTimerEnabled) {
  m_areYouThereTimerEnabled = areYouThereTimerEnabled;
  if (m_areYouThereTimerEnabled &&
      m_connection->socketState() == QAbstractSocket::ConnectedState)
    m_areYouThereTimer->start();
  if (!m_areYouThereTimerEnabled)
    m_areYouThereTimer->stop();
}
