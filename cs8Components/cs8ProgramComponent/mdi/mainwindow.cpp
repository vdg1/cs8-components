/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QStandardPaths>
#include <QtWidgets>

#include "formmdichildtest.h"
#include "mainwindow.h"
#include "mdichild.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow()
    : QMainWindow(), ui(new Ui::MainWindow), m_application(nullptr) {
  ui->setupUi(this);
  // mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  // mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  // setCentralWidget(mdiArea);
  connect(ui->mdiArea, &QMdiArea::subWindowActivated, this,
          &MainWindow::updateMenus);

  createActions();
  createStatusBar();
  updateMenus();

  readSettings();

  setWindowTitle(tr("MDI"));
  setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  ui->mdiArea->closeAllSubWindows();
  if (ui->mdiArea->currentSubWindow()) {
    event->ignore();
  } else {
    writeSettings();
    event->accept();
  }
}

void MainWindow::open() {
  const QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open Val3 Application"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      "*.pjx");
  if (!fileName.isEmpty())
    openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName) {
  if (m_application)
    delete m_application;
  m_application = new cs8Application(this);
  if (m_application->open(fileName)) {
    prependToRecentFiles(fileName);
    ui->listViewProgams->setModel(m_application->programModel());
    ui->globalDataView->setModel(m_application->globalVariableModel());
    // connect(ui->listViewProgams->selectionModel(),
    //        &QItemSelectionModel::selectionChanged, this,
    //        &MainWindow::programSelectionChanged);
    connect(ui->listViewProgams, &QListView::clicked,
            [this](QModelIndex index) { programSelectionClicked(index); });
    return true;
  } else
    return false;
}

void MainWindow::programSelectionChanged(
    const QItemSelection &selected, const QItemSelection & /*deselected*/) {
  qDebug() << __FUNCTION__;
  const int index = selected.indexes().constFirst().row();
  cs8Program *program = m_application->programModel()->programList().at(index);
  if (program->view) {
    QMdiSubWindow *childView = qobject_cast<QMdiSubWindow *>(program->view);
    ui->mdiArea->setActiveSubWindow(childView);
  } else {
    createMdiChild(program);
  }
}

void MainWindow::programSelectionClicked(const QModelIndex index) {
  qDebug() << __FUNCTION__;
  /// TODO testing - add a simple sub window
  // FormMdiChildTest *child = new FormMdiChildTest(this);
  // auto c = ui->mdiArea->addSubWindow(child);
  // ui->mdiArea->setActiveSubWindow(c);
  // return;
  /// TODO ignore rest of function while testing
  cs8Program *program =
      m_application->programModel()->programList().at(index.row());
  if (program->view) {
    QMdiSubWindow *childView = qobject_cast<QMdiSubWindow *>(program->view);
    ui->mdiArea->setActiveSubWindow(childView);
  } else {
    auto c = createMdiChild(program);
    c->show();
  }
}

bool MainWindow::loadFile(const QString &fileName) {
  /*MdiChild *child = createMdiChild();
const bool succeeded = child->loadFile(fileName);
if (succeeded)
  child->show();
else
  child->close();
MainWindow::prependToRecentFiles(fileName);
return succeeded;
*/
  return true;
}

static inline QString recentFilesKey() {
  return QStringLiteral("recentFileList");
}
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings) {
  QStringList result;
  const int count = settings.beginReadArray(recentFilesKey());
  for (int i = 0; i < count; ++i) {
    settings.setArrayIndex(i);
    result.append(settings.value(fileKey()).toString());
  }
  settings.endArray();
  return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings) {
  const int count = files.size();
  settings.beginWriteArray(recentFilesKey());
  for (int i = 0; i < count; ++i) {
    settings.setArrayIndex(i);
    settings.setValue(fileKey(), files.at(i));
  }
  settings.endArray();
}

