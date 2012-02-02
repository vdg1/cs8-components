#include "cs8wagonode.h"
#include <QHttp>
#include <QDebug>
#include <QBuffer>
#include <QTcpSocket>
#include <QMessageBox>
//
cs8WagoNode::cs8WagoNode ( QObject * parent, const QString & name )
		: QObject ( parent ), m_address ( 0 ), m_configurationByte ( 0 ),
		m_ctrlAddress ( 0 ), m_subnet ( 0 ),m_gateway ( 0 ), m_protocolTCP ( 0 ), m_readBitAddress ( 0 ), m_writeBitAddress ( 0 ),
		m_readWordAddress ( 0 ), m_writeWordAddress ( 0 ),
		m_scanRate ( 200 ), m_timeout ( 500 ), m_enableBootPRequest ( 1 ), m_lastOperation ( 0 )
{
	m_name=name;
	buffer= new QBuffer ( this );
	tcp = new QTcpSocket ( this );
	http = new QHttp ( "",QHttp::ConnectionModeHttp,8080,this );
	connect ( http,SIGNAL ( requestFinished ( int,bool ) ),
	          this,SLOT ( slotRequestFinished ( int,bool ) ) );
	connect ( http,SIGNAL ( readyRead ( const QHttpResponseHeader & ) ),
	          this,SLOT ( slotReadyRead ( const QHttpResponseHeader & ) ) );
}
//


void cs8WagoNode::commit ( const QString & address )
{
//	 "T1=192.168.0.2&T2=192.168.0.5&T3=0.0.0.0&T4=0.0.0.0&T5=0&T6=0&T7=48&T8=0&T9=500&TA=0&TB=0&R1=on&B1=Submit" );
	configString=QString ( "T1=%1&T2=%2&T3=%3&T4=%4&T5=%5&T6=%6&T7=%7&T8=%8&T9=%9&TA=%10&TB=%11&R1=%12&C1=%13&B1=Submit" )
	             .arg ( m_address )
	             .arg ( m_ctrlAddress )
	             .arg ( m_subnet )
	             .arg ( m_gateway )
	             .arg ( m_readWordAddress )
	             .arg ( m_writeWordAddress )
	             .arg ( m_readBitAddress )
	             .arg ( m_writeBitAddress )
	             .arg ( m_timeout )
	             .arg ( m_scanRate )
	             .arg ( m_configurationByte )
	             .arg ( m_protocolTCP ? "on" : "UDP" )
	             .arg ( m_enableBootPRequest ? "on" : "off" );
	QByteArray data;
	QString adr;
	if ( address.isEmpty() )
		adr=m_address;
	else
		adr=address;
	//QMessageBox::information ( 0,tr ( "" ), QString ( "sending post string(%1): " + configString ).arg ( adr ) );
	data.append ( configString );
	http->setHost ( adr, 8080 );

	idCommit=http->post ( "/",data );
	m_lastOperation=1;
}


