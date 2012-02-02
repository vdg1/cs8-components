#include "cs8modbusconfigfile.h"
#include "cs8modbusdelegate.h"


#include <QFile>
#include <QDebug>
#include <QSpinBox>
#include <QTableView>
#include <QMessageBox>

#include <QUrl>
//
cs8ModbusConfigFile::cs8ModbusConfigFile ( QObject *parent )
    : QAbstractTableModel ( parent )
{}

//
cs8ModbusConfigFile::~cs8ModbusConfigFile()
{}

//
void cs8ModbusConfigFile::open ( const QString & fileName )
{
  m_fileName = fileName;
  
  QDomDocument doc;
  QFile file ( m_fileName );
  
  if ( !file.open ( QIODevice::ReadOnly ) )
    return;
    
  if ( !doc.setContent ( &file ) ) {
    file.close();
    return;
  }
  
  file.close();
  
  parseDocument ( doc );
}



/*!
    \fn cs8ModbusConfigFile::save(const QString & fileName)
 */
void cs8ModbusConfigFile::save ( const QString & fileName )
{

  QFile file ( fileName );
  
  if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) )
    return;
    
  QTextStream out ( &file );
  
  out << toString();
  
  file.close();
}

QString cs8ModbusConfigFile::toString()
{
  QDomDocument doc;
  QDomElement root = doc.createElement ( "TOPICSLIST" );
  doc.appendChild ( root );
  
  QDomElement topic = doc.createElement ( "TOPIC" );
  root.appendChild ( topic );
  
  QDomElement bitModule = doc.createElement ( "MODULE" );
  topic.appendChild ( bitModule );
  int offset = 0;
  
  for ( int i = 0;i < itemList.count();i++ ) {
    if ( itemList.at ( i )->type() == cs8ModbusItem::Bit ) {
      QDomElement item = doc.createElement ( "ITEM" );
      
      item.setAttribute ( "Name", itemList.at ( i )->name() );
      
      QString syntax;
      syntax = QString ( "%%1%2" )
               .arg ( itemList.at ( i )->accessType() == cs8ModbusItem::R ? "I" : "Q" )
               .arg ( offset );
               
      if ( itemList.at ( i )->size() > 1 )
        syntax += QString ( ":%1" ).arg ( itemList.at ( i )->size() );
        
      item.setAttribute ( "Syntaxe", syntax );
      
      item.setAttribute ( "Offset", QString ( "%1" ).arg ( offset ) );
      
      bitModule.appendChild ( item );
      
      offset += itemList.at ( i )->size() * itemList.at ( i )->variableSize();
    }
  }
  
  bitModule.setAttribute ( "Tag_Name_Root", "" );
  
  bitModule.setAttribute ( "IDModule", "BOOLEAN" );
  bitModule.setAttribute ( "InputSize", "" );
  bitModule.setAttribute ( "OutputSize", QString ( "%1" ).arg ( offset ) );
  
  QDomElement byteModule = doc.createElement ( "MODULE" );
  topic.appendChild ( byteModule );
  offset = 0;
  
  for ( int i = 0;i < itemList.count();i++ ) {
    if ( itemList.at ( i )->type() != cs8ModbusItem::Bit ) {
      QDomElement item = doc.createElement ( "ITEM" );
      
      item.setAttribute ( "Name", itemList.at ( i )->name() );
      
      QString syntax;
      syntax = QString ( "%%1%2%3" )
               .arg ( itemList.at ( i )->accessType() == cs8ModbusItem::R ? "I" : "Q" )
               .arg ( itemList.at ( i )->typeChar() )
               .arg ( offset );
               
      if ( itemList.at ( i )->size() > 1 )
        syntax += QString ( ":%1" ).arg ( itemList.at ( i )->size() );
        
      item.setAttribute ( "Syntaxe", syntax );
      
      item.setAttribute ( "Offset", QString ( "%1" ).arg ( offset ) );
      
      byteModule.appendChild ( item );
      
      offset += itemList.at ( i )->size() * itemList.at ( i )->variableSize();
    }
  }
  
  byteModule.setAttribute ( "Tag_Name_Root", "" );
  
  byteModule.setAttribute ( "IDModule", "WORD" );
  byteModule.setAttribute ( "InputSize", "" );
  byteModule.setAttribute ( "OutputSize", QString ( "%1" ).arg ( offset*16 ) );
  
  
  QDomElement generalParameters = doc.createElement ( "GENERAL_PARAMETERS" );
  topic.appendChild ( generalParameters );
  generalParameters.setAttribute ( "TopicName", m_topicName );
  generalParameters.setAttribute ( "TCP_Port", QString ( "%1" ).arg ( m_port ) );
  generalParameters.setAttribute ( "ConnectionNumber", QString ( "%1" ).arg ( m_connections ) );
  
  return doc.toString();
}

int cs8ModbusConfigFile::rowCount ( const QModelIndex &parent ) const
{
  return itemList.count();
}

