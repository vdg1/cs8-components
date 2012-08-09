#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QDir>
#include <cs8application.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (qApp->arguments ().count ()>1)
    {
        MainWindow w;
        QStringList dirs;
        QDir dir;

        QString workingPath=dir.currentPath();

        for(int i=1;i<qApp->arguments ().count ();i++)
        {
            QString dirTxt=qApp->arguments ().at (i);
            qDebug() << "Dir: " <<   workingPath+"/"+dirTxt;
            dir.setPath (dirTxt);
            qDebug() << dir.absolutePath();
            foreach(QString pth,dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs ))
                dirs << dir.absolutePath()+"/"+pth;
        }

        QList<cs8Application *> cs8SourceApps;
        foreach(QString pth,dirs)
        {
            cs8Application *cs8SourceApp=new cs8Application();
            if (cs8SourceApp->openFromPathName (pth))
            {
                cs8SourceApps.append (cs8SourceApp);
            }
            else
            {
                delete cs8SourceApp;
            }
        }

        w.createAPIs(cs8SourceApps);
        return 0;
    }
    else
    {
        a.setApplicationName("Val3 Create API");
        MainWindow w;
        w.show();

        return a.exec();
    }
}
