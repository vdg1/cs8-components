#include "dialogsettings.h"
#include "ui_dialogsettings.h"

#include <QFileDialog>
#include <QSettings>

DialogSettings::DialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    QSettings settings;
    QString dir=settings.value("doxygenBin",QDir::currentPath()).toString();
    ui->lineEdit->setText(dir);
    ui->cbIncludeLibraries->setChecked(settings.value("includeLibaries",true).toBool());

}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::on_toolButton_clicked()
{
    QSettings settings;
    QString dir=settings.value("doxygenBin",QDir::currentPath()).toString();
    QString fileName=QFileDialog::getOpenFileName(this,tr("Doxygen executable"),dir,"doxygen.exe");
    if (!fileName.isEmpty())
    {
        settings.setValue("doxygenBin",fileName);
        ui->lineEdit->setText(fileName);
    }
}

void DialogSettings::on_cbIncludeLibraries_toggled(bool checked)
{
    QSettings settings;
    settings.setValue("includeLibaries",checked);
}
