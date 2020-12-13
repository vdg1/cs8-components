#include "mainwindow.h"
#include "cs8application.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
//#include <QtConcurrentRun>
#include <cs8variable.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  QCoreApplication::setOrganizationName("SAXE Swiss System");
  QCoreApplication::setOrganizationDomain("hsh.as");
  QCoreApplication::setApplicationName("Val3 Create API");

  ui->setupUi(this);
  readSettings();

  connect(this, SIGNAL(addLog(QString)), this, SLOT(slotAddLog(QString)));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
  // if (maybeSave()) {
  writeSettings();
  event->accept();
  //} else {
  //    event->ignore();
  //}
}

void MainWindow::readSettings() {
  QSettings settings;
  restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
  restoreState(settings.value("mainWindowState").toByteArray());
  ui->listWidget->clear();
  ui->listWidget->addItems(settings.value("recentFileList").toStringList());
  m_recentLocation = settings.value("recentLocation").toString();
}

void MainWindow::writeSettings() {
  QSettings settings;
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());
  QStringList list;
  for (int i = 0; i < ui->listWidget->count(); i++)
    list << ui->listWidget->item(i)->text();
  settings.setValue("recentFileList", list);
  settings.setValue("recentLocation", m_recentLocation);
}

void MainWindow::on_actionAdd_Folder_triggered() {

  QString filePath = QFileDialog::getExistingDirectory(
      this, "Select Val3 Project directory", m_recentLocation);
  if (!filePath.isEmpty()) {
    // check if path is already in list
    if (ui->listWidget->findItems(filePath, Qt::MatchFixedString).count() > 0) {
      QMessageBox::warning(
          this, tr("Warning"),
          tr("The path %1 is already in the list!").arg(filePath));
      return;
    }
    m_recentLocation = filePath;
    ui->listWidget->addItem(filePath);
  }
}

void MainWindow::on_actionRemove_Folder_triggered() {
  /*
  if (ui->listWidget->currentRow ()>=0)
      ui->listWidget->takeItem (ui->listWidget->currentRow ());
      */
  foreach (QListWidgetItem *item, ui->listWidget->selectedItems())
    ui->listWidget->takeItem(ui->listWidget->row(item));
}

void MainWindow::on_commandLinkButton_clicked() {

  ui->commandLinkButton->setEnabled(false);
  // future=QtConcurrent::run((*this),&MainWindow::createAPI );
  createAPI();
}

