#ifndef CS8PLOTDATA_H
#define CS8PLOTDATA_H
//
#include <qwt_data.h>
#include <QStandardItem>
#include <QDateTime>
#include <QHash>
//
class cs8PlotData : public QwtData
{
protected slots:
	void dumpData();

private:
	QHash<int,int> m_dataY;
	QHash<int,QDateTime> m_dataX;
	QDateTime m_timeStop;
	QDateTime m_timeStart;
	int m_size;
	int m_scanIntervall;
	QStandardItem *m_itemRoot;
public:
	cs8PlotData *copy() const;
	virtual double x(size_t i) const;
	virtual double y(size_t i) const;
	void setTimeStop(QDateTime value) { m_timeStop = value; }
	void setTimeStart(QDateTime value) { m_timeStart = value; }
	void update();
	void setScanIntervall(int value) { m_scanIntervall = value; }
	int scanIntervall() { return m_scanIntervall; }
	virtual size_t size() const;
	void setItemRoot(QStandardItem* value, bool update=true);
	cs8PlotData();	

	void setDataX(const QHash< int, QDateTime >& value)
	{
	  m_dataX = value;
	}

	void setDataY(const QHash< int, int >& value)
	{
	  m_dataY = value;
	}

	void setSize(int value)
	{
	  m_size = value;
	}
	
	protected:
//	 cs8PlotData &operator=(const cs8PlotData &);
	
};
#endif
