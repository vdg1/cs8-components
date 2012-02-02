//
// C++ Implementation:
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "../src/cs8logstatisticswidget.h"
#include "cs8loggerwidget.h"
#include "cs8statisticfilterproxy.h"
#include "cs8statisticmodel.h"

#include <QDebug>
#include <QStandardItem>

cs8LogStatisticsWidget::cs8LogStatisticsWidget ( QWidget* parent, Qt::WFlags fl )
    : QWidget ( parent, fl ), Ui::cs8LogStatisticsWidget()
{
  setupUi ( this ); 
  m_proxy = new cs8StatisticFilterProxy ( this );
// m_proxy->setFilterKeyColumn ( 0 );
  m_proxy->setDynamicSortFilter(true);
  tvStatistics->setModel ( m_proxy );
  connect ( cbFilter,SIGNAL ( editTextChanged ( const QString& ) ),
            this,SLOT ( slotFilterTextChanged ( const QString & ) ) );
  connect(tvStatistics, SIGNAL(doubleClicked(const QModelIndex &)),
          this,SLOT(slotItemDoubleClicked(const QModelIndex &)));
}

cs8LogStatisticsWidget::~cs8LogStatisticsWidget()
{}

/*$SPECIALIZATION$*/




/*!
    \fn cs8LogStatisticsWidget::setLog(cs8LoggerWidget* cs8Logger)
 */
void cs8LogStatisticsWidget::setLog ( cs8LoggerWidget* cs8Logger )
{
	
  m_model=  cs8Logger->model();
  m_proxy->setSourceModel ( m_model );
  connect ( m_model ,SIGNAL(dataChanged( const QModelIndex & , const QModelIndex & )),
            this,SLOT(slotDataChanged( )));
  connect ( m_model ,SIGNAL(rowsInserted ( const QModelIndex & , int , int  ) ),
           this,SLOT(slotDataChanged( )));
  connect(this,SIGNAL(gotoLine(int)),cs8Logger,SLOT(gotoLine(int)));
}

void cs8LogStatisticsWidget::slotFilterTextChanged ( const QString & filterText )
{
  m_proxy->setFilterRegExp ( QRegExp ( filterText, Qt::CaseInsensitive, QRegExp::FixedString ) );
}


/*!
    \fn cs8LogStatisticsWidget::slotDataChanged()
 */
void cs8LogStatisticsWidget::slotDataChanged()
{
  tvStatistics->resizeColumnToContents(0);
}


/*!
    \fn cs8LogStatisticsWidget::slotItemDoubleClicked(const QModelIndex* index)
 */
void cs8LogStatisticsWidget::slotItemDoubleClicked(const QModelIndex& index)
{
  QStandardItem* item=m_model->itemFromIndex(m_proxy->mapToSource(index));
  int lineNumber;
  //if ((item->parent()==m_model->invisibleRootItem()) && item->hasChildren())
  //  lineNumber=item->child(0,0)->data().toInt();
  //else
  lineNumber=item->data().toInt();
  qDebug() << "select line number: " << lineNumber;
  if (lineNumber>0)
    emit gotoLine(lineNumber);
}
