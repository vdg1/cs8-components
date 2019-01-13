#include "mainwindow.h"
#include "cs8filebrowsermodel.h"
#include "ui_mainwindow.h"

#include <QDir>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_localBrowser = new cs8FileBrowserModel(
      QUrl(QDir::fromNativeSeparators(
          "D:\\data\\Staubli\\SRS\\_Development\\Dev_SaxeAutomation_SRS2014\\Dev_SaxeAutomation")),
      this);
  ui->tvLocal->setModel(m_localBrowser);
  m_remoteBrowser = new cs8FileBrowserModel(QUrl("ftp://maintenance:spec_cal@localhost"), this);
  ui->tvRemote->setModel(m_remoteBrowser);
}

MainWindow::~MainWindow() { delete ui; }