bool cs8WagoNode::refresh()
{
	/*
	"<html><head><meta http-equiv="Content-TYPE" content="text/html; charset=windows
	-1252"><meta http-equiv="expires" content="0"><title>WAGO Ethernet controller co
	nfiguration page</title></head><body><p><font color="#006600"><b><font size="+4"
	>WAGO-I/O-System</font></b></font></p><form method="POST"><table border="0" cell
	padding="5" cellspacing="0"><tr><td>WAGO controller IP address</td><td><input TY
	PE="text" name="T1" size="15" value="0.0.0.0"></td></tr><tr><td>ST-UBLI controll
	er IP address</font></td><td><input type="text" name="T2" size="15" value="192.1
	68.0.5"></td></tr><tr><td>Subnet mask</td><td><input type="text" name="T3" size=
	"15" value="0.0.0.0"></td></tr><tr><td>Gateway address</td><td><input type="text
	" name="T4" size="15" value="0.0.0.0"></td></tr><tr><td>Read word address</td><t
	d><input type="text" name="T5" size="5" value="0"></td></tr><tr><td>Write word a
	ddress</td><td><input type="text" name="T6" size="5" value="0"></td></tr><tr><td
	>Read bit address</td><td><input type="text" name="T7" size="5" value="38"></td>
	</tr><tr><td>Write bit address</td><td><input type="text" name="T8" size="5" val
	ue="0"></td></tr><tr><td>Timeout (ms)</td><td><input type="text" name="T9" size=
	"5" value="500"></td></tr><tr><td>Scan rate (ms)</td><td><input type="text" name
	="TA" size="5" value="0"></td></tr><tr><td>Configuration Byte</td><td><input typ
	e="text" name="TB" size="5" value="0"></td></tr><tr><td>Fieldbus protocol</td><t
	d><input type="radio" checked name="R1"> Modbus/TCP</td></tr><tr><td></td><td><i
	nput type="radio" name="R1" value="UDP"> Modbus/UDP</td></tr><tr><td>BootP reque
	st</td><td><input type="checkbox" name="C1" value="on"> Enable</td></tr></table>
	<p><input type="submit" value="Submit" name="B1">&nbsp;&nbsp;&nbsp<input type="r
	eset" value="Reset" name="B2"></p></form></body></html>"
	*/
	QString request=QString ( "GET  HTTP/1.1\nConnection: Keep-Alive\nHost: %1:%2" )
	                .arg ( m_address )
	                .arg ( 8080 );

	tcp->connectToHost ( m_address,8080 );
	m_lastOperation=2;

	if ( !tcp->waitForConnected ( 5000 ) )
	{
		emit ( refreshComplete ( true ) );
		return false;
	}
	tcp->write ( request.toAscii() );
	tcp->flush();
	if ( !tcp->waitForDisconnected ( 5000 ) )
	{
		emit ( refreshComplete ( true ) );
		return false;
	}
	QString rcv=tcp->readAll();
// qDebug() << rcv;
	parseResponse ( rcv );
	emit ( refreshComplete ( false ) );
	return true;
}

void cs8WagoNode::slotRequestFinished ( int id, bool error )
{
	int state=http->state();
	qDebug() << "cs8WagoNode::slotRequestFinished()" << state << id << idCommit << error;
	if ( id==idCommit )
	{
		emit commitComplete ( error );
	}
	/*
	if ( id==idRefresh ) {
	  QString config=buffer->data();
	  qDebug() << config;
	  emit refreshComplete ( error );
	}
	*/
}


void cs8WagoNode::slotReadyRead ( const QHttpResponseHeader & header )
{
	QString config=http->readAll();
	qDebug() << config;
	qDebug() << header.reasonPhrase();
}

QString cs8WagoNode::errorString()
{
	switch ( m_lastOperation )
	{
		case 1:
			return http->errorString();
			break;

		case 2:
			return tcp->errorString();
			break;

		default:
			return tr ( "No Error" );
	}
}


int cs8WagoNode::error()
{
	switch ( m_lastOperation )
	{
		case 1:
			return http->error();
			break;

		case 2:
			return tcp->error();
			break;

		default:
			return 0;
	}
}



