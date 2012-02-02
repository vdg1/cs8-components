#include "wagoconfig.h"

#include "cs8ModbusComponent.h"
#include "cs8ControllerComponent.h"

#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

WagoConfig::WagoConfig(QWidget *parent) :
	QMainWindow(parent), m_modbusConfigValid( 0) {
	ui.setupUi( this);
	m_progress = new progressDialogBaseImpl ( this );
	QTimer::singleShot( 0, this, SLOT ( getConfig() ));

	connect(ui.action_Open, SIGNAL ( triggered() ), this, SLOT ( slotRemoteOpen() ));
	connect(ui.actionRecommission_Wago_Coupler, SIGNAL ( triggered() ), this, SLOT ( slotRecommission() ));
	connect(ui.action_About, SIGNAL ( triggered() ), this, SLOT ( slotAbout() ));
	connect(ui.action_About_Qt, SIGNAL ( triggered() ), this, SLOT ( slotAboutQt() ));
	connect(ui.action_Backup_Configuration, SIGNAL ( triggered() ), this, SLOT ( slotBackupConfig() ));
	connect(ui.action_Restore_Configuration, SIGNAL ( triggered() ), this, SLOT ( slotRestoreConfig() ));

	m_systemNode=new cs8WagoNode ( this , "system");
	m_systemNode->setAddress("192.168.0.2");
	m_userNode=new cs8WagoNode ( this ,"user");
	m_userNode->setAddress("192.168.0.3");
	ui.SystemNode->attach(m_systemNode);
	ui.UserNode->attach(m_userNode);

	connect(ui.btSystemSave, SIGNAL ( clicked() ), m_systemNode, SLOT ( commit() ));
	connect(ui.btSystemSave, SIGNAL ( clicked() ), this, SLOT ( slotCommit() ));
	connect(ui.btSystemRefresh, SIGNAL ( clicked() ), this, SLOT ( slotRefresh() ));
	connect(ui.btSystemRefresh, SIGNAL ( clicked() ), m_systemNode, SLOT ( refresh() ));
	connect(ui.btSystemProposal, SIGNAL ( clicked() ), this, SLOT ( slotSystemProposal() ));
	connect(m_systemNode, SIGNAL ( commitComplete ( bool ) ), this, SLOT ( slotSystemCommitComplete ( bool ) ));
	connect(m_systemNode, SIGNAL ( refreshComplete ( bool ) ), this, SLOT ( slotSystemRefreshComplete ( bool ) ));

	connect(ui.btUserSave, SIGNAL ( clicked() ), m_userNode, SLOT ( commit() ));
	connect(ui.btUserSave, SIGNAL ( clicked() ), this, SLOT ( slotCommit() ));
	connect(ui.btUserRefresh, SIGNAL ( clicked() ), this, SLOT ( slotRefresh() ));
	connect(ui.btUserRefresh, SIGNAL ( clicked() ), m_userNode, SLOT ( refresh() ));
	connect(ui.btUserProposal, SIGNAL ( clicked() ), this, SLOT ( slotUserProposal() ));
	connect(m_userNode, SIGNAL ( commitComplete ( bool ) ), this, SLOT ( slotUserCommitComplete ( bool ) ));
	connect(m_userNode, SIGNAL ( refreshComplete ( bool ) ), this, SLOT ( slotUserRefreshComplete ( bool ) ));

	m_modbusFile=new cs8ModbusConfigFile ( this );
	ui.modbusView->setModel(m_modbusFile);
	ui.modbusView->setReadOnly( true);
	//addToolBar ( ui.modbusView->toolBar() );
}

WagoConfig::~WagoConfig() {
}

void WagoConfig::getConfig() {
	m_progress->show();
	m_systemNode->refresh();
	m_userNode->refresh();
	m_progress->close();
}

/*!
 \fn WagoConfig::slotSystemCommitComplete(bool error)
 */
