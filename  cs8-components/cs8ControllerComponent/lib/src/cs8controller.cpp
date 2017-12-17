#include "cs8controller.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QUrl>
#include "qftp.h"

//
cs8Controller::cs8Controller()
    : QObject(), m_isOnline(false), m_checkOnline(false), m_ftp(0) {
  m_onlineTimer = new QTimer(this);
  m_onlineTimer->setSingleShot(true);
  connect(m_onlineTimer, &QTimer::timeout, this,
          &cs8Controller::slotOnlineTimerTimeout);

  // create time out timer
  m_transferTimeOut = new QTimer(this);
  m_transferTimeOut->setInterval(5000);

  m_rateTime = new QElapsedTimer();

  setLoginData("maintenance", "spec_cal");
  m_url.setHost("");
  m_url.setScheme("ftp");
  m_url.setPath("/log/errors.log");
  m_lastError = tr("No Error");
}
//

void cs8Controller::slotOnlineTimerTimeout() {
  qDebug() << __FUNCTION__ << " state: " << m_ftp->state()
           << " host: " << m_url.host();
  switch (m_ftp->state()) {
    case QFtp::Unconnected:
      qDebug() << "       connecting to " << m_url.host() << m_url.userName()
               << m_url.password();
      m_ftp->connectToHost(m_url.host());
      m_ftp->login(m_url.userName(), m_url.password());
      break;

    case QFtp::LoggedIn:
      m_ftp->rawCommand("NOOP");
      break;

    case QFtp::Closing:
      m_ftp->abort();
      break;

    default:
      m_ftp->abort();
      m_ftp->close();
      break;
  }

  if (m_checkOnline) slotStartCheck();
}

void cs8Controller::initializeFTPSession() {
  if (m_ftp != nullptr) m_ftp->deleteLater();
  m_ftp = new QFtp(this);
  connect(m_ftp, &QFtp::commandFinished, this,
          &cs8Controller::slotCommandFinished);
  connect(m_ftp, &QFtp::dataTransferProgress, this,
          &cs8Controller::slotDataTransferProgress);
}

void cs8Controller::setLoginData(const QString &userName,
                                 const QString &password) {
  initializeFTPSession();
  m_url.setUserName(userName);
  m_url.setPassword(password);
}

void cs8Controller::setAddress(const QString &value) {
  m_url.setHost(value);
  m_isOnline = false;
  emit onlineChanged(m_isOnline, 0, QString());

  if (m_checkOnline) slotStartCheck();
}

void cs8Controller::enableOnlineCheck(bool enable_) {
  m_checkOnline = enable_;
  if (m_checkOnline)
    slotStartCheck();
  else
    slotStopCheck();
}

bool cs8Controller::getFileContent(const QString &fileName, QByteArray &data) {
  qint64 dummy;
  return getFileContent(fileName, data, dummy);
}

void cs8Controller::preFTPTransfer(QEventLoop *eventLoop, bool *timeout) {
  qDebug() << __FUNCTION__;
  slotStopCheck();
  *timeout = false;
  m_ftp->disconnect(SIGNAL(commandFinished(int, bool)));
  // timeout signal
  m_connTimeOut = QObject::connect(m_transferTimeOut, &QTimer::timeout, [=]() {
    qDebug() << "Aborting due to time out";
    m_ftp->abort();
    *timeout = true;
    eventLoop->quit();
  });
  // connect dataprogress signal
  m_connDataProgress =
      QObject::connect(m_ftp, &QFtp::dataTransferProgress, [=]() {
        m_transferTimeOut->stop();
        m_transferTimeOut->start();
      });
  // connect listinfo signal
  m_connListInfo = QObject::connect(m_ftp, &QFtp::listInfo, [=]() {
    m_transferTimeOut->stop();
    m_transferTimeOut->start();
  });
  m_transferTimeOut->start(5000);
}

void cs8Controller::postFTPTransfer() {
  qDebug() << __FUNCTION__;
  // disconnect signals

  QObject::disconnect(m_connDataProgress);
  QObject::disconnect(m_connListInfo);
  QObject::disconnect(m_connTimeOut);
  m_transferTimeOut->stop();

  m_lastError = m_ftp->errorString();
  connect(m_ftp, &QFtp::commandFinished, this,
          &cs8Controller::slotCommandFinished);
  if (m_checkOnline) slotStartCheck();
  emit transferProgress(0, 0, 0);
}

