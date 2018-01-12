#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cs8application.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_app=new cs8Application(this);
    m_app->setName("Vplus");
    m_app->setProjectPath("D:\\data\\Cpp\\_SAXESwissSystem\\_Libraries\\cs8_components\\cs8_components - qt5\\cs8ProgramComponent\\importVPlus\\VPlus");
    m_app->importVPlusFile("D:\\data\\Cpp\\_SAXESwissSystem\\_Libraries\\cs8_components\\cs8_components - qt5\\cs8ProgramComponent\\importVPlus\\KOPP_6.LC");
    m_app->save();
}

MainWindow::~MainWindow()
{
    delete ui;
}
