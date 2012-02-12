#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cs8application.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_cs8SourceApp = new cs8Application(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filePath;
    filePath=QFileDialog::getOpenFileName (this,"Select Val3 Project directory","D:\\data\\Staubli\\CS8\\CreateAPI Test\\usr\\usrapp\\SAXE Automation\\Modules\\modIMM","*.pjx");
    if (!filePath.isEmpty ())
    {

        m_cs8SourceApp->open (filePath);

        cs8Application cs8DestApp;
        cs8DestApp.setName("zFuncCall");
        cs8DestApp.setCellPath (m_cs8SourceApp->cellPath());

        foreach(cs8Program *program, m_cs8SourceApp->programModel ()->publicPrograms ())
        {
            if (program->name ().startsWith ("_"))
            {
                qDebug() << program->name ();
                if (!cs8DestApp.libraryModel ()->contains(m_cs8SourceApp->name ()))
                {
                    qDebug() << "Adding library alias for " << m_cs8SourceApp->name ();
                    cs8DestApp.libraryModel ()->addAlias (m_cs8SourceApp->name (),m_cs8SourceApp->inCellFilePath (),true);
                }
                cs8Program *newProgram=new cs8Program(this);
                QString name=program->name ();
                name=name.remove (0,1);
                newProgram->setPublic (true);

                //newProgram->setParameterModel(program->parameterModel ());
                newProgram->setCellPath (cs8DestApp.cellPath ());
                newProgram->setCode(QString("begin\ncall %1:%2(%3)\nend")
                                    .arg(m_cs8SourceApp->name ())
                                    .arg(program->name ())
                                    .arg(program->parameterModel ()->toString ()));
                cs8DestApp.programModel ()->append(newProgram);
                newProgram->setDescription (program->description ());
                newProgram->setName(name);


            }
        }

        cs8DestApp.save ();
    }
}
