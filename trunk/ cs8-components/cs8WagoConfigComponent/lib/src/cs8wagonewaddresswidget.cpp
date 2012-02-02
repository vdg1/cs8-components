#include "cs8wagonewaddresswidget.h"
#include "cs8wagonode.h"

#include <QMessageBox>
#include <QRegExpValidator>
#include <QRegExp>
//
cs8WagoNewAddressWidget::cs8WagoNewAddressWidget ( QWidget * parent, Qt::WFlags f )
    : QDialog ( parent, f )
{
  setupUi ( this );
  m_node=new cs8WagoNode ( this );
  connect ( m_node,SIGNAL ( commitComplete ( bool ) ),this,SLOT ( slotCommited ( bool ) ) );
  QRegExp rx("(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
  
  leNewAddress->setValidator(new QRegExpValidator (rx,0));
  leNewGateway->setValidator(new QRegExpValidator (rx,0));
  leNewSubnet->setValidator(new QRegExpValidator (rx,0));
  leAddress->setValidator(new QRegExpValidator (rx,0));
  
  connect (leNewAddress, SIGNAL(textEdited(const QString & )), this, SLOT(slotChanged()));
  connect (leNewGateway, SIGNAL(textEdited(const QString & )), this, SLOT(slotChanged()));
  connect (leNewSubnet, SIGNAL(textEdited(const QString & )), this, SLOT(slotChanged()));
  connect (leAddress, SIGNAL(textEdited(const QString & )), this, SLOT(slotChanged()));
}
//

void cs8WagoNewAddressWidget::on_buttonBox_accepted()
{
  m_node->setAddress ( leNewAddress->text() );
  m_node->setGateway ( leNewGateway->text() );
  m_node->setSubnet ( leNewSubnet->text() );
  m_node->commit ( leAddress->text() );
  buttonBox->setEnabled ( false );
}

void cs8WagoNewAddressWidget::on_buttonBox_rejected()
{
  reject();
}

/*
    \fn cs8WagoNewAddressWidget::attach(cs8WagoNode* node)
 */
void cs8WagoNewAddressWidget::attach ( cs8WagoNode* node )
{
// m_node=node;
}



/*!
    \fn cs8WagoNewAddressWidget::slotCommited(bool error)
 */
void cs8WagoNewAddressWidget::slotCommited ( bool error )
{
  buttonBox->setEnabled ( true );
  if ( !error )
    accept();
  else
    QMessageBox::warning ( this,
                           tr ( "Communication error" ),
                           tr ( "Configuration of node failed: \n %1" ).arg ( m_node->errorString() ) );
                           
}

void cs8WagoNewAddressWidget::slotChanged()
{
	/*int pos=0;
	QString txt=leNewAddress->text();
	if (leNewAddress->validator()->validate(txt,pos)!=QValidator::Acceptable)
	{
		leNewAddress->setStyleSheet( QString( "background-color: gray")); 
	}
	else
				leNewAddress->setStyleSheet( QString( "background-color: white")); 
*/
}