bool cs8Controller::getFileContent(const QString &fileName, QByteArray &data,
                                   qint64 &sizeOnServer) {
  qDebug() << __FUNCTION__ << fileName;
  if (!checkFtpSession()) return false;
  // initialize operation
  // prepare local event loop
  QEventLoop *eventLoop = new QEventLoop(this);
  bool timeout = false;
  bool *timeoutPtr = &timeout;
  qint64 fileSize = 0;
  //
  preFTPTransfer(eventLoop, timeoutPtr);
  m_url.setPath(fileName);

  // connect done signal
  QMetaObject::Connection connDone =
      QObject::connect(m_ftp, &QFtp::done, [=](bool error) {
        qDebug() << "command finished with error " << error;
        eventLoop->quit();
      });

  QMetaObject::Connection connTransferProgress = QObject::connect(
      m_ftp, &QFtp::dataTransferProgress, [&](qint64 done, qint64 total) {
        Q_UNUSED(done);
        fileSize = total;
      });

  m_rateTime->start();
  QBuffer buffer(&data);
  if (buffer.open(QIODevice::ReadWrite)) {
    int id = m_ftp->get(fileName, &buffer, QFtp::Binary);
    qDebug() << __FUNCTION__ << ": retrieve " << m_url.toString()
             << " id: " << id;
    eventLoop->exec();
    qDebug() << __FUNCTION__ << ": finished";
  }
  // clean up
  disconnect(connDone);
  disconnect(connTransferProgress);
  eventLoop->deleteLater();

  postFTPTransfer();
  qDebug() << "timeout occured during operation: " << timeout;
  sizeOnServer = fileSize;
  return (m_ftp->error() == QFtp::NoError && !timeout);
}

bool cs8Controller::getFolderContents(const QString &path,
                                      QList<QUrlInfo> &list) {
  qDebug() << __FUNCTION__ << path;
  if (!checkFtpSession()) return false;
  // initialize operation
  // prepare local event loop
  QEventLoop *eventLoop = new QEventLoop(this);
  bool timeout = false;
  bool *timeoutPtr = &timeout;
  preFTPTransfer(eventLoop, timeoutPtr);
  m_url.setPath(path);
  QList<QUrlInfo> *resultList = new QList<QUrlInfo>;

  // connect done signal
  QMetaObject::Connection connDone =
      QObject::connect(m_ftp, &QFtp::done, [=](bool error) {
        qDebug() << "command finished with error " << error;
        eventLoop->quit();
      });
  // connect listInfo signal
  QMetaObject::Connection connListInfo =
      connect(m_ftp, &QFtp::listInfo, [=](const QUrlInfo &i) {
        resultList->append(i);
        qDebug() << "listinfo: " << i.name();
      });
  int id = m_ftp->list(m_url.path());
  qDebug() << __FUNCTION__ << ": retrieve " << m_url.toString()
           << " id: " << id;
  eventLoop->exec();
  qDebug() << __FUNCTION__ << ": finished";
  // read results
  for (int i = 0; i < resultList->size(); i++) {
    QUrlInfo info = resultList->at(i);
    info.setName(path + "/" + info.name());
    list.append(info);
  }
  // clean up
  delete resultList;
  eventLoop->deleteLater();
  disconnect(connListInfo);
  disconnect(connDone);
  postFTPTransfer();
  qDebug() << "timeout occured during operation: " << timeout;
  return m_ftp->error() == QFtp::NoError && !timeout;
}

bool cs8Controller::downloadFile(const QString &remoteFileName,
                                 const QString &localFileName,
                                 qint64 &sizeOnServer) {
  if (!checkFtpSession()) return false;
  QByteArray data;
  if (!getFileContent(remoteFileName, data, sizeOnServer)) return false;
  QFile file(localFileName);
  if (!file.open(QFile::ReadWrite)) {
    m_lastError = file.errorString();
    return false;
  }
  file.write(data);
  return true;
}

void cs8Controller::slotStartCheck() {
  qDebug() << __FUNCTION__;
  if (m_checkOnline) {
    m_onlineTimer->stop();
    m_onlineTimer->start(5000);
  }
}

void cs8Controller::slotStopCheck() {
  qDebug() << __FUNCTION__;
  m_onlineTimer->stop();
}

void cs8Controller::slotCommandFinished(int id, bool error) {
  qDebug() << __FUNCTION__ << " : cmd finished: " << id
           << " error: " << m_ftp->errorString() << " state: " << m_ftp->state()
           << " pending commands: " << m_ftp->hasPendingCommands();

  slotStopCheck();
  if (error) {
    qDebug() << "error: " << m_ftp->error() << m_ftp->errorString();
    m_lastError = m_ftp->errorString();
    slotStartCheck();
    m_isOnline = false;
    emit onlineChanged(m_isOnline, m_ftp->error(), m_ftp->errorString());
    qDebug() << m_url.host() << "is offline";
    // m_ftp->close();
    return;
  }
  if (m_ftp->state() == QFtp::LoggedIn) {
    m_isOnline = true;
    emit onlineChanged(m_isOnline, m_ftp->error(), m_ftp->errorString());
    qDebug() << m_url.host() << "is online";
    if (m_checkOnline) {
      QTimer::singleShot(5000, [=]() { m_ftp->rawCommand("NOOP"); });
    }
    return;
  }

  if (m_ftp->state() == QFtp::Unconnected) {
    slotStartCheck();
    return;
  }
}

