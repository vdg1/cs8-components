#include "cs8networkscanner.h"
#include "cs8controller.h"

#include <QTimer>
#include <QtNetwork>
#include <QTcpSocket>
#include <QtConcurrent>

#define SCANTIMEOUT 1000


Q_DECLARE_METATYPE(QList<cs8NetworkItem>)

void cs8NetworkScanner::controllerSerialNumber(cs8NetworkItem * item) {

    cs8Controller controller;
    controller.setAddress (item->address);
    item->serialNumber=controller.controllerSerialNumber ();
    if (item->serialNumber.isEmpty())
        item->serialNumber="Unknown";

    qDebug() << "Retrieved serial number for " << item->address << ":" << item->serialNumber;
}


cs8NetworkScanner::cs8NetworkScanner(QObject * parent): QAbstractTableModel(parent),
    m_countScanning(0)
{
    m_enabled=false;
    scanTimer=new QTimer(this);
    connect(scanTimer,&QTimer::timeout,this,&cs8NetworkScanner::doScan);
    scanTimer->setSingleShot(true);
    scanTimer->setInterval(5000);
    connect(this,&cs8NetworkScanner::finished,this,&cs8NetworkScanner::slotScanFinished);
}

void cs8NetworkScanner::cancel()
{

}

cs8NetworkScanner::~cs8NetworkScanner(){
    cancel();
}


/*!
    \fn cs8NetworkScanner::stop(bool waitForStop)
 */
void cs8NetworkScanner::stop(bool waitForStop)
{
    qDebug() << "stopping scanner";
    scanTimer->stop();
    m_enabled=false;
    emit scannerActive(false);
    emit scannerStopped();
    qDebug() << "Scanner stopped";
}


/*!
    \fn cs8NetworkScanner::start(bool)
 */
void cs8NetworkScanner::start(bool singleScan)
{

    emit scannerStarted();
    emit scannerActive(true);
    if (!singleScan){
        m_enabled=true;
    }
    doScan();
    qDebug() << "Scanner started";
}

void cs8NetworkScanner::doScan()
{
    // reset already detected items
    foreach(cs8NetworkItem* item,m_detectedNetworkItems)
        item->detected=false;

    QList<QNetworkInterface> interfaceList=QNetworkInterface::allInterfaces();

    bool ifaceAvailable=false;

    QNetworkInterface iface;
    foreach( iface, interfaceList){
        //if (iface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)){
        if (iface.flags() && (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)!=0){
            qDebug() << "Scanning iface: "  << " address: " << iface.hardwareAddress() << "flags: " << iface.flags();
            QList<QNetworkAddressEntry> entries;
            foreach(QNetworkAddressEntry entry,iface.addressEntries()){
                // only scan non local host address and where broadcast address is set
                if (!entry.ip().toString().startsWith("127.") &&
                        (!entry.ip().toString().contains(":")) &&
                        (entry.broadcast()!=QHostAddress())){
                    qDebug()<< "Scanning address: "  << entry.ip() << entry.broadcast();
                    entries << entry;
                }
            }
            if (entries.count()>0){
                ifaceAvailable=true;
                scanNetwork(entries);
            }
        }
    }
}

void cs8NetworkScanner::slotScanOfItemFinished()
{
    m_countScanning--;
    if (m_countScanning==0)
    {
        emit finished();
        if (m_enabled)
            QTimer::singleShot(5000,this, SLOT(doScan()));
    }
}

void cs8NetworkScanner::slotScanFinished()
{
    qDebug() << __FUNCTION__;
    beginResetModel();
    for (int i=0;i<m_detectedNetworkItems.count();i++)
    {
        // remove controllers that are not detected in this scan
        if (!m_detectedNetworkItems.at(i)->detected)
            delete m_detectedNetworkItems.takeAt(i);
    }
    endResetModel();
    emit scanCompleted(m_detectedNetworkItems.count());
    emit scannerActive(false);
    emit scannerStopped();
}

void cs8NetworkScanner::slotItemDetected(const QString &address)
{
    // check if already detected in previous scan
    bool found=false;
    beginResetModel();
    for (int i=0;i<m_detectedNetworkItems.count();i++)
    {
        if (m_detectedNetworkItems.at(i)->address==address)
        {
            m_detectedNetworkItems.at(i)->detected=true;
            found=true;
        }
    }
    if (!found)
    {
        cs8NetworkItem *item=new cs8NetworkItem();
        item->address=address;
        item->detected=true;
        controllerSerialNumber(item);
        m_detectedNetworkItems.append(item);
    }
    endResetModel();
}

