// place your code here
#include <QFtp>
#include <QThread>
#include <QDebug>

QFtp* ftpEngine()
{
    //qDebug() << "ftpEngine pool " << QThread::currentThreadId();
    static QFtp* p_ftpEngine=0;
    if (p_ftpEngine==0)
        p_ftpEngine=new QFtp();

    return p_ftpEngine;
}
