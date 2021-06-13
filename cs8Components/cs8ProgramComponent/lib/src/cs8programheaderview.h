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

#include "formmarkdowneditor.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QPointer>

/**
    @author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/

class QPlainTextEdit;
class QLineEdit;
class cs8Program;

class FocusWatcher : public QObject {
  Q_OBJECT
public:
  explicit FocusWatcher(QObject *parent = nullptr);
  virtual bool eventFilter(QObject *obj, QEvent *event) override;
Q_SIGNALS:
  void focusChanged(bool in);
};

class cs8ProgramHeaderView : public QWidget {
  Q_OBJECT
public:
  cs8ProgramHeaderView(QWidget *parent = 0);

  ~cs8ProgramHeaderView();

  void setMasterView(QAbstractItemView *theValue);
  void setProgram(cs8Program *program);

  QTextEdit *documentation() const;

protected slots:
  void slotSelectionChanged(const QItemSelection &selected,
                            const QItemSelection &deselected);
  void slotModified();

protected:
  QAbstractItemView *m_masterView;
  QTextEdit *m_documentation;
  QLineEdit *m_briefText;
  QPointer<cs8Program> m_program;
};

#endif
