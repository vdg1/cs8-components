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


    m_documentation=new FormMarkDownEditor(this);


    m_briefText=new QLineEdit(this);
    QFont m_font;
    m_font.setFamily("courier");
    m_font.setPointSize(11);
    m_briefText->setFont(m_font);

    QVBoxLayout *layout= new QVBoxLayout(this);
    layout->addWidget(m_briefText);
    layout->addWidget(m_documentation);

    setLayout(layout);

    connect(m_documentation,&FormMarkDownEditor::textEdited,this,&cs8ProgramHeaderView::slotModified);
    connect(m_briefText,&QLineEdit::textEdited,this,&cs8ProgramHeaderView::slotModified);

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
    m_briefText->blockSignals(true);
    if (deselected.count()>0)
    {
        m_masterView->model()->setData(deselected.indexes().at(0),m_documentation->text().trimmed(),Qt::UserRole+10);
        m_masterView->model()->setData(deselected.indexes().at(0),m_briefText->text().trimmed(),Qt::UserRole+11);
    }
    m_documentation->blockSignals(false);
    m_briefText->blockSignals(false);

    if (selected.count()>0)
    {
        m_documentation->setText(m_masterView->model()->data(selected.indexes().at(0),Qt::UserRole+10).toString(),true);
        m_briefText->setText(m_masterView->model()->data(selected.indexes().at(0),Qt::UserRole+11).toString());
    }
    m_documentation->setPrefixText(m_briefText->text());
}

void cs8ProgramHeaderView::slotModified()
{
    emit modified(true);
    m_documentation->setPrefixText(m_briefText->text());
}

FormMarkDownEditor *cs8ProgramHeaderView::documentation() const
{
    return m_documentation;
}


void cs8ProgramHeaderView::setMasterView ( QAbstractItemView* theValue )
{
    m_masterView = theValue;
    connect(m_masterView->selectionModel(),&QItemSelectionModel::selectionChanged, this,&cs8ProgramHeaderView::slotSelectionChanged);

}
