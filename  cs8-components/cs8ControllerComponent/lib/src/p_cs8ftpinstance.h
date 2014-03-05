//
// C++ Interface: p_cs8ftpinstance
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef P_CS8FTPINSTANCE_H
#define P_CS8FTPINSTANCE_H

#include <QFtp>
#include <QTimer>
#include <QCache>
#include <QUrlInfo>


/**
 @author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/


class P_cs8FTPInstance : public QObject
{
    Q_OBJECT
protected slots:
    void slotTimeout();
    void slotStateChanged ( int state );
    void slotRawCommandReply ( int replyCode, const QString & detail ) ;
    void slotCommandStarted (int);
    void slotCommandFinished (int id, bool error);
    void slotDataTransferProgress(qint64,qint64);

signals:
    void dataTransferProgress(qint64,qint64);
    void dataTransferRate(qint64);

public:
    ~P_cs8FTPInstance();
    void waitFtp();
    QString errorString();
    int error();
    QFtp* ftp() { return m_ftp; }
    int login ( const QString & user = QString(), const QString & password = QString() );
    int connectToHost ( const QString & host, quint16 port = 21 );
    QString password()
    {
        return m_password;
    }
    QString userName()
    {
        return m_userName;
    }
    QString host()
    {
        return m_host;
    }
    static P_cs8FTPInstance* Instance();
    int size ( const QString & file );

    qint64 fileSize() const
    {
        return m_fileSize;
    }

    bool aborted() const
    {
        return m_aborted;
    }
    void addCachedInfo ( const QString & filePath, QUrlInfo info );

    QCache< QString, QUrlInfo >* urlInfoCache();
    QUrlInfo* cachedInfo(const QString & fileName);
    bool cacheContains(const QString & fileName);


protected:
    P_cs8FTPInstance();

    P_cs8FTPInstance ( const P_cs8FTPInstance& );
    P_cs8FTPInstance& operator= ( const P_cs8FTPInstance& );
    void initFtp();



private:
    QString m_errorSring;
    int m_error;
    QFtp* m_ftp;
    QTimer timer;
    QString m_password;
    QString m_userName;
    QString m_host;
    static P_cs8FTPInstance* pinstance;
    quint64 m_lastTransferedBytes;
    QTime m_lastTransferTime;

protected:
    quint64 m_fileSize;
    bool m_aborted;
    QCache<QString, QUrlInfo> m_urlInfoCache;
};

#endif
