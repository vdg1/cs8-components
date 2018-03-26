#include "mainwindow.h"
#include "cs8application.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_app = new cs8Application(this);
  m_app->setName("Vplus");
  m_app->setProjectPath("D:/data/Cpp/_SAXESwissSystem/_Libraries/cs8Components/"
                        "cs8Components/cs8ProgramComponent/importVPlus/VPlus");
  m_app->importVPlusFile("D:/data/Cpp/_SAXESwissSystem/_Libraries/"
                         "cs8Components/cs8Components/cs8ProgramComponent/"
                         "importVPlus/VPlus/ADEPT004.LC");
  m_app->save();
}

MainWindow::~MainWindow() { delete ui; }
