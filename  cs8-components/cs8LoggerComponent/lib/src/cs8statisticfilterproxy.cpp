#include "cs8statisticfilterproxy.h"
#include "cs8statisticmodel.h"

#include <QDebug>
//
cs8StatisticFilterProxy::cs8StatisticFilterProxy ( QObject * parent )
    : QSortFilterProxyModel ( parent )
{
  // TODO
}
//
bool cs8StatisticFilterProxy::filterAcceptsRow ( int sourceRow,
    const QModelIndex &sourceParent ) const
  {
    QModelIndex index0 = sourceModel()->index ( sourceRow, 0, sourceParent );
	cs8StatisticModel* model=dynamic_cast<cs8StatisticModel*>(sourceModel());
    QStandardItem* item=model->itemFromIndex(index0);

    bool result=sourceModel()->data ( index0 ).toString().contains ( filterRegExp() )
                || (sourceParent!=model->indexFromItem(model->invisibleRootItem()));
    return ( result );//|| ((cs8StatisticModel*)sourceModel())->itemFromIndex(index0)->parent() !=((cs8StatisticModel*)sourceModel())->invisibleRootItem() ) ;
  }

/*
bool cs8StatisticFilterProxy::filterAcceptsColumn ( int sourceCol,
    const QModelIndex &sourceParent ) const
{
	qDebug() << "source col" << sourceCol;
	return sourceCol==1;
}
*/
