#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFuture>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class cs8Application;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void createAPIs(QList<cs8Application *> cs8SourceApps,
                  bool copyInProjectFolder);
private slots:

  void on_actionAdd_Folder_triggered();

  void on_actionRemove_Folder_triggered();

  void on_commandLinkButton_clicked();

  void on_actionExit_triggered();

  void slotAddLog(const QString &msg);

  void on_actionAdd_Project_File_triggered();

private:
  Ui::MainWindow *ui;
  QFuture<void> future;
  QString m_recentLocation;

signals:
  void addLog(const QString &msg);

protected:
  void closeEvent(QCloseEvent *event);
  void readSettings();
  void writeSettings();
  void createAPI();
};

#endif // MAINWINDOW_H
