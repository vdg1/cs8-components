//
// C++ Interface: cs8programheaderview
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8PROGRAMHEADERVIEW_H
#define CS8PROGRAMHEADERVIEW_H

#include <QTextEdit>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QAbstractItemView>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8ProgramHeaderView : public QTextEdit
{
		Q_OBJECT
	public:
		cs8ProgramHeaderView ( QWidget *parent = 0 );

		~cs8ProgramHeaderView();

	void setMasterView ( QAbstractItemView* theValue );
	


	protected slots:
    void slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) ;

		
	protected:
    QAbstractItemView* m_masterView;
};

#endif
