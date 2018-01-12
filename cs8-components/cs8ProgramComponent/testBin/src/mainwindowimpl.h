#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"
//
class cs8Project;
class MainWindowImpl: public QMainWindow, public Ui::MainWindow {
Q_OBJECT
protected slots:
	void on_action_Save_triggered();
	void on_action_Open_triggered();
	void on_action_Export_triggered();
	void on_action_Open_Doxyfile_triggered();
	void on_action_Export_Prototype_triggered();
	void slotActivated(const QModelIndex & index);
public:
	MainWindowImpl(QWidget * parent = 0, Qt::WFlags f = 0);
protected:
	cs8Project* project;
protected:
	void saveState();
	void restoreState();
	void closeEvent(QCloseEvent *event);
};
#endif

