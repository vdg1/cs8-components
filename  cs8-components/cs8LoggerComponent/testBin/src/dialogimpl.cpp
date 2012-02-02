#include "dialogimpl.h"
#include "../../lib/src/cs8loggerwidget.h"

#include <QTimer>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QSettings>
//
DialogImpl::DialogImpl ( QWidget * parent, Qt::WFlags f )
    : QMainWindow ( parent, f )
{
  setupUi ( this );
   
  QCoreApplication::setOrganizationName ( "SwissSystem" );
  QCoreApplication::setApplicationName ( "cs8Logger" );
  
  
  m_loggerWidget=new cs8LoggerWidget ( this );
  m_plotWidget= new cs8PlotStatisticWidget ( this );
  m_statisticWidget=new cs8LogStatisticsWidget ( this );
  m_mdiArea=new QMdiArea ( this );
  this->setCentralWidget ( m_mdiArea );
  
  m_mdiArea->addSubWindow ( m_loggerWidget );
  m_mdiArea->addSubWindow ( m_plotWidget );
  m_mdiArea->addSubWindow ( m_statisticWidget );
  
  /*m_plotWindow=new QMdiSubWindow ( m_mdiArea );
  m_plotWindow->setWidget ( m_plotWidget );
  m_plotWindow->showMaximized();
  m_statisticWindow=new QMdiSubWindow ( m_mdiArea );
  m_statisticWindow->setWidget ( m_statisticWidget );
  m_statisticWindow->showMaximized();
  m_logWindow=new QMdiSubWindow ( m_mdiArea );
  m_logWindow->setWidget ( m_loggerWidget );
  m_logWindow->showMaximized();
  */
  //m_mdiArea->cascadeSubWindows();
  setupConnections();
  QTimer::singleShot(0,this,SLOT(restoreSettings()));
}
//

void DialogImpl::slotConnect()
{
  m_loggerWidget->connectToCS8 ( cbAddress->currentText() );
}



/*!
    \fn DialogImpl::slotLoad()
 */
void DialogImpl::slotLoad()
{
  m_loggerWidget->open ( "errors.log" );
}

void DialogImpl::slotOpen()
{
  QString fileName = QFileDialog::getOpenFileName ( this,
                     tr ( "Open Log" ), m_fileName, tr ( "Log Files (*.log)" ) );
  if ( !fileName.isEmpty() ) {
  	m_fileName=fileName;
    m_loggerWidget->open ( fileName );
    m_plotWidget->clear();
  }
}


void DialogImpl::slotClear()
{
  m_loggerWidget->clear();
  m_plotWidget->clear();
}

void DialogImpl::slotConnected()
{
  statusBar()->showMessage ( tr ( "Connected to %1" ).arg ( cbAddress->currentText() ) );
  for ( int i=0;i<cbAddress->count();i++ )
    if ( cbAddress->itemText ( i ) ==cbAddress->currentText() )
      return;
  cbAddress->insertItem ( 0,cbAddress->currentText() );
}

void DialogImpl::slotDisconnected()
{
  statusBar()->showMessage ( tr ( "Disconnected" ) );
}

void DialogImpl::slotAttributeChanged ( cs8LoggerWidget::Attribute attribute, const QString & value )
{
  switch ( attribute ) {
    case cs8LoggerWidget::ConfigurationVersion:
      leConfigurationVersion->setText ( value );
      break;
      
    case cs8LoggerWidget::MachineNumber:
      leMachineNumber->setText ( value );
      break;
      
    case cs8LoggerWidget::ArmNumber:
      leArmSerialNumber->setText ( value );
      break;
      
    case cs8LoggerWidget::PowerHourCounter:
      lePowerHour->setText ( value );
      break;
      
    case cs8LoggerWidget::Val3Version:
      leVal3Version->setText ( value );
      break;
  }
}


void DialogImpl::setupConnections()
{
  connect ( m_loggerWidget,SIGNAL ( attributeChanged ( cs8LoggerWidget::Attribute,const QString & ) ),
            this,SLOT ( slotAttributeChanged ( cs8LoggerWidget::Attribute,const QString & ) ) );
  connect ( m_loggerWidget,SIGNAL ( connected ( ) ),
            this,SLOT ( slotConnected() ) );
  connect ( m_loggerWidget,SIGNAL ( disconnected ( ) ),
            this,SLOT ( slotDisconnected() ) );
  connect ( btConnect,SIGNAL ( clicked() ),
            this,SLOT ( slotConnect() ) );
  connect ( action_Open_Log,SIGNAL ( triggered() ),
            this,SLOT ( slotOpen() ) );
  connect ( action_Clear_Log,SIGNAL ( triggered() ),
            this,SLOT ( slotClear() ) );
  connect ( actionLog_Messages,SIGNAL ( triggered() ),
            this,SLOT ( slotMessagesWindow() ) );
  connect ( actionLog_Statistic,SIGNAL ( triggered() ),
            this,SLOT ( slotStatisticWindow() ) );
            
 //m_statisticWidget->setLog ( m_loggerWidget );
  m_plotWidget->setLog ( m_loggerWidget );
}


void DialogImpl::slotMessagesWindow()
{
  if ( !m_loggerWidget->isVisible() )
    m_loggerWidget->show();
  //m_mdiArea->setActiveSubWindow(m_loggerWidget);
}


void DialogImpl::slotStatisticWindow()
{
  // TODO
}


void DialogImpl::slotPlotWindow()
{
  // TODO
}


void DialogImpl::saveSettings()
{
  QSettings settings;
  
  settings.beginGroup ( "MainWindow" );
  settings.setValue ( "geometry", saveGeometry() );
  settings.endGroup();
  settings.beginGroup ( "SubWindows" );
  settings.setValue ( "logger/geometry", m_loggerWidget->saveGeometry() );
  settings.setValue ( "plot/geometry", m_plotWidget->saveGeometry() );
  settings.setValue ( "statistic/geometry", m_statisticWidget->saveGeometry() );
  settings.endGroup();
  settings.setValue ("file",m_fileName);
}


void DialogImpl::restoreSettings()
{
  QSettings settings;
  
  settings.beginGroup ( "MainWindow" );
  restoreGeometry ( settings.value ( "geometry" ).toByteArray() );
  settings.endGroup();
  
  settings.beginGroup ( "SubWindows" );
  m_loggerWidget->restoreGeometry ( settings.value ( "logger/geometry" ).toByteArray() );
  m_plotWidget->restoreGeometry ( settings.value ( "plot/geometry" ).toByteArray() );
  m_statisticWidget->restoreGeometry ( settings.value ( "statistic/geometry" ).toByteArray() );
  settings.endGroup();
  m_fileName=settings.value("file").toString();
  //if (!m_fileName.isEmpty())
  //  m_loggerWidget->open ( m_fileName );
}


void DialogImpl::closeEvent ( QCloseEvent *event )
{
  saveSettings();
}

