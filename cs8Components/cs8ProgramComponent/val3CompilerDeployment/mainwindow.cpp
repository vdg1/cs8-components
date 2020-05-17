#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"
#include "versionInfo.h"
#include "dialogdeploy.h"
#include "val3PrecompilerSettings.h"

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QTimer>
#include <chmxx.h>


using namespace std;
using namespace chm;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_autoExit(0)
{
    ui->setupUi(this);
    setup();

    // check arguments
    qDebug() << "arguments:" << qApp->arguments();
    if (qApp->arguments().contains("--install")) {
        QTimer::singleShot(0,this,SLOT(slotStartup()));
    }

    else if (qApp->arguments().contains("--uninstall")) {
        QTimer::singleShot(0,this,SLOT(uninstallAll()));
    }
    if (qApp->arguments().contains("--exitWhenReady")) {
        m_autoExit=true;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setup()
{
    QSettings settings("HKEY_LOCAL_MACHINE\\Software\\Wow6432Node\\Staubli\\CS8",
                       QSettings::NativeFormat);

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->setRowCount(0);
    int row=0;
    foreach(QString path,settings.allKeys())
    {
        if (path.contains("/s7.") && path.endsWith("/path"))
        {
            QString systemPath=settings.value(path).toString();
            QString val3Version=systemPath.split("\\").last();
            QString fileVersion;
            QString productName;
            if (getProductName(systemPath+"\\VAL3Check.exe", productName, fileVersion))
            {
                if (productName.isEmpty())
                    productName=QString("Staubli");

                bool activePrecompiler= productName.startsWith("Val3") || productName.startsWith("SAXE");
                if (!activePrecompiler)
                    fileVersion="";

                chmfile chm(QString(systemPath+"\\etc\\val3_en.chm").toLocal8Bit().constData());
                QString helpfileTitle=QString::fromUtf8(chm.get_title().c_str());
                bool activeHelpFile=helpfileTitle.contains(("SAXE"));

                qDebug() << path << settings.value(path).toString();
                ui->tableWidget->insertRow(0);
                ui->tableWidget->setItem(0,0,new QTableWidgetItem());
                ui->tableWidget->item(0,0)->setCheckState(activePrecompiler? Qt::Checked : Qt::Unchecked );
                ui->tableWidget->item(0,0)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->item(0,0)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->item(0,0)->setToolTip(tr("Double click to activate or deactivate"));

                ui->tableWidget->setItem(0,1,new QTableWidgetItem());
                ui->tableWidget->item(0,1)->setCheckState(activeHelpFile? Qt::Checked : Qt::Unchecked );
                ui->tableWidget->item(0,1)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->item(0,1)->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->item(0,0)->setToolTip(tr("Double click to activate or deactivate"));

                ui->tableWidget->item(0,0)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->setItem(0,3,new QTableWidgetItem(systemPath));
                ui->tableWidget->item(0,3)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->setItem(0,2,new QTableWidgetItem(val3Version));
                ui->tableWidget->item(0,2)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->setItem(0,4,new QTableWidgetItem(productName));
                ui->tableWidget->item(0,4)->setFlags(Qt::ItemIsEnabled);
                ui->tableWidget->setItem(0,5,new QTableWidgetItem(fileVersion));
                ui->tableWidget->item(0,5)->setFlags(Qt::ItemIsEnabled);
                row++;
            }
        }
    }
    ui->tableWidget->sortByColumn(2);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->blockSignals(false);
}

void MainWindow::processActivation(const QModelIndex &index)
{
    if (!index.isValid())// || index.column()!=0 || index.column()!=1)
        return;

    QString path=ui->tableWidget->item(index.row(),3)->text();
    if (index.column()==0)
    {
        bool active=ui->tableWidget->item(index.row(),0)->checkState()==Qt::Checked;
        if (active){
            if (QMessageBox::question(this,
                                      tr("Question"),
                                      tr("Do you want to deactivate the pre compiler for version %1?\nThis change applies to all Robots/Cells using this version on your computer")
                                      .arg(ui->tableWidget->item(index.row(),1)->text()),
                                      QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
            {

                DialogDeploy dlg(this);
                dlg.setPath(path);
                dlg.show();
                dlg.activatePreCompiler(false);
                dlg.exec();
            }
        }
        else
        {
            if (QMessageBox::question(this,
                                      tr("Question"),
                                      tr("Do you want to activate the pre compiler for version %1?\nThis change applies to all Robots/Cells using this version on your computer")
                                      .arg(ui->tableWidget->item(index.row(),1)->text()),
                                      QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
            {
                DialogDeploy dlg(this);
                dlg.setPath(path);
                dlg.show();
                dlg.activatePreCompiler(true);
                dlg.exec();
            }
        }
    }
    if (index.column()==1)
    {
        bool active=ui->tableWidget->item(index.row(),1)->checkState()==Qt::Checked;
        if (active){
            if (QMessageBox::question(this,
                                      tr("Question"),
                                      tr("Do you want to deactivate the extended help files for version %1?\nThis change applies to all Robots/Cells using this version on your computer")
                                      .arg(ui->tableWidget->item(index.row(),1)->text()),
                                      QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
            {

                DialogDeploy dlg(this);
                dlg.setPath(path);
                dlg.show();
                dlg.activateHelpFile(false);
                dlg.exec();
            }
        }
        else
        {
            if (QMessageBox::question(this,
                                      tr("Question"),
                                      tr("Do you want to activate the extended help files for version %1?\nThis change applies to all Robots/Cells using this version on your computer")
                                      .arg(ui->tableWidget->item(index.row(),1)->text()),
                                      QMessageBox::Yes | QMessageBox::No)==QMessageBox::Yes)
            {
                DialogDeploy dlg(this);
                dlg.setPath(path);
                dlg.show();
                dlg.activateHelpFile(true);
                dlg.exec();
            }
        }
    }
    setup();
}



void MainWindow::on_actionPrecompiler_Settings_triggered()
{
    val3PrecompilerSettings dlg;
    dlg.exec();
}

void MainWindow::slotStartup()
{
    installAll();
}

void MainWindow::installAll()
{
    DialogDeploy dlg(this);
    dlg.show();

    for (int row=0;row<ui->tableWidget->rowCount();row++){
        QModelIndex index=ui->tableWidget->model()->index(row,0);
        QString path=ui->tableWidget->item(index.row(),3)->text();
        bool active=ui->tableWidget->item(index.row(),0)->checkState()==Qt::Checked;
        if (active){

            dlg.setPath(path);
            dlg.show();
            dlg.activatePreCompiler(false);


        }

        dlg.setPath(path);
        dlg.show();
        dlg.activatePreCompiler(true);

        active=ui->tableWidget->item(index.row(),1)->checkState()==Qt::Checked;
        if (active){

            dlg.setPath(path);
            dlg.show();
            dlg.activateHelpFile(false);
            dlg.setPath(path);
            dlg.show();
            dlg.activateHelpFile(true);

        }



        qApp->processEvents();
    }
    dlg.exec();
    setup();
}


void MainWindow::uninstallAll()
{
    DialogDeploy dlg(this);
    dlg.show();
    bool active=false;
    for (int row=0;row<ui->tableWidget->rowCount();row++){
        QModelIndex index=ui->tableWidget->model()->index(row,0);
        QString path=ui->tableWidget->item(index.row(),3)->text();
        active=ui->tableWidget->item(index.row(),0)->checkState()==Qt::Checked;
        if (active){

            dlg.setPath(path);
            dlg.show();
            dlg.activatePreCompiler(false);


        }
        active=ui->tableWidget->item(index.row(),1)->checkState()==Qt::Checked;
        if (active){

            dlg.setPath(path);
            dlg.show();
            dlg.activateHelpFile(false);
        }
    }
    if (m_autoExit)
        qApp->exit();
}

void MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{

}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    processActivation(index);
}