void cs8Controller::slotDataTransferProgress(qint64 done, qint64 total) {
  qint64 elapsed = m_rateTime->elapsed();
  float rate = 0.0;

  if (elapsed > 0) rate = qMax((float)(done / (elapsed)), (float)0.0) * 1000;

  emit transferProgress(done, total, round(rate));
}
/*!
    \fn cs8Controller::logFile()
 */
bool cs8Controller::logFile(QString &text) {
  QByteArray data;
  qint64 size;
  if (!getFileContent("/log/errors.log", data, size)) {
    return false;
  }
  QTextStream stream(data);
  text = stream.readAll();
  return true;
}

bool cs8Controller::armType(QString &armType) {
  QDomDocument doc;
  QString docErrString;
  int docErrLine, docErrCol;
  qint64 size;
  QByteArray data;
  if (!getFileContent("/sys/configs/options.cfx", data, size)) {
    return false;
  }

  if (!doc.setContent(data, &docErrString, &docErrLine, &docErrCol)) {
    return false;
  }

  QDomElement element = doc.documentElement();
  for (int i = 0; i < element.elementsByTagName("String").count(); i++) {
    QDomNode node = element.elementsByTagName("String").at(i);
    if (node.toElement().attribute("name") == "arm") {
      // check if arm type value is not an empty string
      armType = node.toElement().attribute("value");
      if (!armType.isEmpty()) {
        // extract arm type from tuning identifier
        QRegExp rx;
        rx.setPattern("(.*)(-\\*)");
        if (rx.indexIn(armType) > -1) armType = rx.cap(1);
        return true;
      }
    }
  }
  return false;
}

/*!
  \fn cs8Controller::serialNumber()
  Retrieve serial number of controller. If strategy is 0, first
  /sys/config/options.cfx is checked, then /usr/configs/arm.cfx. If strategy is
  1 only /sys/config/options.cfx is checked.
  */
bool cs8Controller::serialNumber(QString &number,
                                 const cs8::serialNumberType source) {
  QDomDocument doc;
  QString docErrString;
  // QFile file;

  int docErrLine, docErrCol;
  number = "";
  bool ok = true;
  qint64 size;
  QByteArray data;
  if (source == cs8::Any || source == cs8::MachineSerialNumber) {
    if (!getFileContent("/sys/configs/options.cfx", data, size)) {
      if (source == cs8::MachineSerialNumber) ok = false;
    }

    if (!doc.setContent(data, &docErrString, &docErrLine, &docErrCol)) {
      if (source == cs8::MachineSerialNumber) ok = false;
    }
    if (ok) {
      // reading /sys/config/options.cfx succeeded
      number = orderNumber(doc);
      return true;
    } else {
      // read controller serial number from /usr/configs/controller.cfx
      // as of version s7
      if (!getFileContent("/usr/configs/controller.cfx", data, size)) {
        if (source == cs8::MachineSerialNumber) return false;
      }

      if (!doc.setContent(data, &docErrString, &docErrLine, &docErrCol)) {
        if (source == cs8::MachineSerialNumber) return false;
      } else {
        // reading /sys/config/options.cfx succeeded
        number = orderNumber(doc);
        return true;
      }
    }
  }
  if (source == cs8::Any || source == cs8::ArmSerialNumber) {
    if (!getFileContent("/usr/configs/arm.cfx", data, size)) {
      return false;
    }

    if (!doc.setContent(data, &docErrString, &docErrLine, &docErrCol)) {
      return false;
    } else {
      // reading /sys/config/options.cfx succeeded
      number = orderNumber(doc);
      return true;
    }
  }
  return false;
}

QString cs8Controller::orderNumber(const QDomDocument &doc) {
  QDomElement element = doc.documentElement();
  for (int i = 0; i < element.elementsByTagName("String").count(); i++) {
    QDomNode node = element.elementsByTagName("String").at(i);
    if (node.toElement().attribute("name") == "orderNumber" ||
        node.toElement().attribute("name") == "serialNumber") {
      // check if orderNumber value is not an empty string
      QString orderNumber = node.toElement().attribute("value");
      if (!orderNumber.isEmpty()) {
        return orderNumber.replace("/", "_");
      }
    }
  }
  // return empty string in case order number could not be determined
  // return ui.ftpServerLineEdit->lineEdit()->text();
  return QString();
}

