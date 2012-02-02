//
// C++ Implementation:
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "cs8wagonodewidget.h"
#include "cs8wagonode.h"

#include <QDebug>
#include <QMessageBox>
#include <QSignalMapper>

cs8WagoNodeWidget::cs8WagoNodeWidget ( QWidget* parent, Qt::WFlags fl )
    : QWidget ( parent, fl ), Ui::cs8WagoNodeWidgetBase()
{
  setupUi ( this );
  m_node=0;
  m_mapper= new QSignalMapper ( this );
  disableControls();
  connect ( leAddress,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leAddress,"leAddress" );
  connect ( leCtrlAddress,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leCtrlAddress,"leCtrlAddress" );
  connect ( leSubnet,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leSubnet,"leSubnet" );
  connect ( leGateway,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leGateway,"leGateway" );
  
  connect ( leWriteWordAddress,SIGNAL ( valueChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leWriteWordAddress,"leWriteWordAddress" );
  connect ( leReadWordAddress,SIGNAL ( valueChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leReadWordAddress,"leReadWordAddress" );
  connect ( leWriteBitAddress,SIGNAL ( valueChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leWriteBitAddress,"leWriteBitAddress" );
  connect ( leReadBitAddress,SIGNAL ( valueChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leReadBitAddress,"leReadBitAddress" );
  
  connect ( leConfigurationByte,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leConfigurationByte,"leConfigurationByte" );
  connect ( cbProtocol,SIGNAL ( currentIndexChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( cbProtocol,"cbProtocol" );
  connect ( cbBootPRequest,SIGNAL (
              stateChanged ( int ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( cbBootPRequest,"cbBootPRequest" );
  connect ( leTimeout,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leTimeout,"leTimeout" );
  connect ( leScanRate,SIGNAL ( textEdited ( QString ) ),m_mapper, SLOT ( map() ) );
  m_mapper->setMapping ( leScanRate,"leScanRate" );
  
  connect ( m_mapper, SIGNAL ( mapped ( const QString & ) ),
            this, SLOT ( slotChanged ( const QString & ) ) );
}

cs8WagoNodeWidget::~cs8WagoNodeWidget()
{}

/*$SPECIALIZATION$*/




/*!
    \fn cs8WagoNodeWidget::attach(cs8WagoNode* node)
 */
void cs8WagoNodeWidget::attach ( cs8WagoNode* node )
{
  m_node=node;
  connect ( m_node,SIGNAL ( refreshComplete ( bool ) ),this, SLOT ( update() ) );
  connect ( m_node,SIGNAL ( refreshComplete ( bool ) ),this, SLOT ( slotRefreshComplete ( bool ) ) );
//   dumpObjectInfo();
  //update();
}


/*!
    \fn cs8WagoNodeWidget::update()
 */
void cs8WagoNodeWidget::update()
{
  if ( m_node==0 )
    return;
    
  leAddress->setText ( m_node->address() );
  leCtrlAddress->setText ( m_node->ctrlAddress() );
  leSubnet->setText ( m_node->subnet() );
  leGateway->setText ( m_node->gateway() );
  
  leWriteWordAddress->setValue ( m_node->writeWordAddress() );
  leReadWordAddress->setValue ( m_node->readWordAddress() );
  leWriteBitAddress->setValue ( m_node->writeBitAddress() );
  leReadBitAddress->setValue ( m_node->readBitAddress() );
  
  leConfigurationByte->setText ( QString ( "%1" ).arg ( m_node->configurationByte() ) );
  cbProtocol->setCurrentIndex ( ( m_node->protocolTCP() ?0:1 ) );
  cbBootPRequest->setCheckState ( ( m_node->enableBootPRequest() ?Qt::Checked:Qt::Unchecked ) );
  leTimeout->setText ( QString ( "%1" ).arg ( m_node->timeout() ) );
  leScanRate->setText ( QString ( "%1" ).arg ( m_node->scanRate() ) );
}


/*!
    \fn cs8WagoNodeWidget::slotChanged()
 */
void cs8WagoNodeWidget::slotChanged ( const QString & source )
{
  if ( m_node==0 )
    return;
 //   qDebug() << "signal source " << source;
  if ( source=="leAddress" )
    m_node->setAddress ( leAddress->text() );
  if ( source=="leCtrlAddress" )
    m_node->setCtrlAddress ( leCtrlAddress->text() );
  if ( source=="leSubnet" )
    m_node->setSubnet ( leSubnet->text() );
  if ( source=="leGateway" )
    m_node->setGateway ( leGateway->text() );
    
  if ( source=="leWriteWordAddress" )
    m_node->setWriteWordAddress ( leWriteWordAddress->text().toUInt() );
  if ( source=="leReadWordAddress" )
    m_node->setReadWordAddress ( leReadWordAddress->text().toUInt() );
  if ( source=="leWriteBitAddress" )
    m_node->setWriteBitAddress ( leWriteBitAddress->text().toUInt() );
  if ( source=="leReadBitAddress" )
    m_node->setReadBitAddress ( leReadBitAddress->text().toUInt() );
    
  if ( source=="leConfigurationByte" )
    m_node->setConfigurationByte ( leConfigurationByte->text().toUInt() );
  if ( source=="cbProtocol" )
    m_node->setProtocolTCP ( cbProtocol->currentIndex() ==0?true:false );
  if ( source=="cbBootPRequest" )
    m_node->setEnableBootPRequest ( cbBootPRequest->checkState() ==Qt::Checked?true:false );
  if ( source=="leTimeout" )
    m_node->setTimeout ( leTimeout->text().toUInt() );
  if ( source=="leScanRate" )
    m_node->setScanRate ( leScanRate->text().toUInt() );
  qDebug() << "data updated";
}



void cs8WagoNodeWidget::slotRefreshComplete ( bool error )
{
  if ( error ) {
    //QMessageBox::critical ( this,tr ( "Error" ),tr ( "Uploading the data from %1 failed" ).arg ( m_node->address() ) );
    disableControls();
  } else
    enableControls();
}


void cs8WagoNodeWidget::disableControls()
{
  enableControls ( false );
}


void cs8WagoNodeWidget::enableControls ( bool enable )
{

//leAddress->setEnabled(enable);
  leCtrlAddress->setEnabled ( enable );
  leSubnet->setEnabled ( enable );
  leGateway->setEnabled ( enable );
  
  leWriteWordAddress->setEnabled ( enable );
  leReadWordAddress->setEnabled ( enable );
  leWriteBitAddress->setEnabled ( enable );
  leReadBitAddress->setEnabled ( enable );
  
  leConfigurationByte->setEnabled ( enable );
  cbProtocol->setEnabled ( enable );
  cbBootPRequest->setEnabled ( enable );
  leTimeout->setEnabled ( enable );
  leScanRate->setEnabled ( enable );
}
