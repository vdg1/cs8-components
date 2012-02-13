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
    filePath=QFileDialog::getExistingDirectory (this,"Select Val3 Project directory","D:\\data\\Staubli\\CS8\\CreateAPI Test\\usr\\usrapp\\SAXE Automation\\Modules");
    if (!filePath.isEmpty ())
    {

        QDir dir;
        dir.setCurrent (filePath);
        QStringList dirs=dir.entryList (QDir::NoDotAndDotDot | QDir::Dirs );

        foreach(QString pth,dirs)
        {
            cs8Application *cs8App=new cs8Application(this);
            if (cs8App->openFromPathName (pth))
            {
                m_cs8SourceApps.append (cs8App);
            }
            else
                delete cs8App;
        }

        cs8Application cs8DestApp;
        cs8DestApp.setName("zFuncCall");
        cs8DestApp.setCellPath (m_cs8SourceApps.at (0)->cellPath());

        foreach(cs8Application *cs8App,m_cs8SourceApps)
        {

            foreach(cs8Program *program, cs8App->programModel ()->publicPrograms ())
            {
                if (program->name ().startsWith ("_"))
                {
                    qDebug() << program->name ();
                    if (!cs8DestApp.libraryModel ()->contains(cs8App->name ()))
                    {
                        qDebug() << "Adding library alias for " << cs8App->name ();
                        cs8DestApp.libraryModel ()->addAlias (cs8App->name (),cs8App->cellProjectFilePath (),true);
                    }
                    cs8Program *newProgram=new cs8Program(this);
                    QString name=program->name ();
                    name=name.remove (0,1);
                    newProgram->setPublic (true);
                    newProgram->copyFromParameterModel(program->parameterModel ());
                    newProgram->setCellPath (cs8DestApp.cellPath ());
                    newProgram->setCode(QString("begin\ncall %1:%2(%3)\nend")
                                        .arg(cs8App->name ())
                                        .arg(program->name ())
                                        .arg(program->parameterModel ()->toString (false)));
                    cs8DestApp.programModel ()->append(newProgram);
                    newProgram->setDescription (program->description ());
                    newProgram->setName(name);


                }
            }

        }
        cs8DestApp.save ();

    }
}
