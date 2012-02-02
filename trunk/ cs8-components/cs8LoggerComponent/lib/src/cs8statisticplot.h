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

#ifndef CS8STATISTICPLOT_H
#define CS8STATISTICPLOT_H

#include <QWidget>
#include <QDateTime>
#include <QHash>
#include "../ui_cs8plotstatisticswidgetbase.h"
#include "cs8statisticmodel.h"

class cs8PlotCurve;   
class cs8LoggerWidget;
class QStandardItem;
class QwtPlotZoomer;
class QwtPlotMarker;
class QwtPlotPanner;
class QwtPlotMagnifier;

class cs8PlotStatisticWidget : public QWidget, private Ui::cs8StatisticPlot
{
    Q_OBJECT
  private:
	QwtPlotMagnifier* m_magnifier;
	QwtPlotPanner* m_panner;
    QHash<QString, QwtPlotMarker*> m_markers;
    QwtPlotZoomer* m_zoomer;
    
  public:
    cs8PlotStatisticWidget ( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~cs8PlotStatisticWidget();
    QDateTime timeEnd() const
    {
      return m_timeEnd;
    }
    
    
    /*$PUBLIC_FUNCTIONS$*/
    
    QDateTime timeStart() const
    {
      return m_timeStart;
    }
    void addCurve ( QStandardItem *item );
    void removeCurve ( QStandardItem* item );
    void clear();
    void setLog ( cs8LoggerWidget* cs8Logger );
    void update();
    
    
  public slots:
    void setTimeStampMark ( QDateTime timeStamp, bool set );
    void setCurrentTimeStampMarker ( QDateTime timeStamp );
    /*$PUBLIC_SLOTS$*/
    
  protected:
    /*$PROTECTED_FUNCTIONS$*/
    
  protected slots:
    void setModel ( cs8StatisticModel* value );
	void slotResolutionSlider(int value);
	/*$PROTECTED_SLOTS$*/
    
  protected:
    cs8StatisticModel *m_model;
    QDateTime m_timeStart;
    QHash<QStandardItem*,cs8PlotCurve*> m_curves;
    QDateTime m_timeEnd;
    int m_resolution;
    QwtPlotMarker* m_currentTimeStampMarker;
    QwtText y_legendText;
    
  protected slots:
    void slotDataChanged ( QStandardItem *item );
};

#endif

