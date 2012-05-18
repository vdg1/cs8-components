#ifndef PASSWORDIMPL_H
#define PASSWORDIMPL_H
//
#include <QDialog>
#include "../ui_password.h"
//
class PasswordImpl : public QDialog, public Ui::Password
{
Q_OBJECT
public:
    PasswordImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
private slots:
    void accepted();
    void rejected();
};
#endif





