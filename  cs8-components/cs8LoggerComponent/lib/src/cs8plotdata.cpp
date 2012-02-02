#include "cs8plotdata.h"
#include <QStandardItem>
#include <QDebug>
//
cs8PlotData::cs8PlotData(  )
    : QwtData(), m_itemRoot ( 0 )
{
  // TODO
}
//

size_t cs8PlotData::size() const
  {
    //qDebug() << "returning size of plot data : " << m_size;
    return m_size;
  }

void cs8PlotData::update()
{
  if (m_itemRoot==NULL)
    return;
  m_dataX.clear();
  m_dataY.clear();
  int index=0;
  int j=0;
  for ( QDateTime i=m_timeStop;i>m_timeStart; )
    {
      int count=0;
      QDateTime start=i;
      QDateTime stop=start.addSecs ( m_scanIntervall );
      for (;j<m_itemRoot->rowCount();j++ )
        {
          QDateTime timeStamp=m_itemRoot->child ( j,0 )->data ( Qt::UserRole+5 ).toDateTime();
          if (timeStamp<start)
            break;
          if ( ( timeStamp>=start ) && timeStamp<stop )
            count++;
        }
      //if (count>0)
      {
        m_dataY[index]=count;
        //m_dataY[index+1]=0;
        m_dataX[index]=start;
        //m_dataX[index+1]=start.addSecs(m_scanIntervall);
        index++;
      }

      i=i.addSecs (-m_scanIntervall );
    }
  m_size=index+1;
  //dumpData();
}


double cs8PlotData::x ( size_t i ) const
  {
    //qDebug() << "         accessing X value of " << i << ":" << m_dataX.value ( i ).toTime_t();
    if (m_dataX.contains(i))
      return m_dataX.value ( i ).toTime_t();
    else
      return 0;
  }

double cs8PlotData::y ( size_t i ) const
  {
    //qDebug() << "         accessing Y value of " << i << ":" << m_dataY.value ( i );
    if (m_dataY.contains(i))
      return m_dataY.value ( i );
    else
      return 0;
  }

cs8PlotData* cs8PlotData::copy() const
  {
    cs8PlotData *data=new cs8PlotData();
    data->setDataX(m_dataX);
    data->setDataY(m_dataY);
    data->setScanIntervall(m_scanIntervall);
    data->setSize(m_size);
    data->setTimeStart(m_timeStart);
    data->setTimeStop(m_timeStop);
    data->setItemRoot(m_itemRoot, false);
    return data;
  }


void cs8PlotData::dumpData()
{
  qDebug() << "Plot for " << m_itemRoot->text() << " has " << size() << " items";
  for ( int i=0; i<size();i++ )
    qDebug() << i << ":   " << QDateTime::fromTime_t ( x ( i ) ) << " : " << y ( i );
}

void cs8PlotData::setItemRoot(QStandardItem* value, bool doUpdate)
{
  m_itemRoot = value;
  if (doUpdate)
  update();
}

