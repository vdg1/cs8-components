#include "dialogbuilddocumentation.h"
#include "ui_dialogbuilddocumentation.h"

#include <QProcess>
#include <QSettings>
#include <QDesktopServices>
#include <QDateTime>
#include <QDir>

DialogBuildDocumentation::DialogBuildDocumentation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBuildDocumentation)
{
    ui->setupUi(this);
    doxygenProcess=new QProcess(this);
    connect(doxygenProcess,SIGNAL(error(QProcess::ProcessError)),
            this,SLOT(slotProcessError(QProcess::ProcessError)));
    connect(doxygenProcess,SIGNAL(finished(int)),
            this,SLOT(slotFinished(int)));
    connect(doxygenProcess,SIGNAL(readyRead()),
            this,SLOT(slotReadyRead()));
    connect(doxygenProcess,SIGNAL(readyReadStandardError()),
            this,SLOT(slotReadyRead()));

    setModal(true);
    configFile=new QTemporaryFile(this);

}

DialogBuildDocumentation::~DialogBuildDocumentation()
{
    delete ui;
}

void DialogBuildDocumentation::build(cs8Application *application)
{

    qApp->setOverrideCursor(Qt::WaitCursor);
    QSettings settings;
    QString tempPath=QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            +"/Val3"+QDateTime::currentDateTime().toString("hh-mm-ss");
    QDir dir;
    if ( dir.mkpath(tempPath))

    {
        ui->plainTextEdit->appendPlainText("Exporting to C file");
        application->exportToCClass(tempPath);
        if (application->includeLibraryDocuments())
        {
            foreach(cs8LibraryAlias *alias, application->libraryModel()->list())
            {
                cs8Application app;
                app.setCellPath(application->cellPath()+"/usr/usrapp");
                app.open(alias->path());
                app.exportToCClass(tempPath);
            }
        }
        // generate doxygen file

        QDir templateDir(QDir::currentPath()+"/doxygenTemplateData");
        QFile sourceFile(templateDir.absolutePath()+"/DoxyfileVal3");

        if (sourceFile.open(QFile::ReadOnly) && configFile->open())
        {
            QString outputPath=QDir::toNativeSeparators(dir.absolutePath()+"/documentation");
            dir.mkpath(outputPath);
            QString outputFileName=outputPath+"/"+application->name()+(application->version().isEmpty()?"":"_"+application->version())+".chm";
            QFile file(outputFileName);

            if (file.remove() || !file.exists())
            {
                QString configText=sourceFile.readAll();
                configText.replace("#Temp#",QDir::toNativeSeparators(tempPath));
                configText.replace("#OutputDir#",outputPath);
                configText.replace("#projectName#",application->name());
                configText.replace("#chmFile#",outputFileName);
                configText.replace("#templateDir#",templateDir.absolutePath()+"/");
                configText.replace("#Version#",application->version());
                configFile->write(configText.toLatin1());
                //ui->plainTextEdit->appendPlainText(configText);
                configFile->close();
                ui->plainTextEdit->appendPlainText("Config file: "+configFile->fileName());
                doxygenProcess->start(settings.value("doxygenBin").toString(),QStringList() << QDir::toNativeSeparators(configFile->fileName()));
            }
            else
            {
                ui->plainTextEdit->appendPlainText(tr("Couldn't delete destination file: %1").arg(outputFileName));
                ui->buttonBox->setEnabled(true);
                qApp->restoreOverrideCursor();
            }

        }


    }
    else
    {
        ui->plainTextEdit->appendPlainText("Failed to generate temporary output path " + tempPath);
        ui->buttonBox->setEnabled(true);
        qApp->restoreOverrideCursor();
    }

}

void DialogBuildDocumentation::slotProcessError(QProcess::ProcessError)
{
    ui->plainTextEdit->appendPlainText("Error occured in callling doxygen:" + doxygenProcess->errorString());
    ui->buttonBox->setEnabled(true);
    qApp->restoreOverrideCursor();

}

void DialogBuildDocumentation::slotFinished(int)
{
    ui->plainTextEdit->appendPlainText("doxygen finished: "+doxygenProcess->errorString());
    ui->buttonBox->setEnabled(true);
    qApp->restoreOverrideCursor();
}

void DialogBuildDocumentation::slotReadyRead()
{
    while (doxygenProcess->canReadLine())
    {
        ui->plainTextEdit->appendPlainText(doxygenProcess->readLine());
        qApp->processEvents();
    }
    ui->plainTextEdit->appendPlainText(doxygenProcess->readAllStandardError());
    qApp->processEvents();

}
