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
    ui->plainTextEdit->setText(text,true);
}

QString DialogProjectDocumentationEditor::text() const
{
    return ui->plainTextEdit->text();
}

void DialogProjectDocumentationEditor::setTitle(const QString & text)
{
    setWindowTitle(text);
}
