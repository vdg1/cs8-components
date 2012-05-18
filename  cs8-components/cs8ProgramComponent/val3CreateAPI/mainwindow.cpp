#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cs8application.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QtConcurrentRun>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QCoreApplication::setOrganizationName("SAXE Swiss System");
    QCoreApplication::setOrganizationDomain("hsh.as");
    QCoreApplication::setApplicationName("Val3 Create API");

    ui->setupUi(this);
    readSettings ();

    connect (this,SIGNAL(addLog(QString)),this,SLOT(slotAddLog(QString)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //if (maybeSave()) {
    writeSettings();
    event->accept();
    //} else {
    //    event->ignore();
    //}
}

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
    ui->listWidget->clear ();
    ui->listWidget->addItems (settings.value("recentFileList").toStringList());
    m_recentLocation=settings.value("recentLocation").toString ();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    QStringList list;
    for (int  i=0;i<ui->listWidget->count ();i++)
        list << ui->listWidget->item (i)->text ();
    settings.setValue ("recentFileList",list);
    settings.setValue ("recentLocation",m_recentLocation);
}


void MainWindow::on_actionAdd_Folder_triggered()
{

    QString filePath=QFileDialog::getExistingDirectory (this,"Select Val3 Project directory",m_recentLocation);
    if (!filePath.isEmpty ())
    {
        // check if path is already in list
        if (ui->listWidget->findItems (filePath,Qt::MatchFixedString).count ()>0)
        {
            QMessageBox::warning (this,tr("Warning"),tr("The path %1 is already in the list!").arg(filePath));
            return;
        }
        m_recentLocation=filePath;
        ui->listWidget->addItem (filePath);
    }
}

void MainWindow::on_actionRemove_Folder_triggered()
{
    /*
    if (ui->listWidget->currentRow ()>=0)
        ui->listWidget->takeItem (ui->listWidget->currentRow ());
        */
    foreach(QListWidgetItem *item, ui->listWidget->selectedItems ())
        ui->listWidget->takeItem (ui->listWidget->row(item));
}

void MainWindow::on_commandLinkButton_clicked()
{

    ui->commandLinkButton->setEnabled(false);
    //future=QtConcurrent::run((*this),&MainWindow::createAPI );
    createAPI();
}

void MainWindow::createAPIs(QList<cs8Application *> cs8SourceApps)
{
    QMap<QString, cs8Application *> cs8DestApps;
    cs8Application* cs8DestApp;
    QString name;

    foreach(cs8Application *cs8SourceApp,cs8SourceApps)
    {
        foreach(cs8Program *program, cs8SourceApp->programModel ()->publicPrograms ())
        {
            if (program->name ().startsWith ("_"))
            {
                QString destAppName;
                destAppName=cs8SourceApp->exportDirectives ().value (program->name (),"zFuncCall");
                if (!cs8DestApps.contains (destAppName))
                {
                    cs8DestApp=new cs8Application(this);
                    cs8DestApp->setName(destAppName);
                    cs8DestApp->setCellPath (cs8SourceApps.at (0)->cellPath());
                    cs8DestApps.insert (destAppName,cs8DestApp);
                }
                cs8DestApp=cs8DestApps.value (destAppName);

                qDebug() << program->name ();
                emit addLog (tr("Exporting function %1 from application %2 to module %3")
                             .arg(program->name ())
                             .arg(cs8SourceApp->name ())
                             .arg(destAppName));
                if (!cs8DestApp->libraryModel ()->contains(cs8SourceApp->name ()))
                {
                    qDebug() << "Adding library alias for " << cs8SourceApp->name ();
                    cs8DestApp->libraryModel ()->add (cs8SourceApp->name (),cs8SourceApp->cellProjectFilePath (),true);
                }
                cs8Program *newProgram=new cs8Program(this);
                name=program->name ();
                name=name.remove (0,1);
                newProgram->setPublic (true);
                newProgram->copyFromParameterModel(program->parameterModel ());
                newProgram->setCellPath (cs8DestApp->cellPath ());
                newProgram->setCode(QString("begin\ncall %1:%2(%3)\nend")
                                    .arg(cs8SourceApp->name ())
                                    .arg(program->name ())
                                    .arg(program->parameterModel ()->toString (false)));

                cs8DestApp->programModel ()->append(newProgram);
                cs8DestApp->moveParamsToGlobals(newProgram);
                newProgram->setDescription (program->description ());
                newProgram->setName(name);
                newProgram->setDescription(program->description());
                newProgram->setDetailedDocumentation(program->detailedDocumentation());
            }
        }
    }
    foreach(cs8Application* cs8DestApp, cs8DestApps)
        cs8DestApp->save ();
}

void MainWindow::createAPI()
{
    ui->plainTextEdit->clear ();
    QStringList dirs;
    QDir dir;


    for(int i=0;i<ui->listWidget->count ();i++)
    {
        dir.setCurrent (ui->listWidget->item (i)->text ());
        foreach(QString pth,dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs ))
            dirs << dir.currentPath ()+"/"+pth;
    }

    QList<cs8Application *> cs8SourceApps;
    foreach(QString pth,dirs)
    {
        cs8Application *cs8SourceApp=new cs8Application(this);
        if (cs8SourceApp->openFromPathName (pth))
        {
            emit addLog (tr("Opened source app %1").arg(pth));
            cs8SourceApps.append (cs8SourceApp);
        }
        else
        {
             emit addLog (tr("Opened source app %1 failed").arg(pth));
            delete cs8SourceApp;
        }
    }

    createAPIs(cs8SourceApps);
    ui->commandLinkButton->setEnabled(true);
}

void MainWindow::on_actionExit_triggered()
{
    close ();
}

void MainWindow::slotAddLog(const QString &msg)
{
    ui->plainTextEdit->appendPlainText (msg);
}
