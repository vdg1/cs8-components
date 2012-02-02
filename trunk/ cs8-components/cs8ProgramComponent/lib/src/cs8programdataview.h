//
// C++ Interface: cs8programdataview
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8PROGRAMDATAVIEW_H
#define CS8PROGRAMDATAVIEW_H

#include <QTableView>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8ProgramDataView : public QTableView
{
		Q_OBJECT
	public:
		cs8ProgramDataView ( QWidget * parent=0 );

		~cs8ProgramDataView();

		void setMasterView ( QAbstractItemView* theValue );

	void setMode ( bool theValue );
	

	bool mode() const;
	


	protected:
		QAbstractItemView* m_masterView;
    bool m_mode;

	protected slots:
		void slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
};

#endif
