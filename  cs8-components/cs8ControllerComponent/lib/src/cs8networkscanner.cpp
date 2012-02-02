#include "cs8networkscanner.h"
#include "cs8controller.h"

#include <QTimer>
#include <QtNetwork>
#include <QTcpSocket>

#define SCANTIMEOUT 40


bool controllerIsAvailable(const cs8NetworkItem & item) {
        QTcpSocket socket;
        //
        if (item.address.count(".")!=3)
            return false;
        //qDebug() << "checking " << address;
        socket.connectToHost(item.address,21);
        if (!socket.waitForConnected(SCANTIMEOUT)){
            //emit removeHost(address);
            //qDebug() << address << " host not online";
            return false;
        }
        if (!socket.waitForReadyRead(SCANTIMEOUT))
        {
            //emit removeHost(address);
            //qDebug() << address << " not sending data";
            return false;
        }
        QString welcomeMsg=socket.readAll();

        if (!welcomeMsg.contains("VxWorks"))
        {
            //emit removeHost(address);
            qDebug() << item.address << " not a CS8 controller";
            // return false;
        }

        socket.close();
        //qDebug() << address << " ok";
        //emit addHost(address);
        //cs8Controller controller;
        //controller.setAddress (item.address);
        ///TODO:
        // set address for testing
        //QString serial=controller.controllerSerialNumber ();
        //if (serial.isEmpty())
        //    serial=("Unknown");
        //item.serialNumber=serial;

        return true;
    }

cs8NetworkItem controllerSerialNumber(const cs8NetworkItem & item_) {

        cs8NetworkItem item;
        cs8Controller controller;
        controller.setAddress (item_.address);
        ///TODO:
        // set address for testing
        item.serialNumber=controller.controllerSerialNumber ();
        if (item.serialNumber.isEmpty())
            item.serialNumber="Unknown";

        item.address=item_.address;
        qDebug() << "Retrieved serial number for " << item.address << ":" << item.serialNumber;
        return item;
    }


