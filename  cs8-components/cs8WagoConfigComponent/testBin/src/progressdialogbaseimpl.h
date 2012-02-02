#ifndef PROGRESSDIALOGBASEIMPL_H
#define PROGRESSDIALOGBASEIMPL_H
//
#include <QDialog>
#include "ui_progressWidget.h"
//
class progressDialogBaseImpl : public QDialog, public Ui::progressDialogBase
{
    Q_OBJECT
  public slots:
    void setText ( const QString & text );
  public:
	  
    progressDialogBaseImpl ( QWidget * parent = 0, Qt::WFlags f = 0 );
    virtual void show();
    virtual void close();
  private slots:
};
#endif





