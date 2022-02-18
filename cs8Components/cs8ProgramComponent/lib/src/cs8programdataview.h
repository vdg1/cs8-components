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

#include "cs8variablemodel.h"
#include <QTableView>

/**
    @author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8ProgramDataView : public QTableView {
  Q_OBJECT
  public:
  enum class Mode { GlobalData, LocalData, ParameterData, ReferencedGlobalData };

  cs8ProgramDataView(QWidget *parent = 0);

  ~cs8ProgramDataView();

  void setMasterView(QAbstractItemView *theValue);
  void setMode(Mode theValue);
  Mode mode() const;

  protected:
  QAbstractItemView *m_masterView;
  cs8VariableModel *m_globalVariableModel;
  Mode m_mode;

  protected slots:
  void slotSelectionChanged(const QItemSelection &selected,
                            const QItemSelection &);
};

#endif
