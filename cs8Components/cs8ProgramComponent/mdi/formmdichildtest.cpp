#include "formmdichildtest.h"
#include "ui_formmdichildtest.h"

FormMdiChildTest::FormMdiChildTest(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::FormMdiChildTest)
{
    ui->setupUi(this);
}

FormMdiChildTest::~FormMdiChildTest()
{
    delete ui;
}