void WagoConfig::slotSystemCommitComplete(bool error) {
	qDebug() << "commit complete: " << error;
	if (error)
		QMessageBox::warning(
				this,
				tr("Communication error"),
				tr ( "Commit of system node failed: \n %1" ).arg(m_systemNode->errorString() ) );
	ui.btSystemSave->setEnabled( true);
	ui.btUserSave->setEnabled( true);
	m_progress->close();
}

/*!
 \fn WagoConfig::slotUserCommitComplete(bool error)
 */
void WagoConfig::slotUserCommitComplete(bool error) {
	qDebug() << "commit complete: " << error;
	if (error)
		QMessageBox::warning(
				this,
				tr("Communication error"),
				tr ( "Commit of user node failed: \n %1" ).arg(m_userNode->errorString() ) );
	ui.btUserSave->setEnabled( true);
	ui.btSystemSave->setEnabled( true);
	m_progress->close();
}
void WagoConfig::slotOpen() {
	QString fileName = QFileDialog::getOpenFileName( this,
			tr("Open Modbus File"), m_modbusFile->fileName(),
			tr("Modbus Files (*.xml)") );
	if ( !fileName.isEmpty() ) {
		qDebug() << "opening file: " << fileName;
		m_modbusFile->open(fileName);
	}
}

bool WagoConfig::slotRecommission() {
	cs8WagoNewAddressWidget* dialog=new cs8WagoNewAddressWidget ( this );
	return dialog->exec() ==QDialog::Accepted;
}

void WagoConfig::slotRemoteOpen() { 
	bool ok;
	QString text="192.168.0.5";
	cs8LoginDataDialog dialog(this, "192.168.0.5", "default");
	if (dialog.exec()) {
		m_progress->setText(tr("Retrieving Modbus Configuration from controller") );
		m_progress->show();
		m_modbusConfigValid=m_modbusFile->remoteOpen(dialog.hostName(),
				dialog.userName(), dialog.password());
		m_progress->close();
	}
	return;

	text = QInputDialog::getText( this, tr("Modbus file upload"),
			tr("IP Address of CS8C Controller:"), QLineEdit::Normal, text, &ok);
	if (ok && !text.isEmpty() ) {
		m_progress->setText(tr("Retrieving Modbus Configuration from controller") );
		m_progress->show();
		m_modbusConfigValid=m_modbusFile->remoteOpen(text, "default", "");
		m_progress->close();
	} 
}

void WagoConfig::slotSystemProposal() {
	if ( !m_modbusConfigValid)
		slotRemoteOpen();
	if (m_modbusConfigValid) {
		qDebug() << "System node proposal:";
		if (m_modbusFile->getItemsByName ( "node0_*" ).count() >0) {
			m_systemNode->setReadBitAddress(m_modbusFile->getItemByName ( "node0_01" )->offset() );
			m_systemNode->setWriteBitAddress(m_modbusFile->getItemByName ( "LifeBitSys" )->offset() );

			m_systemNode->setReadWordAddress( 0);
			QString name;
			QList<cs8ModbusItem*> list=m_modbusFile->getItemsByName("node0_*");
			foreach ( cs8ModbusItem* item, list )
			{

				if ( (item->accessType() ==cs8ModbusItem::RW) && (item->type()!=cs8ModbusItem::Bit) )
				{
					name=item->name();
					break;
				}
			}

			m_systemNode->setWriteWordAddress(m_modbusFile->getItemByName ( name )->offset()-2);
		} else if (m_modbusFile->getItemsByName ( "A12*" ).count() >0) {
			m_systemNode->setReadBitAddress(m_modbusFile->getItemByName ( "A1211" )->offset() );
			m_systemNode->setWriteBitAddress(m_modbusFile->getItemByName ( "LifeBitSys" )->offset() );

			m_systemNode->setReadWordAddress( 0);
			QString name;
			QList<cs8ModbusItem*> list=m_modbusFile->getItemsByName("A12*");
			foreach ( cs8ModbusItem* item, list )
			{
				if ( (item->accessType() ==cs8ModbusItem::RW) && (item->type()!=cs8ModbusItem::Bit) )
				{
					name=item->name();
					break;
				}
			}
			qDebug() << "Use " << name << " for write word proposal";
			m_systemNode->setWriteWordAddress(m_modbusFile->getItemByName ( name )->offset()-2);
		} else {
			QMessageBox::warning( this, tr("Configuration Error"),
					tr("The modbus file does not contain the required items") );
			return;
		}
	}
	m_systemNode->setAddress("192.168.0.2");
	m_systemNode->setConfigurationByte( 0);
	m_systemNode->setCtrlAddress("192.168.0.5");
	m_systemNode->setGateway("0.0.0.0");
	m_systemNode->setSubnet("255.255.255.0");
	m_systemNode->setEnableBootPRequest( true);
	m_systemNode->setProtocolTCP( true);

	m_systemNode->setTimeout( 100);
	m_systemNode->setScanRate( 0);

	//m_systemNode->dumpConfigData();
	ui.SystemNode->update();
	ui.SystemNode->enableControls();
}