bool MainWindow::hasRecentFiles() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
  const int count = settings.beginReadArray(recentFilesKey());
  settings.endArray();
  return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName) {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());

  const QStringList oldRecentFiles = readRecentFiles(settings);
  QStringList recentFiles = oldRecentFiles;
  recentFiles.removeAll(fileName);
  recentFiles.prepend(fileName);
  if (oldRecentFiles != recentFiles)
    writeRecentFiles(recentFiles, settings);

  setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible) {
  recentFileSubMenuAct->setVisible(visible);
  recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());

  const QStringList recentFiles = readRecentFiles(settings);
  const int count = qMin(int(MaxRecentFiles), recentFiles.size());
  int i = 0;
  for (; i < count; ++i) {
    const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
    recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
    recentFileActs[i]->setData(recentFiles.at(i));
    recentFileActs[i]->setVisible(true);
  }
  for (; i < MaxRecentFiles; ++i)
    recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile() {
  if (const QAction *action = qobject_cast<const QAction *>(sender()))
    openFile(action->data().toString());
}

void MainWindow::save() {
  // if (activeMdiChild() && activeMdiChild()->save())
  //   statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs() {}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut() {
  if (activeMdiChild())
    activeMdiChild()->editor()->cut();
}

void MainWindow::copy() {
  if (activeMdiChild())
    activeMdiChild()->editor()->copy();
}

void MainWindow::paste() {
  if (activeMdiChild())
    activeMdiChild()->editor()->paste();
}
#endif

void MainWindow::about() {
  QMessageBox::about(
      this, tr("About MDI"),
      tr("The <b>MDI</b> example demonstrates how to write multiple "
         "document interface applications using Qt."));
}

void MainWindow::updateMenus() {
  qDebug() << __FUNCTION__;
  bool hasMdiChild = (activeMdiChild() != nullptr);
  saveAct->setEnabled(hasMdiChild);
  saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
  pasteAct->setEnabled(hasMdiChild);
#endif
  closeAct->setEnabled(hasMdiChild);
  closeAllAct->setEnabled(hasMdiChild);
  tileAct->setEnabled(hasMdiChild);
  cascadeAct->setEnabled(hasMdiChild);
  nextAct->setEnabled(hasMdiChild);
  previousAct->setEnabled(hasMdiChild);
  windowMenuSeparatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
  bool hasSelection = (activeMdiChild() &&
                       activeMdiChild()->editor()->textCursor().hasSelection());
  cutAct->setEnabled(hasSelection);
  copyAct->setEnabled(hasSelection);
#endif
}

void MainWindow::updateWindowMenu() {
  windowMenu->clear();
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);
  windowMenu->addAction(windowMenuSeparatorAct);

  QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
  windowMenuSeparatorAct->setVisible(!windows.isEmpty());

  for (int i = 0; i < windows.size(); ++i) {
    QMdiSubWindow *mdiSubWindow = windows.at(i);
    MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

    QString text;
    if (i < 9) {
      text = tr("&%1 %2").arg(i + 1).arg(child->program()->name());
    } else {
      text = tr("%1 %2").arg(i + 1).arg(child->program()->name());
    }
    QAction *action =
        windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
          ui->mdiArea->setActiveSubWindow(mdiSubWindow);
        });
    action->setCheckable(true);
    action->setChecked(child == activeMdiChild());
  }
}

MdiChild *MainWindow::createMdiChild(cs8Program *program) {
  MdiChild *child = new MdiChild(this);
  child->setProgram(program);
  auto c = ui->mdiArea->addSubWindow(child);
  ui->mdiArea->setActiveSubWindow(c);

#ifndef QT_NO_CLIPBOARD
  connect(child->editor(), &QPlainTextEdit::copyAvailable, cutAct,
          &QAction::setEnabled);
  connect(child->editor(), &QPlainTextEdit::copyAvailable, copyAct,
          &QAction::setEnabled);
#endif

  return child;
}

void MainWindow::createActions() {
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  QToolBar *fileToolBar = addToolBar(tr("File"));

  const QIcon newIcon =
      QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
  newAct = new QAction(newIcon, tr("&New"), this);
  newAct->setShortcuts(QKeySequence::New);
  newAct->setStatusTip(tr("Create a new file"));
  // connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
  fileMenu->addAction(newAct);
  fileToolBar->addAction(newAct);

  const QIcon openIcon =
      QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, &QAction::triggered, this, &MainWindow::open);
  fileMenu->addAction(openAct);
  fileToolBar->addAction(openAct);

  const QIcon saveIcon =
      QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
  saveAct = new QAction(saveIcon, tr("&Save"), this);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::save);
  fileToolBar->addAction(saveAct);

  const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
  saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
  fileMenu->addAction(saveAsAct);

  fileMenu->addSeparator();

  QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
  connect(recentMenu, &QMenu::aboutToShow, this,
          &MainWindow::updateRecentFileActions);
  recentFileSubMenuAct = recentMenu->menuAction();

  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileActs[i] =
        recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
    recentFileActs[i]->setVisible(false);
  }

  recentFileSeparator = fileMenu->addSeparator();

  setRecentFilesVisible(MainWindow::hasRecentFiles());

  //! [0]
  const QIcon exitIcon = QIcon::fromTheme("application-exit");
  QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp,
                                         &QApplication::closeAllWindows);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  fileMenu->addAction(exitAct);
  //! [0]

