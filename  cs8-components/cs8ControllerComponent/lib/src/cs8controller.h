#ifndef CS8CONTROLLER_H
#define CS8CONTROLLER_H
//
#include <QObject>
#include <QBuffer>
#include <QUrl>
#include <QDomDocument>
#include <QTimer>
#include <QFtp>
//

namespace cs8 { enum serialNumberType { Any, ArmSerialNumber, MachineSerialNumber }; }



class cs8Controller : public QObject
{
    Q_OBJECT

public:

    bool logFile ( QString & text );
    bool serialNumber(QString & number, cs8::serialNumberType=cs8::Any);
    bool armType(QString & armType);
    void abort();
    void setLoginData ( const QString & userName, const QString & password=QString ( "" ) );
    void setAddress ( const QString & value );
    QString address() { return m_url.host(); }
    cs8Controller();
    QString armSerialNumber();
    QString controllerSerialNumber();

    //QString serialNumber ( cs8::serialNumberType type );
    QString armType();
    QString lastError() const;
    QUrl url() const
    {
        return m_url;
    }
    QString urlWithoutPath() const;
    static bool isOnline(const QString & address);
    bool isOnline() { return m_isOnline;}
    void enableOnlineCheck(bool enable_);

        signals:
    void isOnline(bool online, int error, const QString & errorString);

        public slots:

        protected:
    bool m_isOnline;
    bool m_checkOnline;
    QString m_lastError;
    QUrl m_url;
    QFtp *m_ftp;
    QTimer *m_onlineTimer;
    QString configFileItem ( const QDomDocument & configFile_, const QString & type_, const QString & name_, const QString & default_=QString() );
    QString orderNumber(const QDomDocument & doc);

        protected slots:
    void slotOnlineTimerTimeout();
    void slotCommandFinished(int id, bool error);
    void slotStartCheck();
    void slotStopCheck();
};
#endif


