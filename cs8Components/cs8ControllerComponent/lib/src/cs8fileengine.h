#ifndef CS8FILEENGINE_H
#define CS8FILEENGINE_H
//
#include <QAbstractFileEngine>
#include <QUrl>
#include <QBuffer>
#include <QList>
#include <QUrlInfo>

//


class P_cs8FTPInstance;

class cs8FileEngine : public QObject, public QAbstractFileEngine
{
    Q_OBJECT
private:
    QIODevice::OpenMode m_mode;
    
    
public:
    qint64 write ( const char * data, qint64 len );
    Iterator* beginEntryList ( QDir::Filters filters, const QStringList & filterNames );
    static cs8FileEngine* create ( const QString & fileName );
    cs8FileEngine ( const QString & file=QString() );
    ~cs8FileEngine();
    
    bool copy ( const QString & newName );
    bool seek ( quint64 offset );
    bool caseSensitive() const;
    bool isRelativePath() const;
    bool mkdir ( const QString& dirName, bool createParentDirectories ) ;
    bool open ( QIODevice::OpenMode mode );
    bool remove();
    bool rename ( const QString& newName );
    bool rmdir ( const QString& dirName, bool recurseParentDirectories ) const;
    bool setPermissions ( uint perms );
    bool setSize ( qint64 size );
    QAbstractFileEngine::FileFlags fileFlags ( FileFlags type ) const;
    QDateTime fileTime ( FileTime time ) const;
    qint64 size() const;
    QString fileName ( FileName file = DefaultName ) const;
    QStringList entryList ( QDir::Filters filters, const QStringList& filterNames ) ;//const;
    QString owner ( FileOwner arg1 ) const;
    uint ownerId ( FileOwner arg1 ) const;
    void setFileName ( const QString& file );
    
    qint64 pos () const;
    qint64 read ( char * data, qint64 maxlen );
    qint64 readLine ( char * data, qint64 maxlen );
    bool close ();

    QUrl url() const;

    
    
    
signals:

public slots:

protected:
    bool sendFile();
    bool retrieveFile();
    void primeFtp() const ;
    bool waitFtp() const;
    
    QUrl m_url;
    P_cs8FTPInstance* m_ftp;
    QBuffer m_buffer;
    //QList<QUrlInfo> m_urlInfoList;
    QStringList m_entryList;
    QStringList m_filterNames;
    QDir::Filters m_filters;
    bool m_cached;
    QString m_entryListPath;
    
protected slots:
    void slotListInfo ( const QUrlInfo & i );
};
#endif
