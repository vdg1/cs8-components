#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include <QDialog>
#include "ui_dialog.h"
//
class cs8Controller;
class cs8NetworkScanner;

class DialogImpl : public QDialog, public Ui::testWidget
{
    Q_OBJECT

  public:
    DialogImpl ( QWidget * parent = 0, Qt::WFlags f = 0 );

  private slots:

  protected slots:
    void slotLoad();
    void slotHostListChanged();

  protected:
    cs8Controller* m_ctrl;
    cs8NetworkScanner* scanner;
};
#endif




