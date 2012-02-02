#include "dialogimpl.h"
#include "cs8ControllerComponent.h"


#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QDirModel>
#include <QFileDialog>
//
DialogImpl::DialogImpl ( QWidget * parent, Qt::WFlags f )
    : QDialog ( parent, f )
    {
        setupUi ( this );
        connect ( btLoad,SIGNAL ( clicked() ),this,SLOT ( slotLoad() ) );
        m_ctrl=new cs8Controller();


        QUrl url;
        url.setUrl ( QString ( "cs8://staubli:Herakles@%1/" ).arg ( leHost->text() ) );

        cs8DirModel* model=new cs8DirModel(this);
        model->setLazyChildCount(true);
        model->setRootPath(url.toString());
        treeView->setModel(model);
        treeView->setRootIndex(model->index(url.toString()));

        m_ctrl->setAddress(leHost->text());
        m_ctrl->setLoginData("staubli","Herakles");
        scanner=new cs8NetworkScanner(this);

        scanner->start(false);
        view->setModel (scanner);

        connect(m_ctrl,SIGNAL(isOnline(bool, int)),radioButton,SLOT(setChecked(bool)));
    }
//

void DialogImpl::slotHostListChanged(){
        //listWidgetHosts->clear();
        //listWidgetHosts->addItems(scanner->hostList());
        scanner->stop();
    }

/*!
    \fn DialogImpl::slotLoad()
 */
void DialogImpl::slotLoad()
    {

        QFile file ;
        QString text;
        QUrl url;
        QDir dir;

        dir.setPath( QString ( "cs8://default@%1/sys/configs/options.cfx" ).arg ( leHost->text() ));
        QList<QFileInfo> list=dir.entryInfoList(QDir::AllEntries);


        url.setUrl ( QString ( "cs8://default@%1/sys/configs/options.cfx" ).arg ( leHost->text() ) );
        file.setFileName ( url.toString() );
        if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QMessageBox::warning ( this,
                                   tr ( "Error opening file" ),
                                   tr ( "File %3 could not be opened: (%1) %2" )
                                   .arg ( file.error() )
                                   .arg ( file.errorString() )
                                   .arg ( url.toString() ) );
            return;
        }
        text=file.readAll();
        file.close();

        QFileInfo info(file);
        QDateTime time=info.created ();
        qDebug() << "created on " << info.created ().toString ("yyyyMMdd-hh:mm:ss");
        qDebug() << "modified on " << info.lastModified ().toString ("yyyyMMdd-hh:mm:ss");
        qDebug() << "last read on " << info.lastRead ().toString ("yyyyMMdd-hh:mm:ss");
        textEdit->setText ( text );

        //textEdit->setText(m_ctrl->armType());


        /*
    QDir dir;
    dir.setPath("cs8://default@localhost/usr");
    QList<QFileInfo> list=dir.entryInfoList(QDir::AllEntries);

    text="";
    foreach(QFileInfo info,list)
    {
        text+=info.fileName();
        text+=info.isDir()?" D":" F";
        text+="\n";
    }
    textEdit->setText(text);
*/
        //textEdit->setText(cs8Controller::isOnline("127.0.0.1")?"online":"offline");

        // use convenience class for accessing controller information

        // m_ctrl->setAddress ( leHost->text() );
        // m_ctrl->setLoginData("staubli","Herakles");
        // url=m_ctrl->url();
        // url.setPath("/usr/applicom/io/config/APLDFILE.TA3");//"D:\data\Staubli\CS8\6.2\usr\applicom\io\config\APLDFILE.TA3"

        //bool ok = file.copy(url.toString(),"d:/temp/test.TA3");
        // QString text;
        /*
if ( m_ctrl->serialNumber ( text ) )
                textEdit->setText ( text );
        else
                textEdit->setText ( tr ( "get log failed" ) );
*/
    }
