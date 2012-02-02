//
// C++ Implementation: p_cs8ftpinstance
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "p_cs8ftpinstance.h"

#include <QFtp>
#include <QDebug>
#include <QMutex>
#include <QApplication>
#include <QThread>


QHash<Qt::HANDLE,P_cs8FTPInstance*> ftpInstancePool;


P_cs8FTPInstance* P_cs8FTPInstance::pinstance = 0;// initialize pointer
P_cs8FTPInstance* P_cs8FTPInstance::Instance ()
{
    Qt::HANDLE threadHandle=QThread::currentThreadId();
    qDebug() << "ftpEngine pool " << threadHandle;

    //if ( pinstance == 0 )  // is it the first call?
    if (!ftpInstancePool.contains(threadHandle))
    {
        pinstance = new P_cs8FTPInstance(); // create sole instance
        qDebug() << "Created new engine instance for thread " << threadHandle;
        ftpInstancePool[threadHandle]=pinstance;
    }
    return ftpInstancePool[threadHandle];//pinstance; // address of sole instance
}


P_cs8FTPInstance::P_cs8FTPInstance()
{
    m_ftp=0;
    m_error=QFtp::NoError;
    timer.setSingleShot(true);
    timer.setInterval(5000);
    initFtp();
}

P_cs8FTPInstance::~P_cs8FTPInstance()
{
    qDebug() << "P_cs8FTPInstance::~P_cs8FTPInstance() " << QThread::currentThreadId();
    //ftpInstancePool.remove(QThread::currentThreadId());
}

int P_cs8FTPInstance::connectToHost ( const QString & host, quint16 port )
{
    //qDebug() << "P_cs8FTPInstance::connectToHost " << host << port;

    m_host=host;
    m_aborted=false;
    m_urlInfoCache.clear();
    //!TODO
    connect ( &timer,SIGNAL ( timeout() ),this,SLOT ( slotTimeout() ) );

    uint id=m_ftp->connectToHost ( host,  port );
    qDebug() << "id " << id;
    return id;
}

int P_cs8FTPInstance::login ( const QString & user , const QString & password )
{
    //qDebug() << "P_cs8FTPInstance::login " << user << password;

    m_userName=user;
    m_password=password;
    uint id=m_ftp->login ( user,password );
    qDebug() << "id " << id;
    return id;

}

void P_cs8FTPInstance::slotStateChanged ( int state )
{
    qDebug() << "ftp state changed: "<< state;


    switch ( state )
    {
    case 0: qDebug() << "QFtp::Unconnected"; break;
    case 1: qDebug() << "QFtp::HostLookup"; break;
    case 2: qDebug() << "QFtp::Connecting"; break;
    case 3: qDebug() << "QFtp::Connected"; break;
    case 4: qDebug() << "QFtp::LoggedIn"; break;
    case 5: qDebug() << "QFtp::Closing"; break;
    default:
        qDebug() << "new state" << state;
    }

    if (state==QFtp::Unconnected){
        timer.stop();}
    else{
        if ( state<QFtp::Connected )
            timer.start ();
        else
            timer.stop();
    }
}


void P_cs8FTPInstance::slotTimeout()
{

    qDebug() << "P_cs8FTPInstance::slotTimeout() state " << m_ftp->state();
    if ( m_ftp->state() <=4 )
    {
        qDebug() << "clearing pending commands";
        m_ftp->abort();
        m_ftp->clearPendingCommands();
        m_ftp->close();
        m_error=100;
        delete m_ftp;
        m_ftp=0;
        m_aborted=true;

        initFtp();
    }
    timer.stop();

}



/*!
    \fn P_cs8FTPInstance::size(const QString & file)
 */
int P_cs8FTPInstance::size ( const QString & file )
{

    m_fileSize=0;
    //qDebug() << "P_cs8FTPInstance::size: current id: " << m_ftp->currentId();
    return m_ftp->rawCommand ( QString ( "size %1" ).arg ( file ) );

}


/*!
    \fn P_cs8FTPInstance::slotRawCommandReply( int replyCode, const QString & detail )
 */
void P_cs8FTPInstance::slotRawCommandReply ( int replyCode, const QString & detail )
{

    //qDebug() << "raw reply " << ( qint64 ) replyCode/100 << detail;
    bool result= ( qint64 ) replyCode/100==2;
    m_fileSize=result?detail.toUInt() :0;
}



