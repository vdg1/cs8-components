#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class cs8FileBrowserModel;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  cs8FileBrowserModel *m_localBrowser;
  cs8FileBrowserModel *m_remoteBrowser;
};

#endif // MAINWINDOW_H
