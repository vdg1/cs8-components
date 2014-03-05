#ifndef CS8NETWORKSCANNER_H
#define CS8NETWORKSCANNER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QHostAddress>
#include <QFuture>
#include <QFutureWatcher>
#include <QNetworkAddressEntry>
#include <QAbstractTableModel>

#ifdef WIN32
# if DLLEXPORT
#  define MYDLLAPI __declspec(dllexport)
# else
#  define MYDLLAPI __declspec(dllimport)
# endif
#endif


class QTimer;
class QTcpSocket;


struct MYDLLAPI cs8NetworkItem
{
        QString serialNumber;
        QString address;
        bool detected;
};


class MYDLLAPI cs8NetworkScanner : public QAbstractTableModel
{
        Q_OBJECT
    public:
        cs8NetworkScanner(QObject * parent=0);
        ~cs8NetworkScanner();

        int rowCount(const QModelIndex & /*index*/) const;
        int columnCount (const QModelIndex &) const;
        QVariant data(const QModelIndex & index, int role=Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation, int role=Qt::DisplayRole) const;
        QString serialNumber(const QString & host);


    protected:
        QList<cs8NetworkItem*> networkItems;
        QTimer * scanTimer;
        QMap<QString,QTcpSocket*> socketMap;
        QFuture<cs8NetworkItem> m_futureControllerDetection;
        QFuture<cs8NetworkItem> m_futureControllerSerialNumber;
        QFutureWatcher<cs8NetworkItem> m_watcherControllerDetection;
        QFutureWatcher<cs8NetworkItem> m_watcherControllerSerialNumber;
        bool m_enabled;

        //bool controllerIsAvailable(const QString & address) const;
        void scanNetwork(QList<QNetworkAddressEntry> entries);

    protected slots:
        void slotTimeOut();
        void slotScanForControllersFinished();
        void slotScanForControllerResultReadyAt(int index);
        void slotScanForSerialNumberFinished();

    signals:
        void scannerStopped();
        void scannerStarted();
        void detected(const QString & host);
        void scanCompleted(uint count);

    public slots:
        void start(bool singleScan=true);
        void stop(bool waitForStop=true);

    public:

        bool enabled() const
            {
                return m_enabled;
            }


        void cancel();
};

#endif // CS8NETWORKSCANNER_H

