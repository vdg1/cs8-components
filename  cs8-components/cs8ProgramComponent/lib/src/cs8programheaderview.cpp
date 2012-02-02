//
// C++ Implementation: cs8programheaderview
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8programheaderview.h"

cs8ProgramHeaderView::cs8ProgramHeaderView ( QWidget *parent )
    : QTextEdit ( parent )
{
    setWordWrapMode(QTextOption::WordWrap);
    document()->setTextWidth(40);
}


cs8ProgramHeaderView::~cs8ProgramHeaderView()
{
}

/*!
    \fn cs8ProgramHeaderView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) 
 */
void cs8ProgramHeaderView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) 
{
    if (deselected.count()>0)
    {
        m_masterView->model()->setData(deselected.indexes().at(0),toPlainText(),Qt::UserRole+10);
    }
    if (selected.count()>0)
    {
        setText(m_masterView->model()->data(selected.indexes().at(0),Qt::UserRole+10).toString());
    }
}


void cs8ProgramHeaderView::setMasterView ( QAbstractItemView* theValue )
{
    m_masterView = theValue;
    connect(m_masterView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,SLOT(slotSelectionChanged(const QItemSelection & , const QItemSelection &)));

}
