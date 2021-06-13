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

#include "mdichild.h"
#include "ui_mdichild.h"

#include <QTimer>

MdiChild::MdiChild(QWidget *parent)
    : QWidget(parent), ui(new Ui::val3ProgramEditorView) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  ui->splitter->setStretchFactor(0, 1);
  ui->splitter->setStretchFactor(1, 6);
  updateCodeModelTimer = new QTimer(this);
  updateCodeModelTimer->setSingleShot(true);
  connect(updateCodeModelTimer, &QTimer::timeout, [this]() {
    m_program->setCode(ui->plainTextEditCode->toPlainText(), true, false);
  });
}

QPlainTextEdit *MdiChild::editor() const { return ui->plainTextEditCode; }

void MdiChild::setProgram(cs8Program *program) {
  program->view = this;
  m_program = program;
  ui->plainTextEditCode->setPlainText(program->val3Code());
  ui->labelDeclaration->setText(program->definition());
  ui->tableViewPars->setModel(program->parameterModel());
  ui->tableViewVars->setModel(program->localVariableModel());
  ui->widgetDocumentation->setProgram(m_program);
  setWindowTitle(program->name() + "[*]");
  connect(program, &cs8Program::modifiedChanged,
          [this]() { setWindowModified(true); });

  connect(ui->plainTextEditCode, &CodeEditor::textChanged, [this]() {
    qDebug() << __FUNCTION__ << "text changed";
    updateCodeModelTimer->stop();
    updateCodeModelTimer->start(1000);
  });

  connect(program, &cs8Program::codeChanged,
          [=]() { ui->plainTextEditCode->setPlainText(program->val3Code()); });
  setWindowModified(program->isModified());
}

void MdiChild::closeEvent(QCloseEvent *event) { event->accept(); }

void MdiChild::documentWasModified() {
  // setWindowModified(m_program.isModified());
}

cs8Program *MdiChild::program() const { return m_program; }
