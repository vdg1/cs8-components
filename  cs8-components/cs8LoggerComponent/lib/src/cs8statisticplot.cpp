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


#include "cs8statisticplot.h"
#include "cs8plotdata.h"
#include "cs8plotcurve.h"
#include "cs8loggerwidget.h"
#include <qwt_plot_layout.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_marker.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <QTime>
#include <QDebug>


class TimeScaleDraw: public QwtScaleDraw
  {
  public:
    TimeScaleDraw ( const QDateTime &base ) :
        baseTime ( base )
    {}
    virtual QwtText label ( double v ) const
      {
        QDateTime upTime = QDateTime::fromTime_t ( ( uint ) v );
        return upTime.toString ( "yy-MM-dd\nhh:mm:ss" );
      }
  private:
    QDateTime baseTime;
  };


cs8PlotStatisticWidget::cs8PlotStatisticWidget ( QWidget* parent, Qt::WFlags fl )
    : QWidget ( parent, fl ), Ui::cs8StatisticPlot(),m_resolution(3600)
{
  setupUi ( this );

  //plotStatistic->setAutoReplot ( false );
  plotStatistic->plotLayout()->setAlignCanvasToScales ( true );

  QFont font;
  font.setFamily ( "courier" );
  font.setPointSize ( 8 );
  QwtLegend *legend = new QwtLegend;
  legend->setItemMode ( QwtLegend::CheckableItem );
  plotStatistic->insertLegend ( legend, QwtPlot::RightLegend );

  QwtText legendText;
  legendText.setFont ( font );
  legendText.setText ( " Log Time [h:m:s]" );
  plotStatistic->setAxisTitle ( QwtPlot::xBottom,  legendText );
  plotStatistic->setAxisScaleDraw ( QwtPlot::xBottom,
                                    new TimeScaleDraw ( QDateTime::currentDateTime() ) );
  plotStatistic->setAxisScale ( QwtPlot::xBottom, 0, 0 );
  plotStatistic->setAxisFont ( QwtPlot::xBottom,font );
  plotStatistic->setAxisLabelRotation ( QwtPlot::xBottom, -50.0 );
  plotStatistic->setAxisLabelAlignment ( QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom );

  y_legendText.setText ( "Occurences [min-1]" );
  plotStatistic->setAxisTitle ( QwtPlot::yLeft, y_legendText );
  plotStatistic->setAxisFont ( QwtPlot::yLeft,font );
// plotStatistic->setAxisScale ( QwtPlot::yLeft, 0, 5 );
  plotStatistic->setAxisAutoScale ( QwtPlot::yLeft );

  //m_zoomer=new QwtPlotZoomer ( plotStatistic->canvas() );
  m_panner=new QwtPlotPanner ( plotStatistic->canvas() );
  m_panner->setAxisEnabled(QwtPlot::yLeft,false);

  m_magnifier=new QwtPlotMagnifier ( plotStatistic->canvas() );
  m_magnifier->setAxisEnabled(QwtPlot::yLeft,false);

  m_currentTimeStampMarker=new QwtPlotMarker();
  QPen pen;
  pen.setColor ( Qt::red );
  m_currentTimeStampMarker->setLinePen ( pen );
  m_currentTimeStampMarker->setLineStyle ( QwtPlotMarker::VLine );
  m_currentTimeStampMarker->attach ( plotStatistic );
 
  m_currentTimeStampMarker->setLabelAlignment ( Qt::AlignTop );

  connect (slResolution,SIGNAL(valueChanged(int)),this, SLOT(slotResolutionSlider(int)));
}

cs8PlotStatisticWidget::~cs8PlotStatisticWidget()
{}

/*$SPECIALIZATION$*/

void cs8PlotStatisticWidget::setModel ( cs8StatisticModel* value )
{
  m_model = value;
  connect ( m_model,SIGNAL ( itemChanged ( QStandardItem* ) ),this,SLOT ( slotDataChanged ( QStandardItem * ) ) );
}


/*!
    \fn cs8PlotStatisticWidget::slotDataChanged(QStandardItem *item)
 */
void cs8PlotStatisticWidget::slotDataChanged ( QStandardItem *item )
{
  if ( item->checkState() ==Qt::Checked )
    {
      //qDebug() << "Add item to plot";
      addCurve ( item );
    }
  if ( item->checkState() ==Qt::Unchecked )
    {
      //qDebug() << "Remove item from plot";
      removeCurve ( item );
    }
}


/*!
    \fn cs8PlotStatisticWidget::addCurve(QStandardItem *item)
 */
