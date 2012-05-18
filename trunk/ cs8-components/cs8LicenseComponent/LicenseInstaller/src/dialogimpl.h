#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include <QDialog>
#include <QDomDocument>
#include "../ui_dialog.h"
//
class DialogImpl : public QDialog, public Ui::LicenseInstallerWidgetBase
{
    Q_OBJECT
  public:
    DialogImpl ( QWidget * parent = 0, Qt::WFlags f = 0 );
  protected:
    QDomDocument doc;
  private slots:
    void on_btInstall_clicked();
protected slots:
    void slotChanged();
    void slotMachineNumberChanged();
};
#endif




