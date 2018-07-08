#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->widget, &cs8MCPDisplay::toggle, ui->checkBox, &QCheckBox::setChecked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pbCLS_clicked() { ui->widget->cls(); }

void MainWindow::on_pbPut_clicked() {
  ui->widget->setTextMode((cs8MCPDisplay::TextMode)ui->cbMode->currentIndex());
  ui->widget->put(ui->spColumn->value(), ui->spRow->value(), ui->leText->text());
}
