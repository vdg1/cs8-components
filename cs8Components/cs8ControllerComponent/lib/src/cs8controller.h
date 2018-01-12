#ifndef CS8CONTROLLER_H
#define CS8CONTROLLER_H
//
#include <QBuffer>
#include <QDomDocument>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>
#include <QUrl>
//#include <QFtp>
#include "cs8telnet.h"
#include "qftp.h"
//

namespace cs8 {
enum serialNumberType { Any, ArmSerialNumber, MachineSerialNumber };
}

class cs8Controller : public QObject {
  Q_OBJECT

  void preFTPTransfer(QEventLoop *eventLoop, bool *timeout);

  void postFTPTransfer();

 public:
  bool logFile(QString &text);
  bool serialNumber(QString &number, const cs8::serialNumberType = cs8::Any);
  bool armType(QString &armType);
  void abort();
  void setLoginData(const QString &userName,
                    const QString &password = QString(""));
  void setAddress(const QString &value);
  QString address() { return m_url.host(); }
  cs8Controller();
  QString armSerialNumber();
  QString controllerSerialNumber();

  // QString serialNumber ( cs8::serialNumberType type );
  QString armType();
  QString lastError() const;
  QUrl url() const { return m_url; }
  QString urlWithoutPath() const;
  static bool isOnline(const QString &address);
  bool isOnline() { return m_isOnline; }
  void enableOnlineCheck(bool enable_);

  bool getFileContent(const QString &fileName, QByteArray &data);
  bool getFileContent(const QString &fileName, QByteArray &data,
                      qint64 &sizeOnServer);
  bool getFolderContents(const QString &path, QList<QUrlInfo> &list);
  bool downloadFile(const QString &remoteFileName, const QString &localFileName,
                    qint64 &sizeOnServer);

 signals:
  void onlineChanged(bool online, int error, const QString &errorString);
  void transferProgress(qint64 done, qint64 total, double bytesPerSec);

 public slots:

 protected:
  bool m_isOnline;
  bool m_checkOnline;
  QString m_lastError;
  QUrl m_url;
  QMetaObject::Connection m_connTimeOut;
  QMetaObject::Connection m_connDataProgress;
  QMetaObject::Connection m_connListInfo;
  QFtp *m_ftp;
  QTimer *m_onlineTimer;
  QTimer *m_transferTimeOut;
  QElapsedTimer *m_rateTime;

  QString configFileItem(const QDomDocument &configFile_, const QString &type_,
                         const QString &name_,
                         const QString &default_ = QString());
  QString orderNumber(const QDomDocument &doc);
  bool checkFtpSession();
  void initializeFTPSession();

 protected slots:
  void slotOnlineTimerTimeout();
  void slotCommandFinished(int id, bool error);
  void slotDataTransferProgress(qint64 done, qint64 total);
  void slotStartCheck();
  void slotStopCheck();
};
#endif
