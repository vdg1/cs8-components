#ifndef CS8LOGINDATADIALOG_H
#define CS8LOGINDATADIALOG_H

#include <QDialog>
#include "../build/ui_cs8logindatadialog.h"

class cs8LoginDataDialog : public QDialog
{
    Q_OBJECT

public:
    cs8LoginDataDialog(QWidget *parent = 0, const QString & host=QString(), const QString & username=QString(), const QString & password=QString());
    ~cs8LoginDataDialog();
    QString password();
    QString userName();
    QString hostName();

private:
    Ui::cs8LoginDataDialogClass ui;
};

#endif // CS8LOGINDATADIALOG_H
