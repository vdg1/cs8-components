//
// C++ Implementation: cs8fileengine
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8ControllerComponent.h"
#include "cs8fileengineiterator.h"
//#include "cs8fileengine.h"
//#include "ftpengine.h"

#include <QDateTime>
#include <QFile>
#include <QFtp>
#include <QAbstractFileEngine>
#include <QApplication>
#include <QDebug>
#include <QUrlInfo>
#include <QAbstractFileEngineIterator>
#include "p_cs8ftpinstance.h"

cs8FileEngine::cs8FileEngine ( const QString & file )
    : QAbstractFileEngine ( )
{
    //qDebug() << "create new instance of cs8FileEngine " << this;
    if ( !file.isEmpty() )
        setFileName ( file );

    m_cached=false;
    m_ftp=P_cs8FTPInstance::Instance();
    //connect ( m_ftp->ftp(),SIGNAL ( listInfo ( const QUrlInfo& ) ),this,SLOT ( slotListInfo ( const QUrlInfo& ) ) );
}

cs8FileEngine::~cs8FileEngine()
{
    //qDebug() << "destroy instance of cs8FileEngine " << this;
    //delete m_ftp;
}


bool cs8FileEngine::caseSensitive() const
{
    return true;//QAbstractFileEngine::caseSensitive();
}

bool cs8FileEngine::isRelativePath() const
{
    //path are always absolute as the complete url must be passed to the file engine
    return false;
}

bool cs8FileEngine::copy ( const QString & newName ){
    //qDebug() << "copy to " << newName;
    QByteArray buffer;
    if (!retrieveFile()){
        return false;
    }
    QFile file(newName);
    if (!file.open(QIODevice::WriteOnly)){
        return false;
    }
    if (file.write(m_buffer.readAll())==-1){
        file.close();
        return false;
    }
    file.close();
    return true;
}

bool cs8FileEngine::mkdir ( const QString& dirName, bool createParentDirectories )
{
    qDebug() << "cs8FileEngine::mkdir() " << dirName;
    qDebug() << "				" << m_url.userName();
    qDebug() << "				" << m_url.password();
    qDebug() << "				" << m_url.path();
    qDebug() << "				" << dirName;


    QStringList dirs=QUrl ( dirName ).path().split ( "/",QString::SkipEmptyParts );
    primeFtp();
    if ( !createParentDirectories )
        m_ftp->ftp()->mkdir ( dirName );
    else
        {
            QString buildPath=QString ( "/" );
            foreach ( QString dir, dirs )
                {
                    buildPath+=dir+"/";
                    m_ftp->ftp()->mkdir ( buildPath );
                    waitFtp();
                }
        }
    return waitFtp();
}


bool cs8FileEngine::remove()
{
    primeFtp();
    m_ftp->ftp()->remove ( m_url.path() );
    return waitFtp();
}

bool cs8FileEngine::rename ( const QString& newName )
{
    primeFtp();
    m_ftp->ftp()->rename ( m_url.path(), newName );
    return waitFtp();
}

bool cs8FileEngine::rmdir ( const QString& dirName, bool recurseParentDirectories ) const
{
    return QAbstractFileEngine::rmdir ( dirName, recurseParentDirectories );
}

bool cs8FileEngine::setPermissions ( uint perms )
{
    return QAbstractFileEngine::setPermissions ( perms );
}

bool cs8FileEngine::setSize ( qint64 size )
{
    return QAbstractFileEngine::setSize ( size );
}

QAbstractFileEngine::FileFlags cs8FileEngine::fileFlags ( FileFlags type ) const
{
    if ( ( m_url.path() =="/usr" ) || ( m_url.path() =="/sys" ) || ( m_url.path() =="/log" ) || ( m_url.path().isEmpty() ) )
        {
            return QAbstractFileEngine::DirectoryType |
                    QAbstractFileEngine::ReadOwnerPerm |
                    QAbstractFileEngine::WriteOwnerPerm |
                    QAbstractFileEngine::ReadUserPerm|
                    QAbstractFileEngine::WriteUserPerm|
                    QAbstractFileEngine::ReadGroupPerm|
                    QAbstractFileEngine::WriteGroupPerm |
                    QAbstractFileEngine::ExistsFlag;
        }
    if ( type && QAbstractFileEngine::Refresh!=0 )
        //qDebug() << "refresh requested";

        if ( !m_ftp->cacheContains ( m_url.toString() ) )
            {
                QString url=m_url.toString();
                if ( !m_url.path().isEmpty() )
                    url=url.left ( url.lastIndexOf ( "/" ) );
                QDir dir ( url );
                dir.entryList();
            }
    QUrlInfo* info=m_ftp->cachedInfo ( m_url.toString() );
    QAbstractFileEngine::FileFlags flags;
    //qDebug() << " file flags for " <<  m_url.toString() << " : " << flags;
    flags |= info->isReadable() ? QAbstractFileEngine::ReadUserPerm:flags;
    flags |= info->isWritable() ? QAbstractFileEngine::WriteUserPerm:flags;
    flags |= info->isFile() ? QAbstractFileEngine::FileType:flags;
    flags |= info->isSymLink() ? QAbstractFileEngine::LinkType:flags;
    flags |= info->isDir() ? QAbstractFileEngine::DirectoryType:flags;
    flags |= QAbstractFileEngine::ExistsFlag;


    qDebug() << " file flags for " <<  m_url.toString() << " : " << flags;
    qDebug() << "   dir " << ((flags && QAbstractFileEngine::DirectoryType) ? "1":"0");
    return flags;
    //return QAbstractFileEngine::fileFlags ( type );
}