cs8NetworkScanner::cs8NetworkScanner(QObject * parent): QAbstractTableModel(parent)
    {
        m_enabled=false;
        scanTimer=new QTimer(this);
        connect(scanTimer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
        scanTimer->setSingleShot(true);
        scanTimer->setInterval(5000);
        connect(&m_watcherControllerDetection, SIGNAL(finished()), this, SLOT(slotScanForControllersFinished()));
        connect(&m_watcherControllerDetection,SIGNAL(resultReadyAt(int)),this, SLOT(slotScanForControllerResultReadyAt(int)));
        //connect(&m_watcherSerial, SIGNAL(finished()), this, SLOT(slotScanForControllersFinished()));
        connect(&m_watcherControllerSerialNumber,SIGNAL(finished()),this, SLOT(slotScanForSerialNumberFinished()));
    }

cs8NetworkScanner::~cs8NetworkScanner(){
        if (m_futureControllerDetection.isRunning())
        {
            qDebug() << "canceling pending network scan";
            m_futureControllerDetection.cancel();
            m_futureControllerDetection.waitForFinished();
            qDebug() << "scan is canceled";
        }
    }

void cs8NetworkScanner::slotTimeOut()
    {
        QNetworkInterface networkIFace;
        QList<QNetworkInterface> interfaceList=QNetworkInterface::allInterfaces();
        //scanTimer->stop();

        bool ifaceAvailable=false;

        QNetworkInterface iface;
        foreach( iface, interfaceList){
            //if (iface.flags() == (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)){
            if (iface.flags() && (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)!=0){
                //qDebug() << "Scanning iface: "  << " address: " << iface.hardwareAddress() << "flags: " << iface.flags();
                QList<QNetworkAddressEntry> entries;
                foreach(QNetworkAddressEntry entry,iface.addressEntries()){
                    // only scan non local host address and where broadcast address is set
                    if (!entry.ip().toString().startsWith("127.") &&
                            (!entry.ip().toString().contains(":")) &&
                            (entry.broadcast()!=QHostAddress())){
                        //        qDebug()<< "Scanning address: "  << entry.ip() << entry.broadcast();
                        entries << entry;
                    }
                }
                if (entries.count()>0){
                    ifaceAvailable=true;
                    scanNetwork(entries);
                }
            }
        }
        if (!ifaceAvailable){
            networkItems.clear ();
            reset ();
            if (m_enabled)
                scanTimer->start();
        }
    }

void cs8NetworkScanner::scanNetwork(QList<QNetworkAddressEntry> entries)
    {
        QList<cs8NetworkItem> destinationAddresses;
        foreach(QNetworkAddressEntry entry,entries){
            int maxOctet4=entry.broadcast().toString().split(".").at(3).toUInt();
            QString destinationNetwork=entry.ip().toString();
            destinationNetwork=destinationNetwork.left(destinationNetwork.lastIndexOf("."));
            destinationNetwork.append(".%1");


            for (int octet4=1 ; octet4<maxOctet4; octet4++){
                cs8NetworkItem item;
                item.address=destinationNetwork.arg(octet4);
                destinationAddresses.append(item);
            }

        }
        //networkItems.clear ();
        // reset items in network list
        foreach (cs8NetworkItem* item, networkItems)
            item->detected=false;
        //
        m_futureControllerDetection = QtConcurrent::filtered(destinationAddresses, controllerIsAvailable);
        //qDebug() << "New scan started ";
        m_watcherControllerDetection.setFuture(m_futureControllerDetection);
    }

void cs8NetworkScanner::slotScanForControllersFinished(){
        QList<cs8NetworkItem> result=m_futureControllerDetection.results();

        if (m_enabled)
            scanTimer->start();
        else
        {
            emit scannerStopped();
            qDebug() << "Scanner stopped";
        }
        for (int i=0;i<networkItems.size ();i++)
        {
            if (!networkItems.at (i)->detected)
            {
                beginRemoveRows (QModelIndex(),i,i);
                networkItems.removeAt (i);
                endRemoveRows ();
            }
        }
        emit scanCompleted (networkItems.count ());
    }

/*!
    \fn cs8NetworkScanner::stop(bool waitForStop)
 */
void cs8NetworkScanner::stop(bool waitForStop)
    {
        scanTimer->stop();
        m_enabled=false;
        m_futureControllerDetection.cancel();
        if (waitForStop)
            m_futureControllerDetection.waitForFinished();
        //m_hostList.clear();
        emit scannerStopped();
        qDebug() << "Scanner stopped";
    }


/*!
    \fn cs8NetworkScanner::start(bool)
 */
void cs8NetworkScanner::start(bool singleScan)
    {
        if ( m_futureControllerDetection.isRunning())
        {
            qDebug() << "Scanner is already active";
            qDebug() << "running: " << m_futureControllerDetection.isRunning ();
            qDebug() << "started: " << m_futureControllerDetection.isStarted ();
            return;
        }
        emit scannerStarted();
        if (!singleScan){
            m_enabled=true;
        }
        slotTimeOut();
        qDebug() << "Scanner started";
    }

void cs8NetworkScanner::slotScanForControllerResultReadyAt(int index)
    {
        cs8NetworkItem host=m_futureControllerDetection.resultAt(index);
        qDebug() << "detected: " << host.address;
        if (host.serialNumber.isEmpty ())
        {
            // add detected controller to list
            bool found=false;
            for (int i=0;i<networkItems.size ();i++)
            {
                if (networkItems[i]->address==host.address)
                {
                    networkItems[i]->detected=true;
                    found=true;
                }
            }
            if (!found)
            {
                beginInsertRows (QModelIndex(),0,networkItems.count());
                cs8NetworkItem* item=new cs8NetworkItem;
                item->address=host.address;
                item->serialNumber=tr("Retrieving...");
                item->detected=true;
                networkItems.insert(0,item);
                endInsertRows ();
                //
                emit detected (item->address);
            }
            m_futureControllerSerialNumber=QtConcurrent::run(controllerSerialNumber, host);
            m_watcherControllerSerialNumber.setFuture (m_futureControllerSerialNumber);
        }
    }

void cs8NetworkScanner::slotScanForSerialNumberFinished()
    {
        QList<cs8NetworkItem> hosts=m_futureControllerSerialNumber.results ();
        qDebug() << "detected: " << hosts[0].address;
        if (!hosts[0].serialNumber.isEmpty ())
        {
            for (int i=0;i<networkItems.size ();i++)
            {
                if (networkItems.at (i)->address==hosts[0].address)
                {
                    networkItems[i]->detected=true;
                    networkItems[i]->serialNumber=hosts[0].serialNumber;
                    QModelIndex index=createIndex (i,1);
                    emit dataChanged (index,index);

                }
            }
        }
    }

int cs8NetworkScanner::rowCount(const QModelIndex &index) const
    {
        if (networkItems.count ()==0)
            return 1;
        else
            return networkItems.count();
    }

int cs8NetworkScanner::columnCount(const QModelIndex &index) const
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
        if (networkItems.count ()>0){
            switch (index.column ()){
            case 0:
                return networkItems.at(row)->address;
                break;
            case 1:
                return networkItems.at(row)->serialNumber;
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
                return "No controller detected";
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
        }
    }

QString cs8NetworkScanner::serialNumber(const QString &host)
    {
        foreach(cs8NetworkItem* item,networkItems)
            if (item->address==host)
                return item->serialNumber;
        return tr("unknown");
    }
