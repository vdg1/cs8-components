#include <QApplication>
#include <QDir>
#include <QDebug>
#include "cs8application.h"


//

void printUsage()
{
    fprintf( stderr, "Usage:\n"
             "    \n" );

}

int main( int argc, char **argv )
{

    QCoreApplication app( argc, argv );


    QString sourceDir;
    QString destDir;
    bool sourceFlag = false;
    bool destFlag = false;

    QStringList arguments = app.arguments();
    //qDebug() << arguments;

    foreach( QString argument, arguments )
    {
        if ( !sourceFlag && !destFlag )
        {
            if ( argument == "-help" )
            {
                printUsage();
                return 0;
            }
            else if ( argument == "-version" )
            {
                fprintf( stderr, "extractDoc version %s\n",
                         QT_VERSION_STR );
                return 0;
            }
            else if ( argument == "-source" )
            {
                sourceFlag = true;
            }
            else if ( argument == "-dest" )
            {
                destFlag = true;
            }
        }
        else
        {
            if ( sourceFlag && !argument.startsWith( "-" ) )
            {
                sourceDir = argument;
                sourceFlag = false;
            }
            if ( destFlag && !argument.startsWith( "-" ) )
            {
                destDir = argument;
                destFlag = false;
            }
        }
    }
    if ( destDir.isEmpty() || sourceDir.isEmpty() )
    {
        printUsage();
        return 0;
    }

    qDebug() << "source: " << sourceDir;
    qDebug() << "dest:   " << destDir;

    QDir dir( sourceDir );
    cs8Application application;
    QStringList sourceFiles = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    sourceFiles.append( "" );
    QStringList addFiles = dir.entryList( QStringList() << "*.pjx", QDir::Files );
    if ( addFiles.count() > 0 )
    {
        QString addFile = addFiles.at( 0 );
        addFile.chop( 4 );
        sourceFiles.append( addFile );
    }
    qDebug() << ( "applications: " + sourceFiles.join( ", " ) );
    foreach( QString sourceApp, sourceFiles )
    {
        qDebug() << sourceDir + "/" + sourceApp;
        if ( application.openFromPathName( sourceDir + "/" + sourceApp ) )
        {
            qDebug() << ( sourceApp + " open ok" );
            application.exportToCFile( destDir );

        }
        qDebug() << "Done";

    }
    return 0;
}
