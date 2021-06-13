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
#include "cs8program.h"

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QVBoxLayout>

FocusWatcher::FocusWatcher(QObject *parent) : QObject(parent) {
  if (parent)
    parent->installEventFilter(this);
}
bool FocusWatcher::eventFilter(QObject *obj, QEvent *event) {
  Q_UNUSED(obj)
  if (event->type() == QEvent::FocusIn)
    emit focusChanged(true);
  else if (event->type() == QEvent::FocusOut)
    emit focusChanged(false);

  return false;
}

cs8ProgramHeaderView::cs8ProgramHeaderView(QWidget *parent)
    : QWidget(parent), m_program(nullptr) {

  m_documentation = new QTextEdit(this);

  m_briefText = new QLineEdit(this);
  QFont m_font;
  m_font.setFamily("courier");
  m_font.setPointSize(11);
  m_briefText->setFont(m_font);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_briefText);
  layout->addWidget(m_documentation);

  setLayout(layout);

  connect(m_documentation, &QTextEdit::textChanged, this,
          &cs8ProgramHeaderView::slotModified);
  connect(m_briefText, &QLineEdit::textEdited, this,
          &cs8ProgramHeaderView::slotModified);
}

cs8ProgramHeaderView::~cs8ProgramHeaderView() {}

/*!
    \fn cs8ProgramHeaderView::slotSelectionChanged ( const QItemSelection &
   selected, const QItemSelection & deselected )
 */
void cs8ProgramHeaderView::slotSelectionChanged(
    const QItemSelection &selected, const QItemSelection &deselected) {
  m_documentation->blockSignals(true);
  m_briefText->blockSignals(true);
  if (deselected.count() > 0) {
    m_masterView->model()->setData(deselected.indexes().at(0),
                                   m_documentation->toMarkdown().trimmed(),
                                   Qt::UserRole + 10);
    m_masterView->model()->setData(deselected.indexes().at(0),
                                   m_briefText->text().trimmed(),
                                   Qt::UserRole + 11);
  }
  m_documentation->blockSignals(false);
  m_briefText->blockSignals(false);

  if (selected.count() > 0) {
    m_documentation->setMarkdown(
        m_masterView->model()
            ->data(selected.indexes().at(0), Qt::UserRole + 10)
            .toString());
    m_briefText->setText(m_masterView->model()
                             ->data(selected.indexes().at(0), Qt::UserRole + 11)
                             .toString());
  }
}

void cs8ProgramHeaderView::slotModified() {
  // m_documentation->setPrefixText(m_briefText->text());
}

QTextEdit *cs8ProgramHeaderView::documentation() const {
  return m_documentation;
}

void cs8ProgramHeaderView::setMasterView(QAbstractItemView *theValue) {
  m_masterView = theValue;
  connect(m_masterView->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &cs8ProgramHeaderView::slotSelectionChanged);
}

void cs8ProgramHeaderView::setProgram(cs8Program *program) {
  m_program = program;
  m_documentation->setMarkdown(m_program->detailedDocumentation());
  m_briefText->setText(m_program->briefDescription(true));
  /*
  connect(m_documentation, &FormMarkDownEditor::textEdited,
          [=](const QString &t) {
            qDebug() << __FUNCTION__ << "detailed doc changed";
            m_program->setDetailedDocumentation(t, true);
          });
  */
  connect(new FocusWatcher(m_documentation), &FocusWatcher::focusChanged,
          [=](bool gotFocus) {
            qDebug() << __FUNCTION__ << "detailed doc changed";
            if (!gotFocus)
              m_program->setDetailedDocumentation(m_documentation->toMarkdown(),
                                                  true);
          });

  connect(new FocusWatcher(m_briefText), &FocusWatcher::focusChanged,
          [=](bool gotFocus) {
            qDebug() << __FUNCTION__ << "brief doc changed";
            if (!gotFocus)
              m_program->setBriefDescription(m_briefText->text(), true);
          });
  /*
  connect(m_briefText, &QLineEdit::textEdited, [=](const QString &t) {
      qDebug() << __FUNCTION__ << "brief doc changed";
      m_program->setBriefDescription(t, true);
  });
  */

  connect(m_program, &cs8Program::codeChanged, [=]() {
    // qDebug() << __FUNCTION__ << "Update docs from changes in code";
    // m_documentation->setText(m_program->detailedDocumentation(), true);
    // m_briefText->setText(m_program->briefDescription(true));
  });
  connect(m_program, &cs8Program::briefDescriptionChanged, [=]() {
    qDebug() << __FUNCTION__ << "Update brief docs from changes in code";
    m_briefText->setText(m_program->briefDescription(true));
  });
  connect(m_program, &cs8Program::detailedDescriptionChanged, [=]() {
    qDebug() << __FUNCTION__ << "Update detailed docs from changes in code";
    m_documentation->setMarkdown(m_program->detailedDocumentation());
  });
}
