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
    explicit DialogSettings(QWidget *parent = nullptr);
    ~DialogSettings();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_toolButton_clicked();

    void on_checkBox_stateChanged(int arg1);

private:
    Ui::DialogSettings *ui;
};

#endif // DIALOGSETTINGS_H
