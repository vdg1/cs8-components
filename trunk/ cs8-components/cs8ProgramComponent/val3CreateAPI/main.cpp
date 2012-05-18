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


        for(int i=1;i<qApp->arguments ().count ();i++)
        {
            dir.setCurrent (qApp->arguments ().at (i));
            foreach(QString pth,dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs ))
                dirs << dir.currentPath ()+"/"+pth;
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
