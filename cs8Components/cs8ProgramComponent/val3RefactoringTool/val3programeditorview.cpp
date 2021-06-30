#include "val3programeditorview.h"
#include "ui_val3programeditorview.h"

val3ProgramEditorView::val3ProgramEditorView(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::val3ProgramEditorView)
{
    ui->setupUi(this);
}

val3ProgramEditorView::~val3ProgramEditorView()
{
    delete ui;
}
