//
// C++ Implementation: cs8programdataview
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8programdataview.h"
#include "cs8variablemodel.h"
#include "cs8programmodel.h"

#include <QDebug>

cs8ProgramDataView::cs8ProgramDataView ( QWidget* parent )
		: QTableView ( parent ), m_mode ( false )
{
}


cs8ProgramDataView::~cs8ProgramDataView()
{
}




void cs8ProgramDataView::setMasterView ( QAbstractItemView* theValue )
{
	m_masterView = theValue;
	connect ( m_masterView->selectionModel(),SIGNAL ( selectionChanged ( const QItemSelection &, const QItemSelection & ) ),
	          this,SLOT ( slotSelectionChanged ( const QItemSelection & , const QItemSelection & ) ) );
}


/*!
    \fn cs8ProgramDataView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
 */
void cs8ProgramDataView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
	if ( deselected.count() >0 )
	{

	}
	if ( selected.count() >0 )
	{
		QAbstractItemModel* varModel;
		if ( m_mode )
			varModel= ( ( cs8ProgramModel* ) m_masterView->model() )->localVariableModel ( selected.indexes().at ( 0 ) );
		else
			varModel= ( ( cs8ProgramModel* ) m_masterView->model() )->parameterModel ( selected.indexes().at ( 0 ) );
		setModel ( varModel );
		resizeColumnsToContents();
	}
}


bool cs8ProgramDataView::mode() const
{
	return m_mode;
}


void cs8ProgramDataView::setMode ( bool theValue )
{
	m_mode = theValue;
}
