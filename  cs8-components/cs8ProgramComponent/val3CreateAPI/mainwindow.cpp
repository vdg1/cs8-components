#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cs8application.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_cs8SourceApps.clear ();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filePath;
    filePath=QFileDialog::getExistingDirectory (this,"Select Val3 Project directory","/media/Data/data/Staubli/CS8/CreateAPI Test/usr/usrapp/SAXE Automation/Modules/");
    if (!filePath.isEmpty ())
    {

        QDir dir;
        dir.setCurrent (filePath);
        QStringList dirs=dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs );

        foreach(QString pth,dirs)
        {
            cs8Application *cs8SourceApp=new cs8Application(this);
            if (cs8SourceApp->openFromPathName (pth))
            {
                m_cs8SourceApps.append (cs8SourceApp);
            }
            else
                delete cs8SourceApp;
        }

        QMap<QString, cs8Application *> cs8DestApps;
        cs8Application* cs8DestApp;
        QString name;

        foreach(cs8Application *cs8SourceApp,m_cs8SourceApps)
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
                        cs8DestApp->setCellPath (m_cs8SourceApps.at (0)->cellPath());
                        cs8DestApps.insert (destAppName,cs8DestApp);
                    }
                    cs8DestApp=cs8DestApps.value (destAppName);

                    qDebug() << program->name ();
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
}
