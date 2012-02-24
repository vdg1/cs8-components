#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include "cs8application.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);
    bool maybeSave();
    void openApplication(const QString & applicationName);
    void readSettings();
    void writeSettings();
    void createRecentFilesItems();
    void setCurrentFile(const QString &fileName);
    cs8Application* m_application;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    void updateRecentFileActions();

protected slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void slotSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected );
    void openRecentFile();

private:
    Ui::MainWindow *ui;


private slots:
    void slotModified(bool modified_);
    void on_tableViewPars_doubleClicked(QModelIndex index);
    void on_detailEditor_done();
};

#endif // MAINWINDOW_H