void WagoConfig::slotUserProposal() {
	if ( !m_modbusConfigValid)
		slotRemoteOpen();
	if (m_modbusConfigValid) {
		if (m_modbusFile->getItemsByName ( "node1_*" ).count() >0) {
			// we need to search the first output module in the modbus list
			QString name;
			QList<cs8ModbusItem*> list=m_modbusFile->getItemsByName("node1_*");
			foreach ( cs8ModbusItem* item, list )
			{
				if ( item->accessType() ==cs8ModbusItem::R )
				{
					name=item->name();
					break;
				}
			}
			m_userNode->setReadBitAddress(m_modbusFile->getItemByName ( name )->offset() );
			m_userNode->setWriteBitAddress(m_modbusFile->getItemByName ( "LifeBitUser" )->offset() );

			m_userNode->setReadWordAddress( 0);
			m_userNode->setWriteWordAddress( 0);
		} else {
			if (m_modbusFile->getItemsByName ( "A22*" ).count() >0) {
				// we need to search the first output module in the modbus list
				QString name;
				QList<cs8ModbusItem*> list=m_modbusFile->getItemsByName("A22*");
				foreach ( cs8ModbusItem* item, list )
				{
					if ( item->accessType() ==cs8ModbusItem::R )
					{
						name=item->name();
						break;
					}
				}
				m_userNode->setReadBitAddress(m_modbusFile->getItemByName ( name )->offset() );
				m_userNode->setWriteBitAddress(m_modbusFile->getItemByName ( "LifeBitUser" )->offset() );

				m_userNode->setReadWordAddress( 0);
				m_userNode->setWriteWordAddress( 0);
			} else {
				QMessageBox::warning(
						this,
						tr("Configuration Error"),
						tr("The modbus file does not contain the required items") );
				return;
			}
		}
		m_userNode->setAddress("192.168.0.3");
		m_userNode->setConfigurationByte( 0);
		m_userNode->setCtrlAddress("192.168.0.5");
		m_userNode->setGateway("0.0.0.0");
		m_userNode->setSubnet("255.255.255.0");
		m_userNode->setEnableBootPRequest( true);
		m_userNode->setProtocolTCP( true);

		m_userNode->setTimeout( 100);
		m_userNode->setScanRate( 0);

		//m_userNode->dumpConfigData();
		ui.UserNode->update();
		ui.UserNode->enableControls();
	}
}

