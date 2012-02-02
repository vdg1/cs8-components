//
// C++ Interface: cs8statisticmodel
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8STATISTICMODEL_H
#define CS8STATISTICMODEL_H

#include <QStandardItemModel>
#include <QTimer>
#include <QDateTime>
#include <QStack>
#include <QHash>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/


class cs8LogEntry
  {
  public:
    cs8LogEntry ( const QString& lineText_,int lineNumber_ );
    QString lineText;
    int lineNumber;
  };

class cs8StatisticModel : public QStandardItemModel
  {
    Q_OBJECT
public slots:
	void togglePlot(const QString & message);
	void clear();

  protected:
    void setup();

  private:
	QHash<QString,QStandardItem*> m_itemHash;
	bool m_firstEntry;
    QTimer* m_timer;
    QStack<cs8LogEntry*> m_queue;
    void addStatisticInfo_ ( const QString & line, int lineNumber );

  public:
    void addStatisticInfo ( const QString & line, int lineNumber );
    cs8StatisticModel ( QObject * parent = 0 );
    ~cs8StatisticModel();

	QDateTime timeStart() const
	  {
	    return m_timeStart;
	  }

	QDateTime timeStop() const
	  {
	    return m_timeStop;
	  }
	
	

  protected slots:
    void slotUpdate();
    void addEntry ( const QString & text, const QDateTime & when, int line );
protected:
    QDateTime m_timeStop;
    QDateTime m_timeStart;
  };

#endif
