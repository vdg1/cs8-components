#include "mainwindow.h"
#include "dialogbuilddocumentation.h"
#include "dialogcopyrighteditor.h"
#include "dialogprojectdocumentationeditor.h"
#include "dialogsettings.h"
#include "ui_mainwindow.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);
  m_application = new cs8Application(this);
  connect(m_application, &cs8Application::modified, this,
          &MainWindow::slotModified);
  ui->listViewProgams->setModel(m_application->programModel());

  ui->tableViewPars->setMode(cs8ProgramDataView::ParameterData);
  ui->tableViewPars->setMasterView(ui->listViewProgams);

  ui->tableViewVars->setMode(cs8ProgramDataView::LocalData);
  ui->tableViewVars->setMasterView(ui->listViewProgams);

  ui->tableViewReferencedGlobalVaribales->setMode(
      cs8ProgramDataView::ReferencedGlobalData);
  ui->tableViewReferencedGlobalVaribales->setMasterView(ui->listViewProgams);

  ui->globalDataView->setMode(cs8ProgramDataView::GlobalData);
  ui->globalDataView->setMasterView(ui->listViewProgams);

  ui->widgetDocumentation->setMasterView(ui->listViewProgams);

  connect(ui->listViewProgams->selectionModel(),
          &QItemSelectionModel::selectionChanged, this,
          &MainWindow::slotSelectionChanged);

  readSettings();
  createRecentFilesItems();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave()) {
    writeSettings();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::changeEvent(QEvent *e) {
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    break;
  default:
    break;
  }
}

void MainWindow::readSettings() {
  QSettings settings;
  restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
  // create docks, toolbars, etc…
  restoreState(settings.value("mainWindowState").toByteArray());
  ui->splitter->restoreState(
      settings.value("mainWindowSplitter").toByteArray());
}

void MainWindow::writeSettings() {
  QSettings settings;
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());
  settings.setValue("mainWindowSplitter", ui->splitter->saveState());
}

void MainWindow::createRecentFilesItems() {
  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i] = new QAction(this);
    recentFileActs[i]->setVisible(false);
    connect(recentFileActs[i], SIGNAL(triggered()), this,
            SLOT(openRecentFile()));
  }
  for (int i = 0; i < MaxRecentFiles; ++i)
    ui->menuRecent->addAction(recentFileActs[i]);
  updateRecentFileActions();
}

void MainWindow::on_action_Open_triggered() {
  QString applicationName = QFileDialog::getOpenFileName(
      this, tr("Open a Val3 project"), m_application->cellProjectFilePath(),
      "(*.pjx)");
  if (!applicationName.isEmpty()) {
    openApplication(applicationName);
  }
}

void MainWindow::on_action_Save_triggered() { m_application->save(false); }

void MainWindow::slotSelectionChanged(const QItemSelection &selected,
                                      const QItemSelection & /*deselected*/) {
  int index = selected.indexes().at(0).row();
  cs8Program *program = m_application->programModel()->programList().at(index);
  ui->plainTextEditCode->setPlainText(program->toDocumentedCode());
  ui->labelDeclaration->setText(program->definition());
}

void MainWindow::openRecentFile() {
  if (maybeSave()) {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
      openApplication(action->data().toString());
  }
}

void MainWindow::slotModified(bool modified_) {
  setWindowModified(modified_);
  QModelIndexList l = ui->listViewProgams->selectionModel()->selectedIndexes();
  /// TODO
  if (l.count() > 0 && false) {
    int index = l.at(0).row();
    cs8Program *program =
        m_application->programModel()->programList().at(index);
    program->setDetailedDocumentation(
        ui->widgetDocumentation->documentation()->toMarkdown());
    ui->plainTextEditCode->setPlainText(program->toDocumentedCode());
  }
}

void MainWindow::on_tableViewPars_doubleClicked(QModelIndex index) {
  cs8Variable *var =
      qobject_cast<cs8VariableModel *>(ui->tableViewPars->model())
          ->variable(index);
  ui->detailEditor->setVariable(var);
  ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_detailEditor_done() {
  ui->stackedWidget->setCurrentIndex(0);
}

bool MainWindow::maybeSave() {
  if (m_application->isModified()) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard |
                                   QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
      return m_application->save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MainWindow::openApplication(const QString &applicationName) {
  setCurrentFile(applicationName);
  m_application->open(applicationName);
  ui->listViewProgams->setCurrentIndex(
      m_application->programModel()->index(0, 0));
  ui->actionAdd_tags_for_undocumented_symbols->setChecked(
      m_application->withUndocumentedSymbols());
  ui->actionInclude_documentation_of_Libraries->setChecked(
      m_application->includeLibraryDocuments());
}

void MainWindow::updateRecentFileActions() {
  QSettings settings;
  QStringList files = settings.value("recentFileList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

  for (int i = 0; i < numRecentFiles; ++i) {
    QString text = tr("&%1 %2").arg(i + 1).arg(files[i]);
    recentFileActs[i]->setText(text);
    recentFileActs[i]->setData(files[i]);
    recentFileActs[i]->setVisible(true);
  }
  for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    recentFileActs[j]->setVisible(false);
}

void MainWindow::setCurrentFile(const QString &fileName) {
  setWindowTitle(fileName + " [*]");
  QSettings settings;
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > MaxRecentFiles)
    files.removeLast();

  settings.setValue("recentFileList", files);

  foreach (QWidget *widget, QApplication::topLevelWidgets()) {
    MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
    if (mainWin)
      mainWin->updateRecentFileActions();
  }
}

QStringList scanDirIter(QDir dir) {
  QStringList projects;
  QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
  while (iterator.hasNext()) {
    iterator.next();
    // qDebug() << iterator.fileInfo ().absoluteFilePath ();
    if (!iterator.fileInfo().isDir()) {
      QString filename = iterator.fileName();
      if (filename.endsWith(".pjx")) {
        qDebug("Found %s matching pattern.", qPrintable(filename));
        projects << iterator.fileInfo().absoluteFilePath();
      }
    }
  }
  return projects;
}

void MainWindow::on_actionSettings_triggered() {
  DialogSettings dlg;
  dlg.exec();
}
