#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void processActivation(const QModelIndex &index);

  void on_actionPrecompiler_Settings_triggered();

  void slotStartup();

  void installAll();
  void uninstallAll();

  void on_tableWidget_doubleClicked(const QModelIndex &index);

  void on_actionAbout_Qt_triggered();

private:
  void storeSettings();
  void loadSettings();
  void closeEvent(QCloseEvent *event) override;
  Ui::MainWindow *ui;
  bool m_autoExit;
  void setup();
  // QString GetFileVersionOfApplication(const QString &fileName);
};

#endif // MAINWINDOW_H
