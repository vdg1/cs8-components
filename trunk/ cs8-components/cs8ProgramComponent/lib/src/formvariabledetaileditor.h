#ifndef FORMVARIABLEDETAILEDITOR_H
#define FORMVARIABLEDETAILEDITOR_H

#include <QWidget>
#include "cs8variable.h"

namespace Ui {
    class formVariableDetailEditor;
}

class formVariableDetailEditor : public QWidget {
    Q_OBJECT
public:
    formVariableDetailEditor(QWidget *parent = 0);
    ~formVariableDetailEditor();
    void setVariable(cs8Variable *var);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::formVariableDetailEditor *ui;
    cs8Variable* m_variable;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

signals:
    void done();
};

#endif // FORMVARIABLEDETAILEDITOR_H
