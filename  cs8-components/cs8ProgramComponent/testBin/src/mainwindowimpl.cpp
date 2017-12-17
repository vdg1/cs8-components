#include "mainwindowimpl.h"
#include "cs8application.h"
#include "cs8programmodel.h"
#include "cs8highlighter.h"
#include "cs8project.h"
#include <QSettings>
#include <QFileDialog>
//
MainWindowImpl::MainWindowImpl(QWidget * parent, Qt::WFlags f) :
    QMainWindow(parent, f) {
    setupUi(this);
    connect(lvPrograms, SIGNAL ( activated ( const QModelIndex& ) ),
            this, SLOT ( slotActivated ( const QModelIndex& ) ) );
    project = new cs8Project(this);

    // set model for program list view
    //lvPrograms->setModel((QAbstractItemModel*) project->programModel());
    //lvPrograms->setModel(project);
    // attach views to master view
    teDescription->setMasterView(lvPrograms);
    tvLocalVariables->setMasterView(lvPrograms);
    // parameter mode
    tvParameters->setMode(true);
    tvParameters->setMasterView(lvPrograms);
    teCode->setMasterView(lvPrograms);

    //project->open("cs8://default@127.0.0.1/usr/usrapp/z01_SysUserDisp/z01_SysUserDisp.pjx");
    //project->open ( "/home/vd/Documents/Val3/s5.2/usr/usrapp/z01_SysStacking/z01_SysStacking.pjx" );
    //project->open ( "D:/Data/Staubli/CS8/Development_533/usr/usrapp/z01_SysStacking/z01_SysStacking.pjx" );

    cs8Application app;
    app.openFromPathName("/media/Data/data/Staubli/CS8/CreateAPI Test/usr/usrapp/SAXE Automation/coreModules/modUserDisp/");
    app.setName("testApp");
    app.save();


    QCoreApplication::setOrganizationName("SwissSystem AB");
    QCoreApplication::setApplicationName("cs8 Document Editor");
    restoreState();
    //project->save();
}
//

void MainWindowImpl::slotActivated(const QModelIndex & index) {
}

void MainWindowImpl::restoreState() {
    QSettings settings;

    resize(settings.value("size", QSize(300, 200)).toSize());
    move(settings.value("pos", QPoint(0, 0)).toPoint());
    QMainWindow::restoreState(settings.value("window").toByteArray());
    //project->open(settings.value("path", "").toString());

    /*
  resourceDir=settings.value ( "dir" ).toString();
  if ( !resourceDir.isEmpty() )
  openResources ( resourceDir );
  else
  on_action_Open_Resource_Folder_triggered();
  */
}

void MainWindowImpl::saveState() {
    QSettings settings;

    settings.setValue("window", QMainWindow::saveState());
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    //settings.setValue("path", project->projectPath());
}

void MainWindowImpl::closeEvent(QCloseEvent *event) {
    saveState();
}

void MainWindowImpl::on_action_Open_triggered() {
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Val3 Project"),
                                            ""/*project->projectPath()*/, tr("Val3 Projects (*.pjx)"));

    //if (!fileName.isEmpty())
    //project->open(fileName);
}

void MainWindowImpl::on_action_Save_triggered() {
    //project->save();
}

void MainWindowImpl::on_action_Export_triggered() {
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Export Project"),
                                            ""/*project->projectPath()*/, tr("h file (*.h)"));

    //if (!fileName.isEmpty())
    //project->exportToCSyntax(fileName);
}

void MainWindowImpl::on_action_Open_Doxyfile_triggered() {
    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Doxyfile"),
                                            "D:\\data\\Staubli\\CS8\\Development_653\\usr\\usrapp\\Plastic", tr("Doxyfile (Doxyfile*)"));

    if (!fileName.isEmpty()) {
        cs8Project project(this);
        project.openDoxyFile(fileName);
    }
    //project->open(fileName);
}

void MainWindowImpl::on_action_Export_Prototype_triggered()
{
    //roject->exportInterfacePrototype("d:/temp");
}
