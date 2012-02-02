#ifndef CS8STATISTICFILTERPROXY_H
#define CS8STATISTICFILTERPROXY_H
//
#include <QSortFilterProxyModel>
//
class cs8StatisticFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
  public:
    cs8StatisticFilterProxy ( QObject * parent );
    
  protected:
    bool filterAcceptsRow ( int sourceRow, const QModelIndex &sourceParent ) const;
//    bool filterAcceptsColumn ( int sourceCol, const QModelIndex &sourceParent ) const;
    
};
#endif
