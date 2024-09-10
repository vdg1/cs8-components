#include "dialogsettings.h"
#include "qfileinfo.h"
#include "qprocess.h"
#include "qsettings.h"
#include "ui_dialogsettings.h"

#include <QFileDialog>

DialogSettings::DialogSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSettings)
{
    ui->setupUi(this);
    QSettings settings;
    ui->checkBox->setCheckState(settings.value("gitEnabled", Qt::Unchecked).toBool() ? Qt::Checked : Qt::Unchecked);
    ui->lineEdit->setText(settings.value("gitPath", "").toString());

}

DialogSettings::~DialogSettings()
{
    delete ui;
}

void DialogSettings::on_lineEdit_textChanged(const QString &arg1)
{
    // verify if file at path exists
    QFile file(arg1);
    if (!file.exists()) {
        ui->lbVersionInfo->setText("File does not exist");
    } else {
        // check filename is git.exe
        QFileInfo fileInfo(file);
        if (fileInfo.fileName() == "git.exe") {
            // retrieve git version
            QProcess process;
            process.start(arg1, QStringList() << "--version");
            process.waitForFinished();
            QString output = process.readAllStandardOutput();
            ui->lbVersionInfo->setText(output);
        } else {
            ui->lbVersionInfo->setText("Git not found");
        }
    }
}



void DialogSettings::on_toolButton_clicked()
{
    // open file dialog to select git.exe
    QString fileName = QFileDialog::getOpenFileName(this, "Select git.exe",ui->lineEdit->text(), "Executable (*.exe)");
    if (!fileName.isEmpty()) {
        ui->lineEdit->setText(fileName);
    }
}


void DialogSettings::on_buttonBox_accepted()
{
    // store settings
    QSettings settings;
    settings.setValue("gitEnabled", ui->checkBox->checkState() == Qt::Checked);
    settings.setValue("gitPath", ui->lineEdit->text());
}


void DialogSettings::on_checkBox_stateChanged(int arg1)
{
    ui->lineEdit->setEnabled(arg1 == Qt::Checked);
    ui->toolButton->setEnabled(arg1 == Qt::Checked);
}

