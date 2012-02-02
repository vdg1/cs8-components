#ifndef CS8MODBUSCONFIGFILE_H
#define CS8MODBUSCONFIGFILE_H
//
#include <QObject>
#include <QDomNode>
#include <QAbstractTableModel>
#include <QSettings>


#include "cs8modbusitem.h"
//

class cs8ModbusConfigFile : public QAbstractTableModel
{
    Q_OBJECT
    
  public slots:
    void update();
    
  private:
	bool m_readOnly;
    uint m_connections;
    uint m_port;
    QString m_topicName;
    QString m_fileName;
    
  public:
	void setReadOnly(bool value) { m_readOnly = value; }
	bool readOnly() { return m_readOnly; }
	QList<cs8ModbusItem*> getItemsByName(const QString & name);
    cs8ModbusItem* getItemByName ( const QString & name );
    QString proposeItemName ( const QString & base=QString ( "item_%1" ) );
    bool containsItemName ( const QString & name );
    bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool remoteOpen ( const QString & host, const QString & userName, const QString & password );
    QString fileName()
    {
      return m_fileName;
    }
    
    int rowCount ( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex &index, int role ) const;
    QVariant headerData ( int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags ( const QModelIndex &index ) const;
    bool setData ( const QModelIndex &index, const QVariant &value,
                   int role = Qt::EditRole );
                   
    QString toString();
    void setConnections ( uint value )
    {
      m_connections = value;
    }
    
    uint connections()
    {
      return m_connections;
    }
    
    void setPort ( uint value )
    {
      m_port = value;
    }
    
    uint port()
    {
      return m_port;
    }
    
    void setTopicName ( QString value )
    {
      m_topicName = value;
    }
    
    QString topicName()
    {
      return m_topicName;
    }
    
    void open ( const QString & fileName );
    void save ( const QString & fileName );
    
    cs8ModbusConfigFile ( QObject *parent = 0 );
    virtual ~cs8ModbusConfigFile();
    void setView ( QWidget* view );
    bool restoreConfig(QSettings & settings);
    bool saveConfig(QSettings & settings);
    
  protected:
  
    void parseDocument ( QDomDocument doc );
    QList<cs8ModbusItem*> itemList;
    
};


#endif
