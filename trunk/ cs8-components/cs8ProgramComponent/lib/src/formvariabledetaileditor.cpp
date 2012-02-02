#include "formvariabledetaileditor.h"
#include "ui_formvariabledetaileditor.h"

formVariableDetailEditor::formVariableDetailEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::formVariableDetailEditor)
{
    ui->setupUi(this);
}

formVariableDetailEditor::~formVariableDetailEditor()
{
    delete ui;
}

void formVariableDetailEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void formVariableDetailEditor::on_buttonBox_rejected()
{
    emit done();
}

void formVariableDetailEditor::setVariable(cs8Variable *var)
{
    m_variable=var;
    ui->textEditDocumentation->setPlainText(m_variable->description());
    ui->lineEditName->setText(m_variable->name());
}

void formVariableDetailEditor::on_buttonBox_accepted()
{
    if (m_variable){
        m_variable->setDescription(ui->textEditDocumentation->toPlainText());
    }
    emit done();
}
