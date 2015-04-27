#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QFileDialog>
#include <QSettings>
#include <QDesktopServices>

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    QSettings settings;
    QString programFilesPath(getenv("PROGRAMFILES"));
    QString dir=settings.value("doxygenBin",
                               programFilesPath+"/doxygen/bin/doxygen.exe").toString();
    ui->lineEdit->setText(QDir::toNativeSeparators(dir));
    dir=settings.value("hhcBin",
                       programFilesPath+"/HTML Help Workshop/hhc.exe").toString();
    ui->lineEditHHC->setText(QDir::toNativeSeparators(dir));
}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::on_toolButton_clicked()
{
    QSettings settings;
    QString dir=settings.value("doxygenBin",ui->lineEdit->text()).toString();
    QString fileName=QFileDialog::getOpenFileName(this,tr("Doxygen executable"),dir,"doxygen.exe");
    if (!fileName.isEmpty())
    {
        settings.setValue("doxygenBin",fileName);
        ui->lineEdit->setText(QDir::toNativeSeparators(fileName));
    }
}


void DialogSettings::on_toolButtonHHC_clicked()
{
    QSettings settings;
    QString dir=settings.value("hhcBin",ui->lineEditHHC->text()).toString();
    QString fileName=QFileDialog::getOpenFileName(this,tr("Help File Compiler"),dir,"hhc.exe");
    if (!fileName.isEmpty())
    {
        settings.setValue("hhcBin",fileName);
        ui->lineEditHHC->setText(QDir::toNativeSeparators(fileName));
    }
}