/*!
    \fn P_cs8FTPInstance::slotCommandStarted(int id)
 */
void P_cs8FTPInstance::slotCommandStarted ( int id )
{
    m_aborted=false;
    m_lastTransferedBytes=0;
    m_lastTransferTime=QTime::currentTime();
    timer.start();
}


/*!
    \fn P_cs8FTPInstance::slotCommandFinished(int id, bool error)
 */
void P_cs8FTPInstance::slotCommandFinished ( int id, bool error )
{

    //qDebug() << "P_cs8FTPInstance::slotCommandFinished() id " <<  id << " error: " << error;
    m_error=m_ftp->error();
    timer.stop();
}

void P_cs8FTPInstance::initFtp()
{


    if ( !m_ftp )
        m_ftp = new QFtp ( this );
    connect ( m_ftp,SIGNAL ( stateChanged ( int ) ),
             this,SLOT ( slotStateChanged ( int ) ) );
    connect ( m_ftp,SIGNAL ( rawCommandReply ( int, const QString & ) ),
             this,SLOT ( slotRawCommandReply ( int, const QString & ) ) );
    connect ( m_ftp,SIGNAL ( commandStarted ( int ) ),
             this,SLOT ( slotCommandStarted ( int ) ) );
    connect ( m_ftp,SIGNAL ( commandFinished ( int, bool ) ),
             this,SLOT ( slotCommandFinished ( int, bool ) ) );
    connect ( m_ftp,SIGNAL(dataTransferProgress(qint64,qint64)),
             this,SLOT(slotDataTransferProgress(qint64,qint64)));

    m_urlInfoCache.clear();
    m_urlInfoCache.setMaxCost ( 10000 );

}

void P_cs8FTPInstance::slotDataTransferProgress(qint64 var1,qint64 var2)
{
    emit dataTransferProgress(var1, var2);
    qint64 transferRate; // [bytes/sec]
    qreal timePassed;

    if (var1==0)
    {
        m_lastTransferTime=QTime::currentTime();
        return;
    }

    timePassed=qMax(1,m_lastTransferTime.secsTo(QTime::currentTime()));
    transferRate=var1/timePassed;
    emit dataTransferRate(transferRate);
    // restart time out timer
    timer.stop();
    timer.start();
}

int P_cs8FTPInstance::error()
{
    return m_error;
}


QString P_cs8FTPInstance::errorString()
{

    switch ( m_error )
    {
    case 100:
        return tr ( "Communication Timeout" );
        break;

    default:
        return m_ftp->errorString();
        break;
    }
}


/*!
    \fn P_cs8FTPInstance::addCachedInfo(const QString & filePath, QUrlInfo info)
 */
void P_cs8FTPInstance::addCachedInfo ( const QString & filePath, QUrlInfo info )
{
    //qDebug() << "P_cs8FTPInstance::addCachedInfo() " << this << filePath;
    if ( m_urlInfoCache.contains ( filePath ) )
        m_urlInfoCache.remove ( filePath );
    QUrlInfo* i = new QUrlInfo ( info );
    m_urlInfoCache.insert (filePath, i );
}


QCache< QString, QUrlInfo >* P_cs8FTPInstance::urlInfoCache()
{
    return &m_urlInfoCache;
}


/*!
    \fn P_cs8FTPInstance::cachedInfo(const QString & fileName)
 */
QUrlInfo* P_cs8FTPInstance::cachedInfo ( const QString & fileName )
{
    return m_urlInfoCache.contains ( fileName ) ?m_urlInfoCache.object ( fileName ) :new QUrlInfo();
}


/*!
    \fn P_cs8FTPInstance::cacheContains(const QString & fileName)
 */
bool P_cs8FTPInstance::cacheContains ( const QString & fileName )
{
    //	foreach(QUrlInfo* info, QCache< QString, QUrlInfo >)
    //	{
    //		qDebug() << info->path();
    //	}
    return m_urlInfoCache.contains ( fileName ) ;
}

void P_cs8FTPInstance::waitFtp()
{
    bool quit;
    do
    {

        qApp->processEvents ( /*QEventLoop::ExcludeUserInputEvents*/ );


        if (m_ftp!=0)
            quit=(m_ftp->currentCommand() !=QFtp::None) && !aborted();
    }
    //while ( (m_ftp->ftp()->currentCommand() !=QFtp::None) && !m_ftp->aborted() && (m_ftp->error()==QFtp::NoError));
    while ( quit);
}