void cs8WagoNode::parseResponse ( const QString & response )
{
	QRegExp rx;
	int pos;
	rx=QRegExp ( "name=\"T1\" size=\"\\d+\" value=\"(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		QString address=rx.cap ( 1 );

	rx=QRegExp ( "name=\"T2\" size=\"\\d+\" value=\"(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_ctrlAddress=rx.cap ( 1 );

	rx=QRegExp ( "name=\"T3\" size=\"\\d+\" value=\"(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_subnet=rx.cap ( 1 );

	rx=QRegExp ( "name=\"T4\" size=\"\\d+\" value=\"(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3})" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_gateway=rx.cap ( 1 );

	rx=QRegExp ( "name=\"T5\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_readWordAddress=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"T6\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_writeWordAddress=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"T7\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_readBitAddress=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"T8\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_writeBitAddress=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"T9\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_timeout=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"TA\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_scanRate=rx.cap ( 1 ).toUInt();

	rx=QRegExp ( "name=\"TB\" size=\"\\d+\" value=\"(\\d+)" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_configurationByte=rx.cap ( 1 ).toUInt();

	//<input type="checkbox" name="C1" value="on">
	rx=QRegExp ( "name=\"C1\" (checked) value=\"on\"" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_enableBootPRequest=rx.cap ( 1 ) =="checked"?true:false;
	else
		m_enableBootPRequest=false;

	//<td>Fieldbus protocol</td>
	//<td><input type="radio" checked name="R1"> Modbus/TCP</td>
	//</tr>
	//<tr><td>
	//<input type="radio" name="R1" value="UDP"> Modbus/UDP</td>
	rx=QRegExp ( "type=\"radio\" (checked) name=\"R1\"" );
	pos=rx.indexIn ( response );
	if ( pos>-1 )
		m_protocolTCP=rx.cap ( 1 ) =="checked"?true:false;
	else
		m_protocolTCP= false;


	dumpConfigData();
}


void cs8WagoNode::dumpConfigData()
{
	qDebug() << "Wago Address : " << m_address;
	qDebug() << "CS8  Address : " << m_ctrlAddress;
	qDebug() << "Subnet Mask  : " << m_subnet;
	qDebug() << "Gateway      : " << m_gateway;

	qDebug() << "Word Data:";
	qDebug() << "Read Address : " << m_readWordAddress;
	qDebug() << "Write Address: " << m_writeWordAddress;

	qDebug() << "Bit Data:";
	qDebug() << "Read Address : " << m_readBitAddress;
	qDebug() << "Write Address: " << m_writeBitAddress;

	qDebug() << "Settings:";
	qDebug() << "Config Byte  : " << m_configurationByte;
	qDebug() << "Timeout      : " << m_timeout;
	qDebug() << "Scan Rate    : " << m_scanRate;
	qDebug() << "Protocol     : " << m_protocolTCP;
	qDebug() << "BootP Request: " << m_enableBootPRequest;
}


void cs8WagoNode::commitNewAddress ( const QString & address )
{
	configString=QString ( "T1=%1&B1=Submit" )
	             .arg ( address );
	QByteArray data;
	qDebug() << "sending post string: " << configString;
	data.append ( configString );
	http->setHost ( m_address, 8080 );
	idCommit=http->post ( "/",data );
}



/*!
    \fn cs8WagoNode::saveConfig(QSettings & settings)
 */
bool cs8WagoNode::saveConfig ( QSettings & settings )
{
	settings.beginGroup ( m_name );
	settings.setValue ( "address", m_address );
	settings.setValue ( "ctrlAddress", m_ctrlAddress );
	settings.setValue ( "subnet", m_subnet );
	settings.setValue ( "gateway", m_gateway );
	settings.setValue ( "readWordAddress", m_readWordAddress );
	settings.setValue ( "writeWordAdress", m_writeWordAddress );
	settings.setValue ( "readBitAddress", m_readBitAddress );
	settings.setValue ( "writeBitAddress", m_writeBitAddress );
	settings.setValue ( "timeOut", m_timeout );
	settings.setValue ( "scanRate",m_scanRate );
	settings.setValue ( "configByte", m_configurationByte );
	settings.setValue ( "protocolTCP",m_protocolTCP );
	settings.setValue ( "enableBootPRequest", m_enableBootPRequest );
	settings.endGroup();
	return true;
}


/*!
    \fn cs8WagoNode::restoreConfig(QSettings & settings)
 */
bool cs8WagoNode::restoreConfig ( QSettings & settings )
{
	settings.beginGroup ( m_name );
	m_address=settings.value ( "address", "192.168.0.1" ).toString();
	m_ctrlAddress=settings.value ( "ctrlAddress",  "192.168.0.5" ).toString();
	m_subnet=settings.value ( "subnet", "255.255.255.0" ).toString();
	m_gateway=settings.value ( "gateway",  "0.0.0.0" ).toString();
	m_readWordAddress=settings.value ( "readWordAddress", "0" ).toUInt();
	m_writeWordAddress=settings.value ( "writeWordAdress",  "0" ).toUInt();
	m_readBitAddress=settings.value ( "readBitAddress", "0" ).toUInt();
	m_writeBitAddress=settings.value ( "writeBitAddress", "0" ).toUInt();
	m_timeout=settings.value ( "timeOut",  "100" ).toUInt();
	m_scanRate	=settings.value ( "scanRate", "0" ).toUInt();
	m_configurationByte=settings.value ( "configByte","0" ).toUInt();
	m_protocolTCP=settings.value ( "protocolTCP", "true" ).toString() =="true"?true:false;
	m_enableBootPRequest=settings.value ( "enableBootPRequest",  "true" ).toString() =="true"?true:false;
	settings.endGroup();
	emit (refreshComplete(false));
	return true;
}