void WagoConfig::slotSystemRefreshComplete(bool error) {
	qDebug() << "WagoConfig::slotSystemRefreshComplete(bool error)";
	if (error) {
		if (m_systemNode->error() !=5) {
			QMessageBox::warning( this, tr("Communication error"), tr ( "Refresh of system coupler failed: \n (%2) %1" )
			.arg ( m_systemNode->errorString() )
			.arg(m_systemNode->error() ) );
		} else {
			if (QMessageBox::question(
					this,
					tr("Communication error"),
					tr("The coupler seems to be not responding. This can happen for two reasons:\
			                                  \n 1. The network configuration of this computer is not correct\
			                                  \n 2. The Wago coupler is not properly configured.\
			                                  \n\n Do you want to try to reconfigure the Wago coupler?"),
					QMessageBox::Yes | QMessageBox::No) ==QMessageBox::Yes)
				if (slotRecommission() )
					m_systemNode->refresh();
		}
	}
	m_progress->close();
}

void WagoConfig::slotUserRefreshComplete(bool error) {
	qDebug() << "WagoConfig::slotUserRefreshComplete(bool error)";
	if (error) {
		if (m_userNode->error() !=5) {
			QMessageBox::warning( this, tr("Communication error"), tr ( "Refresh of user coupler failed: \n (%2) %1" )
			.arg ( m_userNode->errorString() )
			.arg(m_userNode->error() ) );
		} else {
			if (QMessageBox::question(
					this,
					tr("Communication error"),
					tr("The coupler seems to be not responding. This can happen for two reasons:\
			                                  \n 1. The network configuration of this computer is not correct\
			                                  \n 2. The Wago coupler is not properly configurated.\
			                                  \n Do you want to try to reconfigure the Wago coupler?"),
					QMessageBox::Yes | QMessageBox::No) ==QMessageBox::Yes)
				if (slotRecommission() )
					m_userNode->refresh();
		}
	}
	m_progress->close();
}

void WagoConfig::slotCommit() {
	m_progress->setText(tr("Saving changes to coupler") );
	m_progress->show();
	ui.btSystemSave->setEnabled( false);
	ui.btUserSave->setEnabled( false);
}

void WagoConfig::slotAbout() {
	QString version = tr("Version %1");
	QString open = tr(" Open Source Edition");
	version.append(open);
	version = version.arg("1.1");

	QString
			text=
					(tr ( "<center><img src=\":/images/splash.png\"/></img><p>%1</p></center>"
							"<p>Plastic Configuration Tool is a software tool to configure CS8 Modbus Elements</p>"
							"<p>%2</p>"
							"<p>Copyright (C) 2007 Swiss System AB. All rights reserved."
							"</p><p>The program is provided AS IS with NO WARRANTY OF ANY KIND,"
							" INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A"
							" PARTICULAR PURPOSE.</p>" ).arg ( version ).arg("") );

	QMessageBox::about( this, tr("Plastic Config Tool"), text);
}

void WagoConfig::slotAboutQt() {
	QMessageBox::aboutQt( this);
}

/*!
 \fn WagoConfig::slotBackupConfig()
 */
void WagoConfig::slotBackupConfig() {
	QString fileName=QFileDialog::getSaveFileName(this,
			tr("Open Configuration File"), QDir::currentPath(),
			"Config File (*.mcf)");
	if (!fileName.isEmpty()) {
		QSettings settings(fileName, QSettings::IniFormat);
		m_modbusFile->saveConfig(settings);
		m_systemNode->saveConfig(settings);
		m_userNode->saveConfig(settings);
	}
}

/*!
 \fn WagoConfig::slotRestoreConfig
 */
void WagoConfig::slotRestoreConfig() {
	QString fileName=QFileDialog::getOpenFileName(this,
			tr("Open Configuration File"), QDir::currentPath(),
			"Config File (*.mcf)");
	if (!fileName.isEmpty()) {
		QSettings settings(fileName, QSettings::IniFormat);
		m_modbusFile->restoreConfig(settings);
		m_systemNode->restoreConfig(settings);
		m_userNode->restoreConfig(settings);
	}
}

void WagoConfig::slotRefresh() {
	m_progress->setText("Refreshing data...");
	m_progress->show();
}