#ifndef QT_NO_CLIPBOARD
  QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
  QToolBar *editToolBar = addToolBar(tr("Edit"));

  const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
  cutAct = new QAction(cutIcon, tr("Cu&t"), this);
  cutAct->setShortcuts(QKeySequence::Cut);
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                          "clipboard"));
  connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
  editMenu->addAction(cutAct);
  editToolBar->addAction(cutAct);

  const QIcon copyIcon =
      QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
  copyAct = new QAction(copyIcon, tr("&Copy"), this);
  copyAct->setShortcuts(QKeySequence::Copy);
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
  connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
  editMenu->addAction(copyAct);
  editToolBar->addAction(copyAct);

  const QIcon pasteIcon =
      QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
  pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
  pasteAct->setShortcuts(QKeySequence::Paste);
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                            "selection"));
  connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
  editMenu->addAction(pasteAct);
  editToolBar->addAction(pasteAct);
#endif

  windowMenu = menuBar()->addMenu(tr("&Window"));
  connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

  closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setStatusTip(tr("Close the active window"));
  connect(closeAct, &QAction::triggered, ui->mdiArea,
          &QMdiArea::closeActiveSubWindow);

  closeAllAct = new QAction(tr("Close &All"), this);
  closeAllAct->setStatusTip(tr("Close all the windows"));
  connect(closeAllAct, &QAction::triggered, ui->mdiArea,
          &QMdiArea::closeAllSubWindows);

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, &QAction::triggered, ui->mdiArea, &QMdiArea::tileSubWindows);

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, &QAction::triggered, ui->mdiArea,
          &QMdiArea::cascadeSubWindows);

  nextAct = new QAction(tr("Ne&xt"), this);
  nextAct->setShortcuts(QKeySequence::NextChild);
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, &QAction::triggered, ui->mdiArea,
          &QMdiArea::activateNextSubWindow);

  previousAct = new QAction(tr("Pre&vious"), this);
  previousAct->setShortcuts(QKeySequence::PreviousChild);
  previousAct->setStatusTip(tr("Move the focus to the previous "
                               "window"));
  connect(previousAct, &QAction::triggered, ui->mdiArea,
          &QMdiArea::activatePreviousSubWindow);

  windowMenuSeparatorAct = new QAction(this);
  windowMenuSeparatorAct->setSeparator(true);

  updateWindowMenu();

  menuBar()->addSeparator();

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

  QAction *aboutAct =
      helpMenu->addAction(tr("&About"), this, &MainWindow::about);
  aboutAct->setStatusTip(tr("Show the application's About box"));

  QAction *aboutQtAct =
      helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
}

void MainWindow::createStatusBar() { statusBar()->showMessage(tr("Ready")); }

void MainWindow::readSettings() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
  const QByteArray geometry =
      settings.value("geometry", QByteArray()).toByteArray();
  if (geometry.isEmpty()) {
    const QRect availableGeometry = screen()->availableGeometry();
    resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
    move((availableGeometry.width() - width()) / 2,
         (availableGeometry.height() - height()) / 2);
  } else {
    restoreGeometry(geometry);
  }
}

void MainWindow::writeSettings() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
  settings.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const {
  qDebug() << __FUNCTION__;
  if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    return qobject_cast<MdiChild *>(activeSubWindow->widget());
  return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const {
  qDebug() << __FUNCTION__;
  /*
   * QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  const QList<QMdiSubWindow *> subWindows = ui->mdiArea->subWindowList();
  for (QMdiSubWindow *window : subWindows) {
    MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
    if (mdiChild->currentFile() == canonicalFilePath)
      return window;
  }
*/
  return nullptr;
}