int cs8ModbusConfigFile::columnCount ( const QModelIndex &parent ) const
{
  return 5;
}

QVariant cs8ModbusConfigFile::data ( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();
    
  if ( index.row() >= itemList.size() )
    return QVariant();
    
  if ( role == Qt::DisplayRole ) {
    cs8ModbusItem* item = itemList.at ( index.row() );
    
    switch ( index.column() ) {
    
      case 0:
        return item->name();
        break;
        
      case 1:
        return item->typeChar ( role );
        break;
        
      case 2:
        return item->size();
        break;
        
      case 3:
        return item->accessType() == 1 ? QString ( "RW" ) : QString ( "R" );
        break;
        
      case 4:
        return item->offset();
        break;
        
      default:
        return QVariant();
    }
  }
  
  if ( role == Qt::EditRole ) {
    cs8ModbusItem* item = itemList.at ( index.row() );
    
    switch ( index.column() ) {
    
      case 0:
        return item->name();
        break;
        
      case 1:
        return item->typeChar ( role );
        break;
        
      case 2:
        return item->size();
        break;
        
      case 3:
        return item->accessType() == 1 ? 1 : 0;
        
      default:
        return QVariant();
    }
  } else
    return QVariant();
}

QVariant cs8ModbusConfigFile::headerData ( int section, Qt::Orientation orientation,
    int role ) const
{
  if ( role != Qt::DisplayRole )
    return QVariant();
    
  if ( orientation == Qt::Horizontal )
    switch ( section ) {
    
      case 0:
        return tr ( "Name" );
        break;
        
      case 1:
        return tr ( "Type" );
        break;
        
      case 2:
        return tr ( "Size" );
        break;
        
      case 3:
        return tr ( "Access" );
        break;
        
      case 4:
        return tr ( "Address" );
        break;
      default:
        return QString ( "" );
    }
  else
    return QString ( "%1" ).arg ( section );
}

Qt::ItemFlags cs8ModbusConfigFile::flags ( const QModelIndex &index ) const
{
  if ( !index.isValid() )
    return Qt::ItemIsEnabled;
    
  if ( m_readOnly )
    return QAbstractItemModel::flags ( index );
  else
    return QAbstractItemModel::flags ( index ) | Qt::ItemIsEditable ;
  }
  
bool cs8ModbusConfigFile::setData ( const QModelIndex &index,
                                    const QVariant &value, int role )
{
  if ( index.isValid() && role == Qt::EditRole ) {
  
    cs8ModbusItem* item = itemList.at ( index.row() );
    
    switch ( index.column() ) {
    
      case 0:
        item->setName ( value.toString() );
        break;
        
      case 1:
        item->setType ( value.toString() );
        update();
        break;
        
      case 2:
        item->setSize ( value.toUInt() );
        update();
        break;
        
      case 3:
        item->setAccessType ( value.toString() =="RW"?cs8ModbusItem::RW:cs8ModbusItem::R );
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}



/*!
    \fn cs8ModbusConfigFile::setView(QWidget* view)
 */
void cs8ModbusConfigFile::setView ( QWidget* view )
{
  cs8ModbusDelegate* delegate=new cs8ModbusDelegate ( this );
  ( ( QTableView* ) view )->setItemDelegate ( delegate );
}

void cs8ModbusConfigFile::parseDocument ( QDomDocument doc )
{
  qDebug() << "content: " << doc.toString();
  itemList.clear();
  QDomElement topicsList = doc.documentElement();
  
  // read general parameters
  QDomElement generalParameters = topicsList.toElement().elementsByTagName ( "GENERAL_PARAMETERS" ).at ( 0 ).toElement();
  m_topicName = generalParameters.attribute ( "TopicName", "Modbus" );
  m_port = generalParameters.attribute ( "TCP_Port", "502" ).toInt();
  m_connections = generalParameters.attribute ( "ConnectionNumber", "2" ).toInt();
  // read items
  QDomElement topic = topicsList.toElement().elementsByTagName ( "TOPIC" ).at ( 0 ).toElement();
  QDomNode bitNode;
  QDomNode byteNode;
  QDomNode n = topic.firstChild();
  
  while ( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    
    if ( !e.isNull() ) {
      qDebug() << e.tagName(); // the node really is an element.
      
      if ( e.attribute ( "IDModule" ) == "BOOLEAN" )
        bitNode = e;
        
      if ( e.attribute ( "IDModule" ) == "WORD" )
        byteNode = e;
    }
    
    n = n.nextSibling();
  }
  
  n = bitNode.firstChild();
  
  while ( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    
    if ( !e.isNull() ) {
      cs8ModbusItem* item = new cs8ModbusItem ( e );
      itemList.append ( item );
    }
    
    n = n.nextSibling();
  }
  
  n = byteNode.firstChild();
  
  while ( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    
    if ( !e.isNull() ) {
      cs8ModbusItem* item = new cs8ModbusItem ( e );
      itemList.append ( item );
    }
    
    n = n.nextSibling();
  }
  update();
  reset();
}


bool cs8ModbusConfigFile::remoteOpen ( const QString & host, const QString & userName, const QString & password )
{
  QUrl url;
  QFile file;
  QDomDocument doc;
  url.setUrl ( QString ( "cs8://%1:%2@%3/usr/applicom/modbus/modbus.xml" ).arg ( userName ).arg ( password ).arg ( host ) );
  file.setFileName ( url.toString() );
  if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) ) {
    QMessageBox::warning ( 0,
                           tr ( "Error opening file" ),
                           tr ( "File %3 could not be opened: (%1) %2" )
                           .arg ( file.error() )
                           .arg ( file.errorString() )
                           .arg ( url.toString() ) );
    return false;
  }
  doc.setContent ( &file );
  file.close();
  parseDocument ( doc );
  return true;
}



