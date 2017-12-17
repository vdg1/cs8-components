#include "dialogimpl.h"
#include "cs8ControllerComponent.h"

#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QMessageBox>
#include <QDirIterator>
#include <QDirModel>
#include <QFileDialog>
//
DialogImpl::DialogImpl (QWidget * parent, Qt::WindowFlags f )
    : QDialog ( parent, f )
{
    setupUi ( this );
    connect ( btLoad,SIGNAL ( clicked() ),this,SLOT ( slotLoad() ) );
    m_ctrl=new cs8Controller();
    //m_ctrl->enableOnlineCheck(true);
    m_ctrl->setAddress(leHost->text());
    scanner=new cs8NetworkScanner(this);

   // scanner->start(true);
    view->setModel (scanner);
    connect(scanner,SIGNAL(scannerActive(bool)),pushButton,SLOT(setDisabled(bool)));

    connect(m_ctrl,&cs8Controller::onlineChanged,radioButton,&QAbstractButton::setChecked);

    m_telnet = new cs8Telnet(this);
    connect(m_telnet,&cs8Telnet::telnetLine,[=]( const QString &newValue ){textEdit->append(newValue);});
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
    textEdit->setText ( QString("") );
    QFile file ;
    QString text;


    m_ctrl->setAddress(leHost->text());
    text=m_ctrl->armSerialNumber();
    file.close();


    textEdit->setText ( text );

}

void DialogImpl::on_leHost_textChanged(const QString &arg1)
{
    m_ctrl->setAddress(arg1);
}

void DialogImpl::on_pushButton_clicked()
{
    scanner->start();
}

void DialogImpl::on_pushButton_2_clicked()
{
    m_telnet->connectToHost(leHost->text(),23);
}
