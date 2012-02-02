#include "cs8controller.h"
#include "cs8fileengine.h"
#include <QFtp>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QDirIterator>
#include <QTcpSocket>

//
cs8Controller::cs8Controller( )
    : QObject()
    {

        QUrl url;
        setLoginData ( "default" );
        m_url.setHost ( "" );
        m_url.setScheme ( "cs8" );
        m_url.setPath ( "/log/errors.log" );
        //QFile file(m_url.toString());
        m_lastError=tr("No Error");
        m_ftp = new QFtp(this);
        connect(m_ftp,SIGNAL(commandFinished(int,bool)),this,SLOT(slotCommandFinished(int,bool)));
        m_onlineTimer = new QTimer(this);
        m_onlineTimer->setSingleShot(true);
        connect(m_onlineTimer,SIGNAL(timeout()),this,SLOT(slotOnlineTimerTimeout()));
        m_isOnline=false;
        m_checkOnline=false;
    }
//

void cs8Controller::slotOnlineTimerTimeout(){
        qDebug() << "cs8Controller::slotOnlineTimerTimeout():  state: " << m_ftp->state() << " host: " << m_url.host();
        if (m_ftp->state()==QFtp::Unconnected){
            qDebug() << "       connecting to " << m_url.host() << m_url.userName() << m_url.password();
            m_ftp->connectToHost(m_url.host());
            m_ftp->login(m_url.userName(),m_url.password());
            m_onlineTimer->start(2000);
        }
        else{
            if (m_ftp->state()==QFtp::LoggedIn){
                m_ftp->close();
                if (m_checkOnline)
                    m_onlineTimer->start(5000);
            }
            else{
                m_ftp->close();
            }
        }
    }

void cs8Controller::setLoginData ( const QString & userName, const QString & password )
    {
        m_url.setUserName ( userName );
        m_url.setPassword ( password );
    }

void cs8Controller::setAddress ( const QString & value )
    {
        m_url.setHost ( value ) ;
        m_isOnline=false;
        emit isOnline(m_isOnline, 0, QString());

        if (m_checkOnline)
            slotStartCheck();
    }

void cs8Controller::enableOnlineCheck(bool enable_)
    {
        m_checkOnline=enable_;
        if (m_checkOnline)
            slotStartCheck();
        else
            slotStopCheck();
    }

void cs8Controller::slotStartCheck(){
        m_onlineTimer->stop();
        m_onlineTimer->start(5000);

        // create new instance of QFTP as there seems to be a problem
        // when logging in multiple times using the same instance of QFtp

        m_ftp->abort();
        m_ftp->deleteLater();
        m_ftp = new QFtp(this);
        connect(m_ftp,SIGNAL(commandFinished(int,bool)),this,SLOT(slotCommandFinished(int,bool)));
    }
void cs8Controller::slotStopCheck(){
        m_onlineTimer->stop();
        m_ftp->abort();
    }


void cs8Controller::slotCommandFinished(int id, bool error){
        qDebug() << "cs8Controller::slotCommandFinished(): cmd finished: " << id << error << m_ftp->state();

        m_onlineTimer->stop();
        if (error){
            qDebug() << m_ftp->error() << m_ftp->errorString();

            m_onlineTimer->start(5000);
            m_isOnline=false;
            emit isOnline(m_isOnline, m_ftp->error(), m_ftp->errorString());
            qDebug() << m_url.host() << "is offline";
            m_ftp->disconnect();
            if (m_checkOnline)
                slotStartCheck();
            return;
        }
        if (m_ftp->state()==QFtp::LoggedIn)
        {
            m_isOnline=true;
            emit isOnline(m_isOnline, m_ftp->error(), m_ftp->errorString());
            qDebug() << m_url.host() << "is online";
            m_ftp->disconnect();
            if (m_checkOnline)
                slotStartCheck();
            //m_onlineTimer->start(5000);
            return;
        }

        if (m_ftp->state()==QFtp::Unconnected){
            m_onlineTimer->start(5000);
            return;
        }

    }