void cs8NetworkScanner::scanNetwork(QList<QNetworkAddressEntry> entries)
{

    foreach(QNetworkAddressEntry entry,entries){
        int maxOctet4=entry.broadcast().toString().split(".").at(3).toUInt();
        QString destinationNetwork=entry.ip().toString();
        destinationNetwork=destinationNetwork.left(destinationNetwork.lastIndexOf("."));
        destinationNetwork.append(".%1");


        for (int octet4=1 ; octet4<maxOctet4; octet4++){
            cs8ScannerItem *item=new cs8ScannerItem(this);
            connect(item,&cs8ScannerItem::detected,this,&cs8NetworkScanner::slotItemDetected);
            connect(item,&cs8ScannerItem::finished,this,&cs8NetworkScanner::slotScanOfItemFinished);
            m_countScanning++;
            item->checkHost(destinationNetwork.arg(octet4));
        }
    }
}


int cs8NetworkScanner::rowCount(const QModelIndex &) const
{
    if (m_detectedNetworkItems.count ()==0)
        return 1;
    else
        return m_detectedNetworkItems.count();
}

int cs8NetworkScanner::columnCount(const QModelIndex & /*index*/) const
{
    return 2;
}

QVariant cs8NetworkScanner::data(const QModelIndex &index, int role) const
{
    if (role!=Qt::DisplayRole)
        return QVariant();

    if (!index.isValid ())
        return QVariant();

    int row=index.row ();
    if (m_detectedNetworkItems.count ()>0){
        switch (index.column ()){
        case 0:
            return m_detectedNetworkItems.at(row)->address;
            break;
        case 1:
            return m_detectedNetworkItems.at(row)->serialNumber;
            break;
        default:
            return QVariant();
            break;
        }
    }
    else
    {
        switch (index.column ()){
        case 0:
            return tr("No controller detected");
            break;
        default:
            return QVariant();
            break;
        }
    }
}

QVariant cs8NetworkScanner::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role!=Qt::DisplayRole)
        return QVariant();

    if (orientation==Qt::Vertical)
        return QVariant();

    switch (section){
    case 0:
        return tr ("Network Address");
        break;

    case 1:
        return tr ("Serial Number");
        break;

    default:
        return QString();
        break;
    }
}

QString cs8NetworkScanner::serialNumber(const QString &host)
{
    foreach(cs8NetworkItem* item,m_detectedNetworkItems)
        if (item->address==host)
            return item->serialNumber;
    return tr("unknown");
}

bool cs8NetworkScanner::enabled() const
{
    return m_enabled;
}

cs8ScannerItem::cs8ScannerItem(QObject *parent) : QObject(parent)
{
    m_socket=new QTcpSocket(this);
    connect(m_socket,&QAbstractSocket::stateChanged,this,&cs8ScannerItem::slotStateChanged);
    connect(m_socket,&QAbstractSocket::readyRead,this,&cs8ScannerItem::slotReadyRead);

    m_timer=new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&cs8ScannerItem::slotTimeOut);
}

QTcpSocket *cs8ScannerItem::socket() const
{
    return m_socket;
}

bool cs8ScannerItem::occupied() const
{
    return m_occupied;
}

void cs8ScannerItem::checkHost(const QString &address)
{
    m_socket->connectToHost(address,21);
}

void cs8ScannerItem::slotTimeOut()
{
    m_socket->abort();
    m_occupied=false;
    emit finished();
    deleteLater();
}

void cs8ScannerItem::slotStateChanged(QAbstractSocket::SocketState socketState)
{
    //qDebug() << "socket state of connection to " << m_socket->peerAddress() << " to " << socketState;
    m_timer->start();
}

void cs8ScannerItem::slotReadyRead()
{
    QString welcomeMsg=m_socket->readAll();
    m_timer->stop();
    m_occupied=false;

    if (!welcomeMsg.contains("VxWorks") && !welcomeMsg.contains("Session limit"))
    {
        //emit removeHost(address);
        qDebug() << m_socket->peerAddress() << " not a CS8 controller";
    }
    else
    {
        qDebug() << m_socket->peerAddress() << " is a CS8 controller";
        emit detected(m_socket->peerAddress().toString());
    }
    emit finished();
    deleteLater();
}