void cs8PlotStatisticWidget::addCurve ( QStandardItem *item )
{
  if ( !m_curves.contains ( item ) )
    {
      cs8PlotCurve* curve=new cs8PlotCurve ( item->text() );
      m_curves[item]=curve;
	  curve->setResolution(m_resolution);
	  curve->setStyle ( QwtPlotCurve::Steps );
      curve->attach ( plotStatistic );
	  curve->setModel(m_model);
	  curve->setItem(item);
      QPen pen;
      pen.setColor ( ( Qt::GlobalColor ) m_curves.count() );
      curve->setPen ( pen );
      plotStatistic->setAxisScale ( QwtPlot::xBottom,
                                    m_model->timeStart().toTime_t(),
                                    m_model->timeStop().toTime_t() );

      plotStatistic->replot();
    }
  else
    {
      cs8PlotCurve* curve=m_curves.value ( item );
      curve->update();
      
      plotStatistic->setAxisScale ( QwtPlot::xBottom,
                                    m_model->timeStart().toTime_t(),
                                    m_model->timeStop().toTime_t() );

      
      plotStatistic->replot();

    }

}


/*!
    \fn cs8PlotStatisticWidget::removeCurve(QStandardItem* item)
 */
void cs8PlotStatisticWidget::removeCurve ( QStandardItem* item )
{
  if ( m_curves.contains ( item ) )
    {
      QwtPlotCurve* curve=m_curves.take ( item );
      curve->detach();
      delete curve;
      plotStatistic->replot();
    }
}


/*!
    \fn cs8PlotStatisticWidget::clear()
 */
void cs8PlotStatisticWidget::clear()
{
  QHashIterator<QStandardItem*,cs8PlotCurve*> i ( m_curves );
  while ( i.hasNext() )
    {
      i.next();
      QwtPlotCurve* curve=i.value();
      curve->detach();
    }
  m_curves.clear();
  QHashIterator<QString,QwtPlotMarker*> j ( m_markers );
  while ( j.hasNext() )
    {
      j.next();
      QwtPlotMarker* marker=j.value();
      marker->detach();
    }
  m_markers.clear();
  plotStatistic->replot();

}

void cs8PlotStatisticWidget::setTimeStampMark ( QDateTime timeStamp, bool set )
{
  if ( set )
    {
      qDebug() <<" set time mark";
      QwtPlotMarker* marker=new QwtPlotMarker();
      QPen pen;
      pen.setColor ( Qt::lightGray );
      marker->setLinePen ( pen );
      marker->setXValue ( timeStamp.toTime_t() );
      marker->setLineStyle ( QwtPlotMarker::VLine );
      marker->attach ( plotStatistic );
      marker->setLabel ( timeStamp.toString ( "hh:mm:ss" ) );
      marker->setLabelAlignment ( Qt::AlignTop );
      plotStatistic->replot();
      m_markers.insertMulti ( timeStamp.toString(),marker );
    }
  else
    {
      if ( m_markers.contains ( timeStamp.toString() ) )
        {
          qDebug() <<" remove time mark";
          QwtPlotMarker* marker=m_markers.take ( timeStamp.toString() );
          marker->detach();
          delete marker;
        }
      plotStatistic->replot();
    }
}


void cs8PlotStatisticWidget::setCurrentTimeStampMarker ( QDateTime timeStamp )
{
  if ( timeStamp.isValid() )
    {
      m_currentTimeStampMarker->setXValue ( timeStamp.toTime_t() );
      m_currentTimeStampMarker->setLabel ( timeStamp.toString ( "hh:mm:ss" ) );
      plotStatistic->replot();
    }
}



void cs8PlotStatisticWidget::setLog ( cs8LoggerWidget* cs8Logger )
{
  setModel ( cs8Logger->model() );
  connect ( cs8Logger,SIGNAL ( markTimeStamp ( QDateTime, bool ) ),
            this,SLOT ( setTimeStampMark ( QDateTime, bool ) ) );
  connect ( cs8Logger,SIGNAL ( markTimeStamp ( QDateTime, bool ) ),
            this,SLOT ( setTimeStampMark ( QDateTime, bool ) ) );
  connect ( cs8Logger,SIGNAL ( currentTimeStampChanged ( QDateTime ) ),
            this,SLOT ( setCurrentTimeStampMarker ( QDateTime ) ) );
  connect (cs8Logger,SIGNAL ( togglePlotMessage ( const QString & ) ),
           m_model,SLOT ( togglePlot ( const QString & ) ) );
}



/*!
    \fn cs8PlotStatisticWidget::slotResolutionSlider(int value)
 */
void cs8PlotStatisticWidget::slotResolutionSlider(int value)
{
  switch ( value )
    {
    case 1:
      m_resolution=60;
	  y_legendText.setText ( "Occurences [sec-1]" );
      break;

    case 2:
      m_resolution=3600;
	  y_legendText.setText ( "Occurences [min-1]" );
      break;

    case 3:
      m_resolution=3600*24;
	  y_legendText.setText ( "Occurences [day-1]" );
      break;
    }
	plotStatistic->setAxisTitle ( QwtPlot::yLeft, y_legendText );
  update();
}


/*!
    \fn cs8PlotStatisticWidget::update()
 */
void cs8PlotStatisticWidget::update()
{
  QHashIterator<QStandardItem*,cs8PlotCurve*> i(m_curves);
  while (i.hasNext())
    {
      i.next();
      cs8PlotCurve* curve=i.value();
	  curve->setResolution(m_resolution);
      curve->update();
    }
  plotStatistic->replot();
}
