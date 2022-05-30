#include "mainwindow.h"
#include "../common/versionInfo.h"
#include "dialogdeploy.h"
#include "ui_mainwindow.h"
#include "val3PrecompilerSettings.h"
#include "windows.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>
#include <chmxx.h>

using namespace std;
using namespace chm;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_autoExit(0)
{
    ui->setupUi(this);

    m_group = new QActionGroup(this);
    loadSettings();

    getProductName(QCoreApplication::applicationDirPath() + "/preCompilerPackage/" + VAL3CHECK,
                   m_productName,
                   m_linterVersion);

    fillTableView();

    // check arguments
    qDebug() << "arguments:" << qApp->arguments();
    if (qApp->arguments().contains("--install")) {
        QTimer::singleShot(0, this, SLOT(slotStartup()));
    }

    else if (qApp->arguments().contains("--uninstall")) {
        QTimer::singleShot(0, this, SLOT(uninstallAll()));
    }

    if (qApp->arguments().contains("--exitWhenReady")) {
        m_autoExit = true;
    }

    if (qApp->arguments().count() == 1) {
        QTimer::singleShot(0, this, [this]() { checkUpdateLinter(); });
    }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::fillTableView() {
  QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Wow6432Node\\Staubli\\CS8",
                     QSettings::NativeFormat);

  ui->tableWidget->blockSignals(true);
  ui->tableWidget->setRowCount(0);
  ui->tableWidget->setSortingEnabled(false);
  ui->tableWidget->hideColumn(1);
  ui->tableWidget->hideColumn(3);

  foreach (QString path, settings.allKeys()) {
    if ((path.contains("/s7.") || path.contains("/s8.")) &&
        path.endsWith("/path")) {
      QString systemPath = settings.value(path).toString();
      QString val3Version = systemPath.split("\\").last();
      QString fileVersion;
      QString productName;
      if (getProductName(systemPath + "\\VAL3Check.exe", productName,
                         fileVersion)) {
        if (productName.isEmpty())
          productName = QString("Staubli Val3 Check");
        else
          productName = QString("SAXE Linter & Staubli Val3 Check");

        bool activePrecompiler =
            productName.startsWith("Val3") || productName.startsWith("SAXE");
        if (!activePrecompiler)
          fileVersion = "";

        chmfile chm(QString(systemPath + "\\etc\\val3_en.chm")
                        .toLocal8Bit()
                        .constData());
        QString helpfileTitle = QString::fromUtf8(chm.get_title().c_str());
        bool activeHelpFile = helpfileTitle.contains(("SAXE"));

        qDebug() << path << settings.value(path).toString();
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0, 0, new QTableWidgetItem());
        ui->tableWidget->item(0, 0)->setCheckState(
            activePrecompiler ? Qt::Checked : Qt::Unchecked);
        ui->tableWidget->item(0, 0)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->item(0, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(0, 0)->setToolTip(
            tr("Double click to activate or deactivate"));

        ui->tableWidget->setItem(0, 1, new QTableWidgetItem());
        ui->tableWidget->item(0, 1)->setCheckState(
            activeHelpFile ? Qt::Checked : Qt::Unchecked);
        ui->tableWidget->item(0, 1)->setFlags(0);
        ui->tableWidget->item(0, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(0, 1)->setToolTip(
            tr("Double click to activate or deactivate"));

        ui->tableWidget->item(0, 0)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(0, 3, new QTableWidgetItem(systemPath));
        ui->tableWidget->item(0, 3)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(0, 2, new QTableWidgetItem(val3Version));
        ui->tableWidget->item(0, 2)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(0, 4, new QTableWidgetItem(productName));
        ui->tableWidget->item(0, 4)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(0, 5, new QTableWidgetItem(fileVersion));
        ui->tableWidget->item(0, 5)->setFlags(Qt::ItemIsEnabled);
        ui->tableWidget->setItem(0, 6, new QTableWidgetItem());
        ui->tableWidget->item(0, 6)->setFlags(0);
        bool outDated = !ui->tableWidget->item(0, 5)->text().isEmpty() &&
                        (ui->tableWidget->item(0, 5)->text() < m_linterVersion);
        ui->tableWidget->item(0, 6)->setCheckState(outDated ? Qt::Checked
                                                            : Qt::Unchecked);
      }
    }
  }
  ui->tableWidget->blockSignals(false);

  ui->tableWidget->sortItems(2, Qt::DescendingOrder);
  ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::processActivation(const QModelIndex &index) {
  if (!index.isValid()) // || index.column()!=0 || index.column()!=1)
    return;

  QString path = ui->tableWidget->item(index.row(), 3)->text();
  if (index.column() == 0) {
    bool active =
        ui->tableWidget->item(index.row(), 0)->checkState() == Qt::Checked;
    if (active) {
      if (QMessageBox::question(
              this, tr("Question"),
              tr("Do you want to deactivate the pre compiler for version "
                 "%1?\nThis change applies to all Robots/Cells using this "
                 "version on your computer")
                  .arg(ui->tableWidget->item(index.row(), 1)->text()),
              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        DialogDeploy dlg(this);
        dlg.setPath(path);
        dlg.show();
        if (!dlg.activatePreCompiler(false))
            dlg.exec();
      }
    } else {
      if (QMessageBox::question(
              this, tr("Question"),
              tr("Do you want to activate the pre compiler for version "
                 "%1?\nThis change applies to all Robots/Cells using this "
                 "version on your computer")
                  .arg(ui->tableWidget->item(index.row(), 1)->text()),
              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        DialogDeploy dlg(this);
        dlg.setPath(path);
        dlg.show();
        if (!dlg.activatePreCompiler(true))
            dlg.exec();
      }
    }
  }
  if (index.column() == 1) {
    bool active =
        ui->tableWidget->item(index.row(), 1)->checkState() == Qt::Checked;
    if (active) {
      if (QMessageBox::question(
              this, tr("Question"),
              tr("Do you want to deactivate the extended help files for "
                 "version %1?\nThis change applies to all Robots/Cells using "
                 "this version on your computer")
                  .arg(ui->tableWidget->item(index.row(), 1)->text()),
              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        DialogDeploy dlg(this);
        dlg.setPath(path);
        dlg.show();
        dlg.activateHelpFile(false);
        dlg.exec();
      }
    } else {
      if (QMessageBox::question(
              this, tr("Question"),
              tr("Do you want to activate the extended help files for version "
                 "%1?\nThis change applies to all Robots/Cells using this "
                 "version on your computer")
                  .arg(ui->tableWidget->item(index.row(), 1)->text()),
              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        DialogDeploy dlg(this);
        dlg.setPath(path);
        dlg.show();
        dlg.activateHelpFile(true);
        dlg.exec();
      }
    }
  }
  fillTableView();
}

void MainWindow::on_actionPrecompiler_Settings_triggered() {
  val3PrecompilerSettings dlg;
  dlg.exec();
}

void MainWindow::slotStartup() { installAll(false); }

void MainWindow::installAll(bool onlyUpdate) {
  DialogDeploy dlg(this);
  dlg.show();

  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    QModelIndex index = ui->tableWidget->model()->index(row, 0);
    QString path = ui->tableWidget->item(index.row(), 3)->text();
    bool active =
        ui->tableWidget->item(index.row(), 0)->checkState() == Qt::Checked;
    bool outDated =
        !ui->tableWidget->item(index.row(), 5)->text().isEmpty() &&
        (ui->tableWidget->item(index.row(), 5)->text() < m_linterVersion);
    qDebug() << __FUNCTION__ << ui->tableWidget->item(index.row(), 5)->text()
             << m_linterVersion;
    if (!onlyUpdate || (onlyUpdate && outDated)) {
      if (active) {

        dlg.setPath(path);
        dlg.show();
        dlg.activatePreCompiler(false);
      }

      dlg.setPath(path);
      dlg.show();
      dlg.activatePreCompiler(true);
    }

    active = ui->tableWidget->item(index.row(), 1)->checkState() == Qt::Checked;
    if (active) {

      dlg.setPath(path);
      dlg.show();
      dlg.activateHelpFile(false);
      dlg.setPath(path);
      dlg.show();
      dlg.activateHelpFile(true);
    }

    qApp->processEvents();
  }
  // dlg.exec();
  fillTableView();
}

void MainWindow::uninstallAll() {
  DialogDeploy dlg(this);
  dlg.show();
  bool active = false;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    QModelIndex index = ui->tableWidget->model()->index(row, 0);
    QString path = ui->tableWidget->item(index.row(), 3)->text();
    active = ui->tableWidget->item(index.row(), 0)->checkState() == Qt::Checked;
    if (active) {

      dlg.setPath(path);
      dlg.show();
      dlg.activatePreCompiler(false);
    }
    active = ui->tableWidget->item(index.row(), 1)->checkState() == Qt::Checked;
    if (active) {

      dlg.setPath(path);
      dlg.show();
      dlg.activateHelpFile(false);
    }
  }
  if (m_autoExit)
    qApp->exit();
}

void MainWindow::checkUpdateLinter() {
  bool atLeastOneNeedsUpdate = false;
  for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
    atLeastOneNeedsUpdate =
        atLeastOneNeedsUpdate |
        (ui->tableWidget->item(row, 6)->checkState() == Qt::Checked);
  }
  if (atLeastOneNeedsUpdate &&
      QMessageBox::question(
          this, tr("New linter version available"),
          tr("At least one SAXE Linter is outdated, do you want to update?")) ==
          QMessageBox::Yes) {
    installAll(true);
  }
}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index) {
  processActivation(index);
}

void MainWindow::storeSettings() {
  qDebug() << __FUNCTION__;
  QSettings settings;
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMenu *m = ui->menuProfile;
  QString activeProfile = "";
  settings.beginWriteArray("profiles");
  int i = 0;
  for (auto item : m->actions()) {
      settings.setArrayIndex(i++);
      settings.setValue("name", item->text());
      settings.setValue("active", item->isChecked());
      settings.setValue("data", item->data());
      if (item->isChecked()) {
          activeProfile = item->text();
      }
  }
  settings.endArray();
  settings.setValue("activeProfile", activeProfile);
}

void MainWindow::loadSettings() {
  qDebug() << __FUNCTION__;
  QSettings settings;
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  QMenu *m = ui->menuProfile;

  int size = settings.beginReadArray("profiles");
  if (size != 0) {
      for (int i = 0; i < size; i++) {
          settings.setArrayIndex(i);
          QAction *act = new QAction(this);
          act->setCheckable(true);
          act->setText(settings.value("name").toString());
          act->setChecked(settings.value("active").toBool());
          act->setData(settings.value("data"));
          m_group->addAction(act);
          m->addAction(act);
      }
      settings.endArray();
  } else {
      QAction *profileItem = new QAction(this);
      m_group->addAction(profileItem);
      profileItem->setText("System developer");
      profileItem->setData(0);
      profileItem->setCheckable(true);
      m->addAction(profileItem);

      profileItem = new QAction(this);
      m_group->addAction(profileItem);
      profileItem->setText("Application programmer");
      profileItem->setData(1);
      profileItem->setCheckable(true);
      profileItem->setChecked(true);
      m->addAction(profileItem);
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  storeSettings();
  QMainWindow::closeEvent(event);
}

void MainWindow::on_actionAbout_Qt_triggered() { qApp->aboutQt(); }

void MainWindow::on_actionRefresh_triggered() { fillTableView(); }
