#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include "ui_dialog.h"
#include <QDialog>
//
class cs8Controller;
class cs8NetworkScanner;
class cs8Telnet;
class cs8FileBrowserModel;

class DialogImpl : public QDialog, public Ui::testWidget {
  Q_OBJECT

public:
  DialogImpl(QWidget *parent = 0, Qt::WindowFlags f = 0);

private slots:

  void on_leHost_textChanged(const QString &arg1);

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

protected slots:
  void slotLoad();
  void slotHostListChanged();

protected:
  cs8Controller *m_ctrl;
  cs8NetworkScanner *scanner;
  cs8Telnet *m_telnet;
  cs8FileBrowserModel *m_browser;
};
#endif