void MainWindow::createAPIs(QList<cs8Application *> cs8SourceApps,
                            bool copyInProjectFolder) {
  // QMap<QString, cs8Application *> cs8DestApps;
  cs8Application *cs8DestApp = nullptr;
  QString moduleBaseName;
  QString name;

  foreach (cs8Application *cs8SourceApp, cs8SourceApps) {
    // check if module has any public programs
    if (cs8SourceApp->programModel()->publicPrograms().count() > 0) {
      moduleBaseName = cs8SourceApp->name().remove(0, 3);
      cs8DestApp = nullptr;
      // create API calls to public programs
      foreach (cs8Program *program,
               cs8SourceApp->programModel()->programList()) {

        if ((program->name().startsWith("_") && program->isPublic()) ||
            program->name().startsWith("zz")) {
          QString destAppName;

          QString defaultDestAppName = "z" + moduleBaseName;
          // destAppName=cs8SourceApp->exportDirectives ().value (program->name
          // (),defaultDestAppName);
          // ignore export directives
          destAppName = defaultDestAppName;

          if (!cs8DestApp) {
            cs8DestApp = new cs8Application(this);
            cs8DestApp->setName(destAppName);
            cs8DestApp->setCellPath(cs8SourceApps.at(0)->cellPath());
            cs8DestApp->setProjectVersion(
                cs8SourceApps.at(0)->getProjectVersion());
            // create version variable
            cs8Variable *var =
                cs8DestApp->globalVariableModel()->createVariable("sVersion",
                                                                  "string");
            var->setPublic(true);
            var->setScope(cs8Variable::Global);
            var->setDimension("1");
            // cs8DestApp->globalVariableModel()->addVariable(var);
          }

          // qDebug() << program->name();
          emit addLog(
              tr("Exporting function %1 from application %2 to module %3")
                  .arg(program->name())
                  .arg(cs8SourceApp->name())
                  .arg(destAppName));
          qDebug()
              << (tr("Exporting function %1 from application %2 to module %3")
                      .arg(program->name())
                      .arg(cs8SourceApp->name())
                      .arg(destAppName));
          if (!cs8DestApp->libraryModel()->contains(cs8SourceApp->name())) {
            qDebug() << "Adding library alias for " << cs8SourceApp->name();
            cs8DestApp->libraryModel()->add(
                cs8SourceApp->name(), cs8SourceApp->cellProjectFilePath(true),
                true);
          }

          cs8Program *newProgram =
              cs8DestApp->programModel()->createProgram(program->name());
          name = program->name();
          if (name.startsWith("_")) {
            name = name.remove(0, 1);
            newProgram->setPublic(true);
            newProgram->copyFromParameterModel(program->parameterModel());
            newProgram->setCellPath(cs8DestApp->cellPath());
            newProgram->setCode(
                QString("begin\n  call %1:%2(%3)\nend")
                    .arg(cs8SourceApp->name())
                    .arg(program->name())
                    .arg(program->parameterModel()->toString(false)));

            cs8DestApp->moveParamsToGlobals(newProgram);
            newProgram->setDescription(program->briefDescription(true));
            newProgram->setName(name);
            //newProgram->setDescription(program->briefDescription(true));
            newProgram->setDetailedDocumentation(
                program->detailedDocumentation());
          } else {
            newProgram->setPublic(false);
            newProgram->setCellPath(cs8DestApp->cellPath());
            newProgram->setName(name);
            newProgram->setCode(program->val3Code(), true);
          }
        }
      }

      if (cs8DestApp) {
        // copy user type from source app to dest app
        foreach (cs8LibraryAlias *type, cs8SourceApp->typeModel()->list()) {
          // check if type path is relative
          QString pth;
          if (type->path().startsWith("./")) {
            pth = cs8SourceApp->projectPath(true) + type->path().remove(0, 2);
          } else {
            pth = type->path();
          }
          cs8DestApp->typeModel()->add(type->name(), pth, true);
        }

        // create an init program in the API library
        cs8Program *initProgram =
            cs8DestApp->programModel()->createProgram("init");
        initProgram->setName("init");
        initProgram->setPublic(true);
        // initProgram->localVariableModel()->createVariable("i", "num");
        QString initProgramCode;
        // create public CONSTS
        QRegExp rx;
        rx.setPattern("([A-Z]+)(_[A-Z0-9]*)");
        foreach (cs8Variable *var,
                 cs8SourceApp->globalVariableModel()->publicVariables()) {
          // public global variable is a CONST and a num type
          if (rx.indexIn(var->name()) == 0 && var->type() == "num") {
            cs8Variable *globalVar =
                cs8DestApp->globalVariableModel()->createVariable(var->name(),
                                                                  var->type());

            globalVar->setPublic(true);
            globalVar->setScope(cs8Variable::Global);
            globalVar->setDimension(var->dimension());
            // cs8DestApp->globalVariableModel()->addVariable(globalVar);
            for (uint i = 0; i < var->size(); i++) {

              initProgramCode += QString("  %1[%2]=%3:%1[%2]\n")
                                     .arg(var->name())
                                     .arg(i)
                                     .arg(cs8SourceApp->name());
            }
          }
        }

        // try to load ioMap library
        cs8Application ioMapApplication;
        // if (cs8DestApp &&
        // ioMapApplication.openFromPathName(cs8SourceApp->cellPath()+"/usr/usrapp/ioMaps/io"+moduleBaseName))
        if (cs8DestApp &&
            ioMapApplication.openFromPathName(cs8SourceApp->projectPath() +
                                              "/io" + moduleBaseName)) {
          // ioMap library exists
          // add library alias
          /*
          QDir dir(ioMapApplication.cellDataFilePath(false));
          dir.cdUp();
          dir.cdUp();
          dir.cdUp();
          dir.cdUp();
          dir.cdUp();
          dir.cd("ioMaps");
          QString pth = QDir::fromNativeSeparators(dir.absolutePath());
          pth = pth.replace(QDir::fromNativeSeparators(
                                ioMapApplication.cellPath() + "usr/usrapp/"),
                            "Disk://");
          pth = QDir::fromNativeSeparators(pth);
            */
          QString pth = "Disk://ioMaps";

          qDebug() << "ioMap library path: " << pth;
          cs8DestApp->libraryModel()->add(
              ioMapApplication.name(),
              pth + "/" + ioMapApplication.name() + "/" +
                  ioMapApplication.name() +
                  ".pjx" /* ioMapApplication.cellProjectFilePath() */,
              true);

          foreach (cs8Variable *variable,
                   ioMapApplication.globalVariableModel()->publicVariables()) {
            if (variable->type() == "dio" && variable->name().startsWith("_")) {
              qDebug() << "Getting link for " << variable->name();
              // create dio Variable
              cs8Variable *dioVar =
                  cs8DestApp->globalVariableModel()->createVariable(
                      variable->name().remove(0, 1), "dio");

              dioVar->setPublic(true);
              dioVar->setScope(cs8Variable::Global);
              dioVar->setDimension(variable->dimension());
              dioVar->setDescription(variable->description());

              // cs8DestApp->globalVariableModel()->addVariable(dioVar);
              for (uint i = 0; i < variable->size(); i++) {
                // add dioLink code
                initProgramCode += QString("  dioLink(%1[%4],%2:%3[%4])\n")
                                       .arg(dioVar->name())
                                       .arg(ioMapApplication.name())
                                       .arg(variable->name())
                                       .arg(i);
              }
            }
            if (variable->type() == "aio" && variable->name().startsWith("_")) {
              qDebug() << "Getting link for " << variable->name();
              // create dio Variable
              cs8Variable *dioVar =
                  cs8DestApp->globalVariableModel()->createVariable(
                      variable->name().remove(0, 1), "aio");

              dioVar->setPublic(true);
              dioVar->setScope(cs8Variable::Global);
              dioVar->setDimension(variable->dimension());

              // cs8DestApp->globalVariableModel()->addVariable(dioVar);

              for (uint i = 0; i < variable->size(); i++) {
                // add dioLink code
                initProgramCode += QString("  aioLink(%1[%4],%2:%3[%4])\n")
                                       .arg(dioVar->name())
                                       .arg(ioMapApplication.name())
                                       .arg(variable->name())
                                       .arg(i);
              }
            }
          }
        }
        initProgram->setCode("begin\n" + initProgramCode + "end");

        cs8DestApp->save();
        if (copyInProjectFolder) {
          QString sourceDir = cs8DestApp->projectPath();
          QString destDir =
              cs8SourceApp->projectPath() + "/" + cs8DestApp->name();
          QDir dir;
          dir.mkpath(destDir);
          dir.setPath(sourceDir);
          foreach (QString fileName,
                   dir.entryList(QStringList(), QDir::Files)) {
            QFile file;
            file.setFileName(destDir + "/" + fileName);
            file.remove();
            file.setFileName(sourceDir + "/" + fileName);
            file.copy(destDir + "/" + fileName);
          }

          // cs8DestApp->setCellPath (cs8SourceApps.at (0)->cellPath());
          // cs8DestApp->save ();
        }
        cs8DestApp->deleteLater();
      }
    }
  }
}

