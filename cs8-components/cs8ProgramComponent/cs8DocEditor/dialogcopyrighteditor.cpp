#include "dialogcopyrighteditor.h"
#include "ui_dialogcopyrighteditor.h"

DialogCopyRightEditor::DialogCopyRightEditor(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogCopyRightEditor) {
  ui->setupUi(this);
}

DialogCopyRightEditor::~DialogCopyRightEditor() { delete ui; }

void DialogCopyRightEditor::changeEvent(QEvent *e) {
  QDialog::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}
QString DialogCopyRightEditor::copyRightText() const {
  return ui->plainTextEdit->toPlainText();
}

void DialogCopyRightEditor::setCopyRightText(const QString &copyRightText) {
  ui->plainTextEdit->setPlainText(copyRightText);
}