/*!
    \fn cs8ModbusConfigFile::insertRow( int row, int count, const QModelIndex & parent)
 */
bool cs8ModbusConfigFile::insertRows ( int row, int count, const QModelIndex & parent )
{
  beginInsertRows ( QModelIndex(), row, row+count );
  for ( int i=0;i<count;i++ ) {
    cs8ModbusItem* item = new cs8ModbusItem ( );
    item->setName ( proposeItemName() );
    itemList.insert ( row,item );
  }
  endInsertRows();
  update();
  return true;
}

/*!
    \fn cs8ModbusConfigFile::removeRows(  int row, int count, const QModelIndex & parent )
 */
bool cs8ModbusConfigFile::removeRows (  int row, int count, const QModelIndex & parent )
{
  beginRemoveRows ( QModelIndex(), row, row+count );
  for ( int i=0;i<count;i++ )
    itemList.removeAt ( row );
  endRemoveRows();
  return true;
}



bool cs8ModbusConfigFile::containsItemName ( const QString & name )
{
  for ( int i=0;i<itemList.count();i++ ) {
    cs8ModbusItem* item=itemList.at ( i );
    if ( item->name().compare ( name,Qt::CaseInsensitive ) ==0 )
      return true;
  }
  return false;
}


QString cs8ModbusConfigFile::proposeItemName ( const QString & base )
{
  QString nameBase;
  if ( !base.contains ( "%1" ) )
    nameBase=base+"%1";
  else
    nameBase=base;
  int i=0;
  while ( containsItemName ( nameBase.arg ( i ) ) )
    i++;
  return nameBase.arg ( i );
}


void cs8ModbusConfigFile::update()
{
  int offset = 0;
  
  for ( int i = 0;i < itemList.count();i++ ) {
    if ( itemList.at ( i )->type() == cs8ModbusItem::Bit ) {
      itemList.at ( i )->setOffset ( offset );
      offset += itemList.at ( i )->size() * itemList.at ( i )->variableSize();
    }
  }
  offset=0;
  for ( int i = 0;i < itemList.count();i++ ) {
    if ( itemList.at ( i )->type() != cs8ModbusItem::Bit ) {
      itemList.at ( i )->setOffset ( offset );
      offset += itemList.at ( i )->size() * itemList.at ( i )->variableSize();
    }
  }
  reset();
}


cs8ModbusItem* cs8ModbusConfigFile::getItemByName ( const QString & name )
{
  for ( int i=0; i<itemList.count();i++ )
    if ( itemList.at ( i )->name() ==name )
      return itemList.at ( i );
  qDebug() << "cs8ModbusConfigFile::getItemByName(): item not found " << name;
  return new cs8ModbusItem();
}


QList<cs8ModbusItem*> cs8ModbusConfigFile::getItemsByName ( const QString & name )
{
  QList<cs8ModbusItem*> list;
  QRegExp rx ( name );
  rx.setPatternSyntax ( QRegExp::Wildcard );
  
  for ( int i=0; i<itemList.count();i++ )
    if ( rx.exactMatch ( itemList.at ( i )->name() ) )
    {
    	qDebug() << "cs8ModbusConfigFile::getItemsByName ()"
					<< itemList.at ( i )->name() << itemList.at(i)->accessType() << itemList.at(i)->type();
      list.append ( itemList.at ( i ) );
    }
  return list;
}



/*!
    \fn cs8ModbusConfigFile::saveConfig(QSettings settings)
 */
bool cs8ModbusConfigFile::saveConfig(QSettings & settings)
{
	settings.setValue("modbus/xml", toString());
}


/*!
    \fn cs8ModbusConfigFile::restoreConfig(QSettings settings)
 */
bool cs8ModbusConfigFile::restoreConfig(QSettings & settings)
{
	QString xml=settings.value("modbus/xml", QString()).toString();
	QDomDocument doc;
	doc.setContent(xml);
	parseDocument(doc);
}