QDateTime cs8FileEngine::fileTime ( FileTime time ) const
{
    //qDebug() << "Return date information of " << m_url.toString ();
    if (m_ftp->urlInfoCache ()->contains (m_url.toString ()))
        {
            QCache< QString, QUrlInfo >* cache=m_ftp->urlInfoCache ();
            switch (time)
                {
                case CreationTime:
                    return cache->object (m_url.toString ())->lastModified ();
                    break;

                case ModificationTime:
                    return cache->object (m_url.toString ())->lastModified ();
                    break;

                case AccessTime:
                    return cache->object (m_url.toString ())->lastRead ();
                    break;

                }
        }
    else
        return QAbstractFileEngine::fileTime ( time );
}

qint64 cs8FileEngine::size() const
{
    qDebug() << "cs8FileEngine::size(): " << m_url.toString();
    if ( m_cached )
        {
            qDebug() << "returning cached size information: " << m_buffer.size();
            return m_buffer.size();//QAbstractFileEngine::size();
        }
    else
        {
            qDebug() << "querying size information: ";
            primeFtp();
            m_ftp->size ( m_url.path() );
            bool ok=waitFtp();
            if ( ok )
                return m_ftp->fileSize();
            else
                return -1;
        }
}

QString cs8FileEngine::fileName ( FileName file ) const
{
    //qDebug() << "cs8FileEngine::fileName (): " << file;
    QString ret;
    switch ( file )
        {
        case QAbstractFileEngine::BaseName:
            ret= QFileInfo ( m_url.path() ).fileName();
            break;

        case QAbstractFileEngine::PathName:
            ret= QFileInfo ( m_url.path() ).absolutePath();
            break;

        case QAbstractFileEngine::AbsoluteName:
            ret= m_url.path();
            break;

        case QAbstractFileEngine::AbsolutePathName:
            ret= QFileInfo ( m_url.path() ).absolutePath();
            break;

        case QAbstractFileEngine::LinkName:
            ret= QString();
            break;

        default:
            ret= m_url.toString();
            break;
        }
    //qDebug() << "           " << ret;
    return ret;
}

QStringList cs8FileEngine::entryList ( QDir::Filters filters, const QStringList& filterNames ) // const
{
    //	m_urlInfoList.clear();
    m_entryList.clear();
    m_filters=filters;
    m_filterNames=filterNames;
    m_entryListPath=m_url.toString();
    QStringList entries;

    //qDebug() << "cs8FileEngine::entryList() " << fileName();
    //qDebug() << "				" << m_url.userName();
    //qDebug() << "				" << m_url.password() << m_url.password().isEmpty();
    //qDebug() << "				" << m_url.path();
    //qDebug() << "				" << filters;

    if ( !m_url.path().isEmpty() )
        {
            connect ( m_ftp->ftp(),SIGNAL ( listInfo ( const QUrlInfo& ) ),this,SLOT ( slotListInfo ( const QUrlInfo& ) ) );
            primeFtp();
            m_ftp->ftp()->list ( m_url.path() );
            waitFtp();
            disconnect ( m_ftp->ftp(),SIGNAL ( listInfo ( const QUrlInfo& ) ) );
            entries= m_entryList;
        }
    else
        {
            entries= QStringList()  << "sys" << "usr" << "log";
        }
    qDebug() << entries;
    return entries;
}

QString cs8FileEngine::owner ( FileOwner arg1 ) const
{
    return QAbstractFileEngine::owner ( arg1 );
}

uint cs8FileEngine::ownerId ( FileOwner arg1 ) const
{
    return QAbstractFileEngine::ownerId ( arg1 );
}

void cs8FileEngine::setFileName ( const QString& file )
{
    QUrl newUrl=m_url;
    //newUrl.setPath ( file );
    ///@todo
    newUrl.setUrl ( file );
    m_url=newUrl;
    //qDebug() << "cs8FileEngine::setFileName()" << file;
    //qDebug() << "                  " << m_url.toString();
}

