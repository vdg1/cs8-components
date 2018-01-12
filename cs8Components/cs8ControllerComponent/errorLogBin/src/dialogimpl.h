#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include <QDialog>
#include "ui_dialog.h"
//
class cs8Controller;
class cs8NetworkScanner;
class cs8Telnet;

class DialogImpl : public QDialog, public Ui::testWidget
{
    Q_OBJECT

  public:
    DialogImpl ( QWidget * parent = 0, Qt::WindowFlags f = 0 );

  private slots:

    void on_leHost_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

protected slots:
    void slotLoad();
    void slotHostListChanged();

  protected:
    cs8Controller* m_ctrl;
    cs8NetworkScanner* scanner;
    cs8Telnet* m_telnet;
};
#endif