bool cs8Controller::checkFtpSession() {
  qDebug() << __FUNCTION__ << "Current FTP session state: " << m_ftp->state();
  if (m_ftp->state() != QFtp::LoggedIn) {
    initializeFTPSession();
    // m_ftp->abort();
    // m_ftp->close();
    m_ftp->connectToHost(m_url.host());
    int idLogin = m_ftp->login(m_url.userName(), m_url.password());
    qDebug() << "Login command ID " << idLogin;
    QEventLoop *eventLoop = new QEventLoop(this);

    QTimer *timeout = new QTimer(this);
    timeout->setSingleShot(true);
    timeout->start(5000);

    QMetaObject::Connection connTimeOut =
        QObject::connect(timeout, &QTimer::timeout, [=]() {
          qDebug() << "Aborting due to time out";
          m_ftp->abort();
          eventLoop->quit();
        });

    QMetaObject::Connection connCommandFinished = QObject::connect(
        m_ftp, &QFtp::commandFinished, [=](int id, bool error) {
          qDebug() << "command " << id << " finished with error " << error;
          if (id == idLogin || error) eventLoop->quit();
        });

    // connect(m_ftp,&QFtp::done,eventLoop,&QEventLoop::quit);
    eventLoop->exec();
    eventLoop->deleteLater();
    timeout->deleteLater();
    disconnect(connCommandFinished);
    disconnect(connTimeOut);
  }
  m_lastError = m_ftp->errorString();
  bool ok = m_ftp->state() == QFtp::LoggedIn;

  return ok;
}

void cs8Controller::abort() {}

/*!
    \fn cs8Controller::controllerSerialNumber()
 */
QString cs8Controller::controllerSerialNumber() {
  QString number;
  serialNumber(number, cs8::MachineSerialNumber);
  return number;
}

/*!
    \fn cs8Controller::armSerialNumber()
 */
QString cs8Controller::armSerialNumber() {
  QString number;
  serialNumber(number, cs8::ArmSerialNumber);
  return number;
}

/*!
    \fn cs8Controller::configFileItem(const QDomDocument & configFile, const
   QString & type, const QString & name, const QString & default=QString())
 */
QString cs8Controller::configFileItem(const QDomDocument &configFile_,
                                      const QString &type_,
                                      const QString &name_,
                                      const QString &default_) {
  QDomElement element = configFile_.documentElement();
  for (int i = 0; i < element.elementsByTagName(type_).count(); i++) {
    QDomNode node = element.elementsByTagName(type_).at(i);
    if (node.toElement().attribute("name") == name_)
      return node.toElement().attribute("value");
  }
  return default_;
}

/*!
    \fn cs8Controller::serialNumber(cs8::serialNumberType type)
 */
/*
QString cs8Controller::serialNumber ( cs8::serialNumberType type )
{
    if ( type==cs8::MachineSerialNumber )
        m_url.setPath ( "sys/configs/options.cfx" );
    else
        m_url.setPath ( "usr/configs/arm.cfx" );
    QFile file ( m_url.toString() );
    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        m_lastError=file.errorString();
        return false;
    }
    QDomDocument doc;
    QTextStream in(&file);
    QString document=in.readAll();
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent ( document, &errorMsg, &errorLine,&errorColumn ) )
    {
        m_lastError=QString("%1:%2
%3,%4").arg(m_url.toString()).arg(errorMsg).arg(errorLine).arg(errorColumn);
        qWarning() << m_lastError;
        qDebug() << document;
        return false;
    }
    file.close();
    return configFileItem ( doc,"String","orderNumber" ).replace ( "/","_" );
}
*/

/*!
    \fn cs8Controller::armType()
 */
QString cs8Controller::armType() {
  QByteArray data;
  qint64 size;
  if (!getFileContent("usr/configs/arm.cfx", data, size)) {
    return QString();
  }
  QDomDocument doc;
  if (!doc.setContent(data)) return QString();

  return configFileItem(doc, "String", "arm");
}

/*!
    \fn cs8Controller::lastError() const
 */
QString cs8Controller::lastError() const { return m_lastError; }

bool cs8Controller::isOnline(const QString &address) {
  QTcpSocket socket;
  int timeout = 2000;
  socket.connectToHost(address, 21);
  return socket.waitForConnected(timeout);
}

QString cs8Controller::urlWithoutPath() const {
  QUrl url = m_url;
  qDebug() << url.toString(QUrl::RemovePath);
  return url.toString(QUrl::RemovePath);
}
