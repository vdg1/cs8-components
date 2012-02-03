#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QFileDialog>
#include <QDebug>
#include "cs8application.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName (this,"Select Val3 Project directory","D:\\data\\Staubli\\CS8\\Development_7.2\\usr\\usrapp\\Plastic\\coreModules\\modUserDisp","*.pjx");
       if (!filePath.isEmpty ())
    {
        cs8Application app;
        app.open (filePath);
        ui->plainTextEdit->clear ();
        ui->plainTextEdit->appendPlainText (app.checkVariables ());
    }
}
