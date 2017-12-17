#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QtGui/QMainWindow>
#include "ui_mainwindowimpl.h"

class MainWindowImpl: public QMainWindow, private Ui::MainWindowImplClass {
Q_OBJECT

public:
	MainWindowImpl(QWidget *parent = 0);
	~MainWindowImpl();

protected slots:
	void on_toolButtonSource_clicked();
	void on_toolButtonDest_clicked();
	void on_toolButtonCDest_clicked();
	void on_pushButtonExecute_clicked();
};

#endif // MAINWINDOWIMPL_H
