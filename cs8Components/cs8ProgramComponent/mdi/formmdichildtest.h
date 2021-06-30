#ifndef FORMMDICHILDTEST_H
#define FORMMDICHILDTEST_H

#include <QWidget>

namespace Ui {
class FormMdiChildTest;
}

class FormMdiChildTest : public QWidget
{
    Q_OBJECT

public:
    explicit FormMdiChildTest(QWidget *parent = nullptr);
    ~FormMdiChildTest();

private:
    Ui::FormMdiChildTest *ui;
};

#endif // FORMMDICHILDTEST_H
