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
#include "cs8application.h"
#include "cs8programmodel.h"
#include "cs8variablemodel.h"

#include <QDebug>

cs8ProgramDataView::cs8ProgramDataView(QWidget *parent) : QTableView(parent), m_mode(Mode::GlobalData)
{
    m_globalVariableModel = new cs8VariableModel(this);
}

cs8ProgramDataView::~cs8ProgramDataView() {}

void cs8ProgramDataView::setMasterView(QAbstractItemView *theValue) {
  m_masterView = theValue;
  connect(m_masterView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &cs8ProgramDataView::slotSelectionChanged);
  if (m_mode == Mode::GlobalData) {
      auto model = (cs8Application *) m_masterView->model()->parent();
      setModel(model->globalVariableModel());
  }
}

/*!
    \fn cs8ProgramDataView::slotSelectionChanged ( const QItemSelection &
   selected, const QItemSelection & deselected )
 */
void cs8ProgramDataView::slotSelectionChanged(
    const QItemSelection &selected, const QItemSelection & /*deselected*/) {

  if (selected.count() > 0) {
    QAbstractItemModel *varModel;
    switch (m_mode) {
    case Mode::LocalData: {
        varModel = ((cs8ProgramModel *) m_masterView->model())->localVariableModel(selected.indexes().at(0));
        break;
    }
    case Mode::ParameterData: {
        varModel = ((cs8ProgramModel *) m_masterView->model())->parameterModel(selected.indexes().at(0));
        break;
    }
    case Mode::GlobalData:
        varModel = qobject_cast<cs8Application *>(m_masterView->model()->parent())->globalVariableModel();
        break;

    case Mode::ReferencedGlobalData:
        varModel = m_globalVariableModel;
        ///TODO qobject_cast<cs8ProgramModel *>(m_masterView->model())->referencedGlobalVariables(selected.indexes().at(0));
        break;
    }
    setModel(varModel);
    resizeColumnsToContents();
  }
}

cs8ProgramDataView::Mode cs8ProgramDataView::mode() const
{
    return m_mode;
}

void cs8ProgramDataView::setMode(Mode theValue) { m_mode = theValue; }
