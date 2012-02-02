//
// C++ Interface:
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CS8LOGSTATISTICSWIDGET_H
#define CS8LOGSTATISTICSWIDGET_H

#include <QWidget>

#include "../ui_cs8logstatisticswidgetbase.h"

class cs8LoggerWidget;
class cs8StatisticFilterProxy;
class cs8StatisticModel;
class cs8LogStatisticsWidget : public QWidget, private Ui::cs8LogStatisticsWidget
  {
    Q_OBJECT
  private:
	  cs8StatisticFilterProxy* m_proxy;

  public:
    cs8LogStatisticsWidget(QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~cs8LogStatisticsWidget();
    void setLog(cs8LoggerWidget* cs8Logger);
    /*$PUBLIC_FUNCTIONS$*/

  public slots:
    /*$PUBLIC_SLOTS$*/

  protected:
    /*$PROTECTED_FUNCTIONS$*/

  protected slots:
    void slotFilterTextChanged(const QString & filterText);
    void slotDataChanged();
    void slotItemDoubleClicked(const QModelIndex& index);
    /*$PROTECTED_SLOTS$*/
protected:
    cs8StatisticModel *m_model;

signals:
    void gotoLine(int LineNumber);

  };

#endif

