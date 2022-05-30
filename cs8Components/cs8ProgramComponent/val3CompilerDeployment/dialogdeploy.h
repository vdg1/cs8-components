#ifndef DIALOGDEPLOY_H
#define DIALOGDEPLOY_H

#include <QDialog>

namespace Ui {
class Dialog;
}


class DialogDeploy : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDeploy(QWidget *parent = 0);
    ~DialogDeploy();


    QString path() const;
    void setPath(const QString &path);

    bool activatePreCompiler(bool activate);
    void activateHelpFile(bool activate);

private:
    Ui::Dialog *ui;
    QString m_path;
};

extern QString VAL3CHECKORIG;
extern QString VAL3CHECK;
extern QString HELPCHECKORIG;
extern QString HELPCHECK;

#endif // DIALOGDEPLOY_H
