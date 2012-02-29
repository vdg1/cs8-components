/*
// C++ Implementation: cs8programheaderview
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
*/
#include "cs8programheaderview.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>

cs8ProgramHeaderView::cs8ProgramHeaderView ( QWidget *parent )
    : QWidget ( parent )
{

    QFont m_font;
    m_font.setFamily("courier");
    m_font.setPointSize(11);
    m_documentation=new QPlainTextEdit(this);
    m_documentation->setWordWrapMode(QTextOption::WordWrap);
    m_documentation->document()->setTextWidth(40);
    m_documentation->setFont(m_font);

    m_description=new QLineEdit(this);
    m_description->setFont(m_font);

    QVBoxLayout *layout= new QVBoxLayout(this);
    layout->addWidget(m_description);
    layout->addWidget(m_documentation);

    setLayout(layout);

    connect(m_documentation,SIGNAL(textChanged()),this,SLOT(slotModified()));
    connect(m_description,SIGNAL(textChanged(QString)),this,SLOT(slotModified()));

}


cs8ProgramHeaderView::~cs8ProgramHeaderView()
{
}

/*!
    \fn cs8ProgramHeaderView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
 */
void cs8ProgramHeaderView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
    m_documentation->blockSignals(true);
    m_description->blockSignals(true);
    if (deselected.count()>0)
    {
        m_masterView->model()->setData(deselected.indexes().at(0),m_documentation->toPlainText(),Qt::UserRole+10);
        m_masterView->model()->setData(deselected.indexes().at(0),m_description->text(),Qt::UserRole+11);
    }
    if (selected.count()>0)
    {
        m_documentation->setPlainText(m_masterView->model()->data(selected.indexes().at(0),Qt::UserRole+10).toString());
        m_description->setText(m_masterView->model()->data(selected.indexes().at(0),Qt::UserRole+11).toString());
    }
    m_documentation->blockSignals(false);
    m_description->blockSignals(false);
}

void cs8ProgramHeaderView::slotModified()
{
    emit modified();
}


void cs8ProgramHeaderView::setMasterView ( QAbstractItemView* theValue )
{
    m_masterView = theValue;
    connect(m_masterView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,SLOT(slotSelectionChanged(const QItemSelection & , const QItemSelection &)));

}
