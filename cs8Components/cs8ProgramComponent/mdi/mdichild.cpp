/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "mdichild.h"
#include "ui_mdichild.h"

MdiChild::MdiChild(QWidget *parent)
    : QWidget(parent), ui(new Ui::val3ProgramEditorView) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  isUntitled = true;
}

bool MdiChild::save() {
  if (isUntitled) {
    return saveAs();
  } else {
    return saveFile(curFile);
  }
}

bool MdiChild::saveAs() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
  if (fileName.isEmpty())
    return false;

  return saveFile(fileName);
}

bool MdiChild::saveFile(const QString &fileName) {
  QString errorMessage;

  QGuiApplication::setOverrideCursor(Qt::WaitCursor);
  QSaveFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Text)) {
    QTextStream out(&file);
    // out << toPlainText();
    if (!file.commit()) {
      errorMessage =
          tr("Cannot write file %1:\n%2.")
              .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
  } else {
    errorMessage =
        tr("Cannot open file %1 for writing:\n%2.")
            .arg(QDir::toNativeSeparators(fileName), file.errorString());
  }
  QGuiApplication::restoreOverrideCursor();

  if (!errorMessage.isEmpty()) {
    QMessageBox::warning(this, tr("MDI"), errorMessage);
    return false;
  }

  setCurrentFile(fileName);
  return true;
}

QString MdiChild::userFriendlyCurrentFile() { return m_program->name(); }

QPlainTextEdit *MdiChild::editor() const { return ui->plainTextEditCode; }

void MdiChild::setProgram(cs8Program *program) {
  program->view = this;
  m_program = program;
  ui->plainTextEditCode->setPlainText(program->val3Code());
  ui->labelDeclaration->setText(program->definition());
  ui->tableViewPars->setModel(program->parameterModel());
  ui->tableViewVars->setModel(program->localVariableModel());
  ui->widgetDocumentation->setProgram(m_program);
  ui->widgetDocumentation->setWindowTitle(program->name() + "[*]");
  connect(program, &cs8Program::modified,
          [this]() { setWindowModified(true); });
  connect(program, &cs8Program::codeChanged,
          [=]() { ui->plainTextEditCode->setPlainText(program->val3Code()); });
  setWindowModified(false);
}

void MdiChild::closeEvent(QCloseEvent *event) {
  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MdiChild::documentWasModified() {
  // setWindowModified(m_program.isModified());
}

bool MdiChild::maybeSave() {
  if (!editor()->document()->isModified())
    return true;
  const QMessageBox::StandardButton ret = QMessageBox::warning(
      this, tr("MDI"),
      tr("'%1' has been modified.\n"
         "Do you want to save your changes?")
          .arg(userFriendlyCurrentFile()),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
  case QMessageBox::Save:
    return save();
  case QMessageBox::Cancel:
    return false;
  default:
    break;
  }
  return true;
}

void MdiChild::setCurrentFile(const QString &fileName) {
  curFile = QFileInfo(fileName).canonicalFilePath();
  isUntitled = false;
  editor()->document()->setModified(false);
  setWindowModified(false);
  setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::strippedName(const QString &fullFileName) {
  return QFileInfo(fullFileName).fileName();
}
