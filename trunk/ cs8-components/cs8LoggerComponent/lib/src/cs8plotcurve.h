#ifndef CS8PLOTCURVE_H
#define CS8PLOTCURVE_H
//
#include <qwt_plot_curve.h>
#include "cs8plotdata.h"
//

class cs8StatisticModel;

class cs8PlotCurve : public QwtPlotCurve
{
  private:
  public:
    cs8PlotCurve ( const QString & title );

	void setItem(QStandardItem* value);


	QStandardItem* item() const
	  {
	    return m_item;
	  }
	
    //void attach(QStandardItem * item);

	void setModel(cs8StatisticModel* value)
	{
	  m_model = value;
	}

	void setResolution(int value)
	{
	  m_resolution = value;
	}
    void update();
	
	
    
protected:
    QStandardItem* m_item;
    cs8StatisticModel* m_model;
    int m_resolution;
    cs8PlotData m_data;
};
#endif
