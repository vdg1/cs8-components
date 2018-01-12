#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogSettings;
}

class DialogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSettings(QWidget *parent = 0);
    ~DialogSettings();

private slots:
    void on_toolButton_clicked();


    void on_toolButtonHHC_clicked();

    void on_toolButtonGraphviz_clicked();

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
