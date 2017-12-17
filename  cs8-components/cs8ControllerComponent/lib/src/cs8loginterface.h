#ifndef CS8LOGINTERFACE_H
#define CS8LOGINTERFACE_H

#include <QAbstractSocket>
#include <QObject>

class cs8LogInterface : public QObject {
  Q_OBJECT
public:
  explicit cs8LogInterface(QObject *parent = nullptr);

  virtual bool areYouThereTimerEnabled() const = 0;
  virtual void setAreYouThereTimerEnabled(bool areYouThereTimerEnabled) = 0;

  virtual bool autoReconnect() const = 0;
  virtual void setAutoReconnect(bool autoReconnect) = 0;
  virtual QString errorString() const = 0;
  virtual QAbstractSocket::SocketState state() const = 0;
    virtual QString host() const=0;


  enum ControllerType { CS8, CS9 };
  Q_ENUM(ControllerType)

public slots:
  virtual void connectToHost(const QString &address, quint16 port = 23) = 0;
  virtual void close()=0;

signals:
  void telnetLine(const QString &);
  void loginFailed();
  void loggedIn();
  void loggedOut();
  void connectionError(QAbstractSocket::SocketError error);
  void stateChanged(QAbstractSocket::SocketState socketState);

signals:

public slots:
};

#endif // CS8LOGINTERFACE_H
