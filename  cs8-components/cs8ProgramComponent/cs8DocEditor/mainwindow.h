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
    cs8Application* m_application;

protected slots:
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void slotSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected );

private:
    Ui::MainWindow *ui;

private slots:
    void on_tableViewPars_doubleClicked(QModelIndex index);
    void on_detailEditor_done();
};

#endif // MAINWINDOW_H
