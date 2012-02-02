#ifndef CS8WAGONODE_H
#define CS8WAGONODE_H
//
#include <QObject>
#include <QHttp>
#include <QProgressDialog>
#include <QSettings>

//

class QBuffer;
class cs8WagoNode : public QObject
{
    Q_OBJECT
  protected:
    void parseResponse ( const QString & response );
    
  signals:
    void commitComplete ( bool error );
    void refreshComplete ( bool error  );
    
  public slots:
    void dumpConfigData();
	void commitNewAddress(const QString & address);
    bool refresh();
    void commit(const QString & address=QString());
    
  private:
	int m_lastOperation;
	QString m_name;
    QTcpSocket* tcp;
    QBuffer* buffer;
    QHttp* http;
    int idCommit;
   // int idRefresh;
    QString configString;
    bool m_enableBootPRequest;
    bool m_protocolTCP;
    uint m_configurationByte;
    uint m_scanRate;
    uint m_timeout;
    uint m_writeBitAddress;
    uint m_readBitAddress;
    uint m_writeWordAddress;
    uint m_readWordAddress;
    QString m_gateway;
    QString m_subnet;
    QString m_ctrlAddress;
    QString m_address;
    
  public:
    int  error();
    QString errorString();
    void setEnableBootPRequest ( bool value )
    {
      m_enableBootPRequest = value;
    }
    bool enableBootPRequest()
    {
      return m_enableBootPRequest;
    }
    void setProtocolTCP ( bool value )
    {
      m_protocolTCP = value;
    }
    bool protocolTCP()
    {
      return m_protocolTCP;
    }
    void setConfigurationByte ( int value )
    {
      m_configurationByte = value;
    }
    int configurationByte()
    {
      return m_configurationByte;
    }
    void setScanRate ( int value )
    {
      m_scanRate = value;
    }
    int scanRate()
    {
      return m_scanRate;
    }
    void setTimeout ( int value )
    {
      m_timeout = value;
    }
    int timeout()
    {
      return m_timeout;
    }
    void setWriteBitAddress ( int value )
    {
      m_writeBitAddress = value;
    }
    int writeBitAddress()
    {
      return m_writeBitAddress;
    }
    void setReadBitAddress ( int value )
    {
      m_readBitAddress = value;
    }
    int readBitAddress()
    {
      return m_readBitAddress;
    }
    void setWriteWordAddress ( int value )
    {
      m_writeWordAddress = value;
    }
    int writeWordAddress()
    {
      return m_writeWordAddress;
    }
    void setReadWordAddress ( int value )
    {
      m_readWordAddress = value;
    }
    int readWordAddress()
    {
      return m_readWordAddress;
    }
    void setGateway ( QString value )
    {
      m_gateway = value;
    }
    QString gateway()
    {
      return m_gateway;
    }
    void setSubnet ( QString value )
    {
      m_subnet = value;
    }
    QString subnet()
    {
      return m_subnet;
    }
    void setCtrlAddress ( QString value )
    {
      m_ctrlAddress = value;
    }
    QString ctrlAddress()
    {
      return m_ctrlAddress;
    }
    void setAddress ( QString value )
    {
      m_address = value;
    }
    QString address()
    {
      return m_address;
    }
    cs8WagoNode ( QObject* parent=0 , const QString & name = QString());
    bool saveConfig(QSettings & settings);
    bool restoreConfig(QSettings & settings);

	void setName ( const QString& theValue )
	{
		m_name = theValue;
	}
	

	QString name() const
	{
		return m_name;
	}
	
    
    
  protected slots:
    void slotRequestFinished ( int id, bool error );
    void slotReadyRead ( const QHttpResponseHeader & header );
    
};
#endif
