#ifndef CS8NETWORKSCANNER_H
#define CS8NETWORKSCANNER_H

#include <QAbstractTableModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QHostAddress>
#include <QMap>
#include <QNetworkAddressEntry>
#include <QObject>
#include <QStringList>
#include <QTimer>

/*
#ifdef WIN32
# if DLLEXPORT
#  define MYDLLAPI __declspec(dllexport)
# else
#  define MYDLLAPI __declspec(dllimport)
# endif
#endif
*/

class QTimer;
class QTcpSocket;
class QThread;

struct /*MYDLLAPI*/ cs8NetworkItem {
  QString serialNumber;
  QString address;
  bool detected;
};

class cs8ScannerItem : public QObject {
  Q_OBJECT
public:
  explicit cs8ScannerItem(QObject *parent = 0);

  QTcpSocket *socket() const;

  bool occupied() const;

signals:
  void detected(const QString &address);
  void finished();

public slots:
  void checkHost(const QString &address);

protected slots:
  void slotTimeOut();
  void slotStateChanged(QAbstractSocket::SocketState socketState);
  void slotReadyRead();

private:
  QTcpSocket *m_socket;
  bool m_occupied;
  QTimer *m_timer;
};

class /*MYDLLAPI*/ cs8NetworkScanner : public QAbstractTableModel {
  Q_OBJECT
public:
  cs8NetworkScanner(QObject *parent = 0);
  ~cs8NetworkScanner();

  int rowCount(const QModelIndex & /*index*/) const;
  int columnCount(const QModelIndex &) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation,
                      int role = Qt::DisplayRole) const;
  QString serialNumber(const QString &host);

  bool enabled() const;

protected:
  QTimer *scanTimer;

  bool m_enabled;
  void scanNetwork(QList<QNetworkAddressEntry> entries);
  void controllerSerialNumber(cs8NetworkItem *item_);

signals:
  void scannerStopped();
  void scannerStarted();
  void detected(const QString &host);
  void scanCompleted(uint count);
  void addItemsToScannerWorker(QList<cs8NetworkItem>);
  void finished();
  void scannerActive(bool);

public slots:
  void start(bool singleScan = true);
  void stop(bool waitForStop = true);
  void doScan();
  void cancel();

protected slots:
  void slotScanOfItemFinished();
  void slotScanFinished();
  void slotItemDetected(const QString &address);

private:
  int m_countScanning;
  QList<cs8ScannerItem *> m_items;
  QList<cs8NetworkItem *> m_detectedNetworkItems;
};

#endif // CS8NETWORKSCANNER_H
