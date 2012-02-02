#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_application=new cs8Application(this);
    ui->listViewProgams->setModel(m_application->programModel());

    ui->tableViewPars->setMode(false);
    ui->tableViewPars->setMasterView(ui->listViewProgams);
    ui->tableViewVars->setMode(true);
    ui->tableViewVars->setMasterView(ui->listViewProgams);
    ui->textEditDoc->setMasterView(ui->listViewProgams);

    connect(ui->listViewProgams->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this,SLOT(slotSelectionChanged(QItemSelection,QItemSelection)));

    //"D:\data\Staubli\CS8\Development_664\usr\usrapp\Libraries\extServo\extServo.pjx"
    //m_application->open("D:/data/Cpp/components/cs8ProgramComponent/cs8DocEditor/sample/extServo/extServo.pjx");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::on_action_Open_triggered()
{
    QString applicationName=QFileDialog::getOpenFileName(this,tr("Open a Val3 project"),m_application->name(),"(*.pjx)");
    if (!applicationName.isEmpty())
    {
        m_application->open(applicationName);
    }
}

void MainWindow::on_action_Save_triggered()
{
    m_application->save();
}

void MainWindow::slotSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected )
{
    int index=selected.indexes().at(0).row();
    ui->textEditCode->setText(m_application->programModel()->programList().at(index)->val3Code());
    // ui->textEditDoc->setText(m_application->programModel()->programList().at(index)->documentation(false));
}

void MainWindow::on_tableViewPars_doubleClicked(QModelIndex index)
{
    cs8Variable* var=qobject_cast<cs8VariableModel*>( ui->tableViewPars->model())->variable(index);
    ui->detailEditor->setVariable(var);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_detailEditor_done(){
    ui->stackedWidget->setCurrentIndex(0);
}
