//
// C++ Implementation: cs8statisticmodel
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8statisticmodel.h"

#include <QStandardItemModel>
#include <QList>
#include <QDebug>
#include <QDateTime>

cs8LogEntry::cs8LogEntry ( const QString& lineText_,int lineNumber_ )
    :lineText ( lineText_ ),lineNumber ( lineNumber_ )
{}

cs8StatisticModel::cs8StatisticModel ( QObject * parent )
    : QStandardItemModel ( parent )
{
  setup();
}


cs8StatisticModel::~cs8StatisticModel()
{}


void cs8StatisticModel::addStatisticInfo_ ( const QString & line_, int lineNumber )
{
  QString line=line_;
  QString dateStr=line.left ( line.indexOf ( ']' ) ).remove ( 0,1 );
  QDateTime date;
  if ( dateStr.contains ( "/" ) ) {
    date=QDateTime::fromString ( dateStr,QString ( "MM/dd/yy HH:mm:ss" ) );
    // Y2K problem
    date=date.addYears ( 100 );
  } else {
    date=QDateTime::fromString ( dateStr,QString ( "MMM dd yyyy HH:mm:ss" ) );
  }
  QString entryText = line.remove ( 0, line.indexOf ( "]" ) +2 ).trimmed();
  // check for multiple entries marked with (int)
  int multiple=0;
  if ( entryText.contains ( "(" ) ) {
    if ( entryText.contains ( QRegExp ( "\\(\\d+\\)$" ) ) ) {
      qDebug() << "multiple entry found " << entryText;
      multiple=entryText.section ( '(',-1 ).toInt();
      entryText=entryText.remove ( QRegExp ( "\\(\\d*\\)$" ) ).trimmed();
    }
  }
  if ( entryText.contains ( "{" ) ) {
    // check if entry contains {x,z,y,a,b,c}
    QRegExp rx ( "\\{(-{0,1}\\d*\\.{0,1}\\d*,){5}(-{0,1}\\d*\\.{0,1}\\d*)\\}" );
    if ( entryText.contains ( rx ) ) {
      entryText=entryText.remove ( rx ).trimmed();
    }
  }
  addEntry ( entryText,date,lineNumber );
}



/*!
    \fn cs8StatisticModel::addEntry(const QString & text, const QDateTime & timeStamp, int line)
 */
void cs8StatisticModel::addEntry ( const QString & text, const QDateTime & timeStamp, int line )
{
  if ( text.isEmpty() )
    return;
  QStandardItem *parentItem = invisibleRootItem();
  QStandardItem* newItem=new QStandardItem ( timeStamp.toString ( Qt::ISODate ) );
  newItem->setData ( line );
  newItem->setData ( timeStamp,Qt::UserRole+5 );
  newItem->setToolTip ( tr ( "Occured in line %1" ).arg ( line ) );
  newItem->setEditable ( false );
  
  QStandardItem* item;
  if ( m_itemHash.contains ( text ) ) {
    item=m_itemHash.value ( text );
  } else {
    item=new QStandardItem ( text );
    item->setEditable ( false );
    item->setCheckable ( true );
    parentItem->appendRow ( item );
    m_itemHash[text]=item;
  }
  Q_ASSERT ( item!=0 );
  item->appendRow ( newItem );
  item->setToolTip ( tr ( "Occurences: %1" ).arg ( item->rowCount() ) );
  
  // update time intervall of log entry
  if ( item->data ( Qt::UserRole+1 ).toDateTime() > timeStamp )
    item->setData ( timeStamp,Qt::UserRole+1 );
  if ( item->data ( Qt::UserRole+2 ).toDateTime() < timeStamp )
    item->setData ( timeStamp,Qt::UserRole+2 );
    
  //qDebug() << "intervall before update: " << m_timeStart << " - " << m_timeStop;
  
  if ( m_firstEntry ) {
    // first item in model
    if ( timeStamp.isValid() ) {
      //qDebug() << "first item";
      m_timeStart=timeStamp;
      m_timeStop=timeStamp;
      m_firstEntry=false;
    }
  } else {
    // update time intervall of model
    if ( timeStamp.isValid() ) {
      //qDebug() << "update intervall";
      m_timeStart=qMin ( m_timeStart,timeStamp );
      m_timeStop=qMax ( m_timeStop,timeStamp );
    }
  }
  //qDebug() << "intervall after update: " << m_timeStart << " - " << m_timeStop;
}

void cs8StatisticModel::slotUpdate()
{
  if ( !m_queue.isEmpty() ) {
    int i=0;
    while ( ( i<10 ) and ( m_queue.count() >0 ) ) {
      cs8LogEntry* entry=m_queue.pop();
      addStatisticInfo_ ( entry->lineText,entry->lineNumber );
      delete entry;
      i++;
    }
  } else {
    m_timer->stop();
  }
}

void cs8StatisticModel::addStatisticInfo ( const QString & line_, int lineNumber )
{
  cs8LogEntry* entry=new cs8LogEntry ( line_,lineNumber );
  if ( m_queue.count() ==0 )
    m_timer->start ( 0 );
  m_queue.push ( entry );
  
}

void cs8StatisticModel::setup()
{
  setColumnCount ( 1 );
  setHorizontalHeaderLabels ( QStringList() << "Text" << "Line" );
  m_timer = new QTimer ( this );
  m_timeStart=QDateTime::fromTime_t ( 0 );
  m_timeStop=QDateTime::currentDateTime();
  m_firstEntry=true;
  connect ( m_timer, SIGNAL ( timeout() ), this ,SLOT ( slotUpdate() ) );
}

void cs8StatisticModel::clear()
{
  m_queue.clear();
  m_itemHash.clear();
  QStandardItemModel::clear();
  setup();
}


void cs8StatisticModel::togglePlot ( const QString & message )
{
  qDebug() << "cs8StatisticModel::togglePlot ( const QString & message )";
  for ( int i=0;invisibleRootItem()->rowCount ();i++ ) {
    QStandardItem *item=invisibleRootItem()->child ( i,0 );
    if ( message.indexOf ( item->text() ) >1 ) {
      item->setCheckState ( item->checkState() ==Qt::Checked?Qt::Unchecked:Qt::Checked );
      return;
    }
  }
}
