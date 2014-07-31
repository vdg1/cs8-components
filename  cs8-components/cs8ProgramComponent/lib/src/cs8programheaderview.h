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


#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QAbstractItemView>

/**
    @author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/

class QPlainTextEdit;
class QLineEdit;

class cs8ProgramHeaderView : public QWidget
{
    Q_OBJECT
public:
    cs8ProgramHeaderView ( QWidget *parent = 0 );

    ~cs8ProgramHeaderView();

    void setMasterView ( QAbstractItemView* theValue );



protected slots:
    void slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) ;
    void slotModified();

signals:
    void modified(bool);

protected:
    QAbstractItemView* m_masterView;
    QPlainTextEdit *m_documentation;
    QLineEdit *m_description;
};

#endif
