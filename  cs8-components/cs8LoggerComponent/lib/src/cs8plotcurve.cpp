#include "cs8plotcurve.h"
#include "cs8statisticmodel.h"
//
cs8PlotCurve::cs8PlotCurve( const QString & title )
    : QwtPlotCurve(title), m_resolution(3600), m_model(0), m_item(0)
{
  // TODO
}
//


/*!
    \fn cs8PlotCurve::attach(QStandardItem * item)
 */
/*void cs8PlotCurve::attach(QStandardItem * item)
{ 
	cs8PlotData data;
	data.setTimeStart ( m_model->timeStart() );
	data.setTimeStop ( m_model->timeStop() );
	data.setScanIntervall ( m_resolution );
	data.setItemRoot ( item );
	cs8PlotCurve* curve=new cs8PlotCurve ( item->text() );

	m_curves[item]=curve;
	curve->setData ( data );
	curve->setStyle ( QwtPlotCurve::Steps );
	curve->attach ( plotStatistic );
	QPen pen;
	pen.setColor ( ( Qt::GlobalColor ) m_curves.count() );
	curve->setPen ( pen );
	*/
//}

void cs8PlotCurve::setItem(QStandardItem* value)
{
  m_item = value;
  if (!m_model==NULL)
    update();
}


/*!
    \fn cs8PlotCurve::update()
 */
void cs8PlotCurve::update()
{
  if (m_item==NULL)
    return;

  m_data.setTimeStart ( m_model->timeStart() );
  m_data.setTimeStop ( m_model->timeStop() );
  m_data.setScanIntervall ( m_resolution );
  m_data.setItemRoot ( m_item );
  setData ( m_data );
}