/*!
    \fn cs8Controller::logFile()
 */
bool cs8Controller::logFile ( QString & text )
    {

        m_url.setPath ( "/log/errors.log" );
        QFile file ( m_url.toString() );
        if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            m_lastError=file.errorString();
            return false;
        }
        QTextStream stream(&file);
        text=stream.readAll();
        file.close();
        return true;
    }

bool cs8Controller::armType(QString & armType)
    {
        QDomDocument doc;
        QFile file;
        QString docErrString;
        int docErrLine, docErrCol;

        m_url.setPath("/sys/configs/options.cfx");
        file.setFileName(m_url.toString());
        if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            m_lastError=file.errorString();
            return false;
        }

        if (!doc.setContent(&file,&docErrString,&docErrLine,&docErrCol))
        {
            file.close();
            m_lastError=docErrString;
            return false;
        }
        file.close();

        QDomElement element=doc.documentElement();
        for (int i = 0; i < element.elementsByTagName ( "String" ).count(); i++)
        {
            QDomNode node = element.elementsByTagName ( "String" ).at(i);
            if (node.toElement().attribute("name") == "arm")
            {
                // check if arm type value is not an empty string
                armType=node.toElement().attribute("value");
                if (!armType.isEmpty())
                {
                    // extract arm type from tuning identifier
                    QRegExp rx;
                    rx.setPattern("(.*)(-\\*)");
                    if (rx.indexIn(armType)>-1)
                        armType=rx.cap(1);
                    return true;
                }
            }
        }
        return false;
    }


/*!
  \fn cs8Controller::serialNumber()
  Retrieve serial number of controller. If strategy is 0, first /sys/config/options.cfx is checked, then /usr/configs/arm.cfx. If
  strategy is 1 only /sys/config/options.cfx is checked.
  */
bool cs8Controller::serialNumber(QString & number, cs8::serialNumberType source){

        QDomDocument doc;
        QString docErrString;
        QFile file;
        int docErrLine,docErrCol;
        number="";
        bool ok=true;

        if (source==cs8::Any || source==cs8::MachineSerialNumber){
            m_url.setPath("/sys/configs/options.cfx");
            file.setFileName(m_url.toString());
            if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
            {
                m_lastError=file.errorString();
                if (source==cs8::MachineSerialNumber)
                    ok=false;
            }

            if (!doc.setContent(&file,&docErrString,&docErrLine,&docErrCol))
            {
                file.close();
                m_lastError=docErrString;
                if (source==cs8::MachineSerialNumber)
                    ok=false;
            }
            if (ok)
            {
                // reading /sys/config/options.cfx succeeded
                number=orderNumber(doc);
                file.close();
                return true;
            }
            else{
                // read controller serial number from /usr/configs/controller.cfx
                // as of version s7
                m_url.setPath("/usr/configs/controller.cfx");
                file.setFileName(m_url.toString());
                if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
                {
                    m_lastError=file.errorString();
                    if (source==cs8::MachineSerialNumber)
                        return false;
                }

                if (!doc.setContent(&file,&docErrString,&docErrLine,&docErrCol))
                {
                    file.close();
                    m_lastError=docErrString;
                    if (source==cs8::MachineSerialNumber)
                        return false;
                }
                else
                {
                    // reading /sys/config/options.cfx succeeded
                    number=orderNumber(doc);
                    file.close();
                    return true;
                }
            }
        }
        if (source==cs8::Any || source==cs8::ArmSerialNumber){
            m_url.setPath("/usr/configs/arm.cfx");
            file.setFileName(m_url.toString());
            if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
            {
                m_lastError=file.errorString();
                return false;
            }

            if (!doc.setContent(&file,&docErrString,&docErrLine,&docErrCol))
            {
                file.close();
                m_lastError=docErrString;
                return false;
            }
            else
            {
                // reading /sys/config/options.cfx succeeded
                number=orderNumber(doc);
                file.close();
                return true;
            }
        }
        return false;
    }

