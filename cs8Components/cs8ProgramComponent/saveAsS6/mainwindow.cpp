#include "mainwindow.h"
#include "cs8application.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  // ui->leOutputDir->setText(QStandardPaths::writableLocation((QStandardPaths::DocumentsLocation)));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_buttonBox_accepted() {
  cs8Application sourceApp;
  if (sourceApp.open(ui->leApplication->text())) {
    sourceApp.save(ui->leOutputDir->text() + "/" + sourceApp.name() + "/", sourceApp.name(), true);

    /*
     * foreach (auto library, sourceApp.libraryModel()->list()) {
      cs8Application lib;
      lib.setCellPath(sourceApp.cellPath() + "/usr/usrapp");
      lib.open(library->path());
      lib.setCellPath(ui->leOutputDir->text() + "/");
      lib.save("", "", true);
    }
    */

    // scan folders?
    if (ui->cbScanFolders->isChecked()) {
      QFileInfo fileInfo(ui->leApplication->text());
      QDir dir(fileInfo.absolutePath());
      QFileInfoList dirList = dir.entryInfoList();
      foreach (auto dir, dirList) {
        cs8Application subApp;
        subApp.openFromPathName(dir.absoluteFilePath());
        subApp.save(ui->leOutputDir->text() + "/" + sourceApp.name() + "/" + subApp.name() + "/", subApp.name(), true);
      }
    }
  }
}

void MainWindow::on_tbApplication_clicked() {
  QString appPath;
  appPath = QFileDialog::getOpenFileName(this, tr("Select Val3 Application"), ui->leApplication->text(), "*.pjx");
  if (!appPath.isEmpty()) {
    ui->leApplication->setText(appPath);
  }
}

void MainWindow::on_tbOutputDir_clicked() {
  QString appPath;
  appPath = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"), ui->leOutputDir->text());
  if (!appPath.isEmpty()) {
    ui->leOutputDir->setText(appPath);
  }
}