bool cs8FileEngine::open ( QIODevice::OpenMode mode )
{
    //qDebug() << "cs8FileEngine::open() " << fileName();
    //qDebug() << "				" << m_url.userName();
    //qDebug() << "				" << m_url.password();
    //qDebug() << "				" << m_url.path();

    m_mode = mode;
    m_cached=false;


    switch ( mode & ( QIODevice::WriteOnly|QIODevice::Append ) )
        {
        case QIODevice::ReadOnly:
            break;

        case QIODevice::ReadWrite:
            break;

        case QIODevice::Append:
            break;

            // download of file is not needed if opened with WRITEONLY flag
        case QIODevice::WriteOnly:
            m_cached = true;
            m_buffer.open ( m_mode | QIODevice::ReadOnly );
            m_buffer.seek ( 0 );
            break;

        }

    m_buffer.buffer().clear();
    int size_=size();
    bool status= ( mode & QIODevice::WriteOnly ) || ( size_ >=0 ) ;
    waitFtp();
    setError ( ( QFile::FileError ) m_ftp->error(), m_ftp->errorString() );

    return status;
}


qint64 cs8FileEngine::pos () const
{
    return m_buffer.pos();
}

qint64 cs8FileEngine::read ( char * data, qint64 maxlen )
{
    if ( !retrieveFile() )
        return 0;
    return m_buffer.read ( data,maxlen );
}

qint64 cs8FileEngine::readLine ( char * data, qint64 maxlen )
{
    if ( !retrieveFile() )
        return 0;
    return m_buffer.readLine ( data,maxlen );
}

bool cs8FileEngine::close()
{
    bool ok=true;
    if ( ( m_mode & QIODevice::WriteOnly ) | ( m_mode & QIODevice::Append ) )
        ok=sendFile();
    m_buffer.close();
    m_cached=false;
    return ok;
}

/*!
    \fn cs8FileEngine::slotListInfo(const QUrlInfo & i)
 */
void cs8FileEngine::slotListInfo ( const QUrlInfo & i )
{
    if ( !m_filterNames.isEmpty() ) {}
    m_entryList.append ( i.name() );
    m_ftp->addCachedInfo ( m_entryListPath+"/"+i.name(),i );
}

cs8FileEngine* cs8FileEngine::create ( const QString & fileName )
{
    return fileName.startsWith ( "cs8:/" ) ? new cs8FileEngine ( fileName ) : 0;
}


QAbstractFileEngine::Iterator* cs8FileEngine::beginEntryList ( QDir::Filters filters, const QStringList & filterNames )
{
    return new cs8FileEngineIterator ( filters, filterNames,m_url );
}


void cs8FileEngine::primeFtp() const
{
    //qDebug() << "cs8FileEngine::primeFtp() " << m_url.toString();
    //qDebug() << "                   state: " << m_ftp->ftp()->state();
    if ( m_url.host() !=m_ftp->host() || m_ftp->ftp()->state() ==QFtp::Unconnected )
        {
            if ( m_ftp->ftp()->state() >= QFtp::Connected )
                {
                    m_ftp->ftp()->close();
                    waitFtp();
                }
            m_ftp->connectToHost ( m_url.host() );
            m_ftp->login ( m_url.userName(),m_url.password().isEmpty() ? QString ( "" ) :m_url.password() );
        }
}



/*!
    \fn cs8FileEngine::waitFtp()
 */
bool cs8FileEngine::waitFtp() const
{
    //qDebug() << "waitFtp()";

    m_ftp->waitFtp();

    QString errString = m_ftp->errorString();
    //qDebug() << "cs8FileEngine::waitFtp() " << m_ftp->error() << m_ftp->errorString();
    ///TODO const problem
    //setError(QFile::ResourceError, errString);

    bool status=m_ftp->error() ==QFtp::NoError && !m_ftp->aborted();
    //m_ftp->close();
    return status;
}

bool cs8FileEngine::seek ( quint64 offset )
{
    return m_buffer.seek ( offset );
}


qint64 cs8FileEngine::write ( const char * data, qint64 len )
{
    qDebug() << "cs8FileEngine::write ()";
    if ( !retrieveFile() )
        return 0;
    qint64 size=m_buffer.write ( data,len );
    qDebug() << "cs8FileEngine::write () wrote " << size << "bytes";
    return size;
}


bool cs8FileEngine::retrieveFile()
{
    if ( !m_cached )
        {
            m_buffer.buffer().clear();
            m_buffer.open ( QIODevice::ReadWrite );
            m_buffer.seek ( 0 );
            primeFtp();
            m_ftp->ftp()->get ( m_url.path(),&m_buffer );
            bool status=waitFtp();
            m_buffer.close();
            if ( status )
                {
                    m_cached=true;
                    m_buffer.open ( m_mode | QIODevice::ReadOnly );
                    m_buffer.seek ( 0 );
                }
            return status;
        }
    else
        return true;
}


bool cs8FileEngine::sendFile()
{
    m_buffer.seek ( 0 );
    primeFtp();
    qDebug() << "sending buffer: " << m_buffer.buffer();
    m_ftp->ftp()->put ( &m_buffer,m_url.path() );
    return waitFtp();
}



QUrl cs8FileEngine::url() const
{
    return m_url;
}
