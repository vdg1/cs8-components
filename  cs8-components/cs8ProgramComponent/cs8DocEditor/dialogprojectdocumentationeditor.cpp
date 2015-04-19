#include "dialogprojectdocumentationeditor.h"
#include "ui_dialogprojectdocumentationeditor.h"

DialogProjectDocumentationEditor::DialogProjectDocumentationEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProjectDocumentationEditor)
{
    ui->setupUi(this);
}

DialogProjectDocumentationEditor::~DialogProjectDocumentationEditor()
{
    delete ui;
}

void DialogProjectDocumentationEditor::setText(const QString &text)
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(text);
}

QString DialogProjectDocumentationEditor::text() const
{
    return ui->plainTextEdit->toPlainText();
}

void DialogProjectDocumentationEditor::setTitle(const QString & text)
{
    setWindowTitle(text);
}
