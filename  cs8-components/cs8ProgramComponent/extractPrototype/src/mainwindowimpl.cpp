#include "mainwindowimpl.h"
#include "cs8application.h"
#include <QFileDialog>
#include <QSettings>
#include <QDebug>

MainWindowImpl::MainWindowImpl(QWidget *parent) :
    QMainWindow(parent) {
        setupUi(this);
        QCoreApplication::setOrganizationName("Swiss-System AB");
        QCoreApplication::setApplicationName("Val3 Prototype Extractor");

        QSettings settings;
        lineEditSource->setText(settings.value("sourceDir").toString());
        lineEditDest->setText(settings.value("DestDir").toString());
        lineEditCDest->setText(settings.value("DestDirCExport").toString());
        checkBoxActivatePrototypes->setCheckState(settings.value(
                                                      "activatePrototypes").toBool() ? Qt::Checked : Qt::Unchecked);
        checkBoxActivateCExport->setCheckState(
                    settings.value("activateCExport").toBool() ? Qt::Checked
                                                               : Qt::Unchecked);

    }

MainWindowImpl::~MainWindowImpl() {
        QSettings settings;

        settings.setValue("activatePrototypes",
                          checkBoxActivatePrototypes->checkState());
        settings.setValue("activateCExport", checkBoxActivateCExport->checkState());
    }

void MainWindowImpl::on_toolButtonSource_clicked() {
        QString dir = lineEditSource->text();
        dir = QFileDialog::getExistingDirectory(this,
                                                tr("Select source directory"), dir, QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty()) {
            lineEditSource->setText(dir);
            QSettings settings;
            settings.setValue("sourceDir", dir);
        }
    }

void MainWindowImpl::on_toolButtonDest_clicked() {
        QString dir = lineEditDest->text();
        dir = QFileDialog::getExistingDirectory(this, tr(
                                                    "Select destination directory for prototype export"), dir,
                                                QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty()) {
            lineEditDest->setText(dir);
            QSettings settings;
            settings.setValue("destDir", dir);
        }
    }

void MainWindowImpl::on_toolButtonCDest_clicked() {
        QString dir = lineEditCDest->text();
        dir = QFileDialog::getExistingDirectory(this, tr(
                                                    "Select destination directory for C export"), dir,
                                                QFileDialog::ShowDirsOnly);
        if (!dir.isEmpty()) {
            lineEditCDest->setText(dir);
            QSettings settings;
            settings.setValue("DestDirCExport", dir);
        }
    }

void MainWindowImpl::on_pushButtonExecute_clicked() {
        QString sourceDir = lineEditSource->text();
        QString destDir = lineEditDest->text();
        QString destCDir = lineEditCDest->text();
        lineEditLog->clear();
        lineEditLog->appendPlainText("source: " + sourceDir);
        lineEditLog->appendPlainText("dest:   " + destDir);
        lineEditLog->appendPlainText("dest C: " + destCDir);

        QDir dir(sourceDir);
        cs8Application application;
        QStringList sourceFiles = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        sourceFiles.append("");
        QStringList addFiles = dir.entryList(QStringList() << "*.pjx", QDir::Files);
        if (addFiles.count() > 0){
            QString addFile=addFiles.at(0);
            addFile.chop(4);
            sourceFiles.append(addFile);
        }
        lineEditLog->appendPlainText("applications: " + sourceFiles.join(", "));
        QString sourceApp;
        foreach(sourceApp,sourceFiles) {
            qApp->processEvents();
            if (application.openFromPathName(sourceDir + "/" + sourceApp)) {
                lineEditLog->appendPlainText(sourceApp + " open ok");
                if (checkBoxActivatePrototypes->checkState() == Qt::Checked)
                    application.exportInterfacePrototype(destDir);
                if (checkBoxActivateCExport->checkState() == Qt::Checked)
                    application.exportToCFile(destCDir);

            } else
                lineEditLog->appendPlainText(sourceApp + " failed");
        }
        lineEditLog->appendPlainText("Done");
        qDebug() << "Done";
    }