void MainWindow::createAPI() {
  ui->plainTextEdit->clear();
  QStringList dirs;
  QDir dir;

  for (int i = 0; i < ui->listWidget->count(); i++) {
    QString item = ui->listWidget->item(i)->text();
    QFileInfo info(item);
    if (info.isFile())
      dirs << info.absoluteFilePath();
    else {
      dir.setCurrent(item);
      foreach (QString pth, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
        dirs << dir.currentPath() + "/" + pth;
    }
  }

  QList<cs8Application *> cs8SourceApps;
  foreach (QString pth, dirs) {
    cs8Application *cs8SourceApp = new cs8Application(this);
    QFileInfo info(pth);
    if (info.isFile()) {
      if (cs8SourceApp->open(pth)) {
        emit addLog(tr("Opened source app %1").arg(pth));
        cs8SourceApps.append(cs8SourceApp);
      } else {
        emit addLog(tr("Opened source app %1 failed").arg(pth));
        delete cs8SourceApp;
      }
    } else {
      if (cs8SourceApp->openFromPathName(pth)) {
        emit addLog(tr("Opened source app %1").arg(pth));
        cs8SourceApps.append(cs8SourceApp);
      } else {
        emit addLog(tr("Opened source app %1 failed").arg(pth));
        delete cs8SourceApp;
      }
    }
  }

  createAPIs(cs8SourceApps, true);
  ui->commandLinkButton->setEnabled(true);
}

void MainWindow::on_actionExit_triggered() { close(); }

void MainWindow::slotAddLog(const QString &msg) {
  ui->plainTextEdit->appendPlainText(msg);
}

void MainWindow::on_actionAdd_Project_File_triggered() {
  QString filePath = QFileDialog::getOpenFileName(this, "Select Val3 Project",
                                                  m_recentLocation, "*.pjx");
  if (!filePath.isEmpty()) {
    // check if path is already in list
    if (ui->listWidget->findItems(filePath, Qt::MatchFixedString).count() > 0) {
      QMessageBox::warning(
          this, tr("Warning"),
          tr("The path %1 is already in the list!").arg(filePath));
      return;
    }
    m_recentLocation = filePath;
    ui->listWidget->addItem(filePath);
  }
}