QString cs8Controller::orderNumber(const QDomDocument & doc)
    {
        QDomElement element=doc.documentElement();
        for (int i = 0; i < element.elementsByTagName ( "String" ).count(); i++)
        {
            QDomNode node = element.elementsByTagName ( "String" ).at(i);
            if (node.toElement().attribute("name") == "orderNumber" || node.toElement().attribute("name") == "serialNumber")
            {
                // check if orderNumber value is not an empty string
                QString orderNumber=node.toElement().attribute("value");
                if (!orderNumber.isEmpty())
                {
                    return orderNumber.replace("/","_");
                }
            }
        }
        // return empty string in case order number could not be determined
        //return ui.ftpServerLineEdit->lineEdit()->text();
        return QString();
    }

void cs8Controller::abort()
    {
    }



/*!
    \fn cs8Controller::controllerSerialNumber()
 */
QString cs8Controller::controllerSerialNumber()
    {
        QString number;
        serialNumber (number, cs8::MachineSerialNumber );
        return number;
    }


/*!
    \fn cs8Controller::armSerialNumber()
 */
QString cs8Controller::armSerialNumber()
    {
        QString number;
        serialNumber (number, cs8::ArmSerialNumber );
        return number;
    }


/*!
    \fn cs8Controller::configFileItem(const QDomDocument & configFile, const QString & type, const QString & name, const QString & default=QString())
 */
QString cs8Controller::configFileItem ( const QDomDocument & configFile_, const QString & type_, const QString & name_, const QString & default_ )
    {
        QDomElement element=configFile_.documentElement();
        for ( int i = 0; i < element.elementsByTagName ( type_ ).count(); i++ )
        {
            QDomNode node = element.elementsByTagName ( type_ ).at ( i );
            if ( node.toElement().attribute ( "name" ) == name_ )
                return node.toElement().attribute ( "value" );
        }
        return QString();
    }



/*!
    \fn cs8Controller::serialNumber(cs8::serialNumberType type)
 */
/*
QString cs8Controller::serialNumber ( cs8::serialNumberType type )
{
    if ( type==cs8::MachineSerialNumber )
        m_url.setPath ( "sys/configs/options.cfx" );
    else
        m_url.setPath ( "usr/configs/arm.cfx" );
    QFile file ( m_url.toString() );
    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        m_lastError=file.errorString();
        return false;
    }
    QDomDocument doc;
    QTextStream in(&file);
    QString document=in.readAll();
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !doc.setContent ( document, &errorMsg, &errorLine,&errorColumn ) )
    {
        m_lastError=QString("%1:%2 %3,%4").arg(m_url.toString()).arg(errorMsg).arg(errorLine).arg(errorColumn);
        qWarning() << m_lastError;
        qDebug() << document;
        return false;
    }
    file.close();
    return configFileItem ( doc,"String","orderNumber" ).replace ( "/","_" );
}
*/


/*!
    \fn cs8Controller::armType()
 */
QString cs8Controller::armType()
    {
        m_url.setPath ( "usr/configs/arm.cfx" );
        QFile file ( m_url.toString() );
        if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            m_lastError=file.errorString();
            return false;
        }
        QDomDocument doc;
        if ( !doc.setContent ( &file ) )
            return false;
        file.close();
        return configFileItem ( doc,"String","arm" );
    }


/*!
    \fn cs8Controller::lastError() const
 */
QString cs8Controller::lastError() const
    {
        return m_lastError;
    }


bool cs8Controller::isOnline(const QString & address)
    {
        QTcpSocket socket;
        int timeout=100;
        socket.connectToHost(address,21);
        return socket.waitForConnected(timeout);
    }

QString cs8Controller::urlWithoutPath() const
    {
        QUrl url=m_url;
        qDebug() << url.toString(QUrl::RemovePath);
        return url.toString(QUrl::RemovePath);
    }
