#ifndef FORMMARKDOWNEDITOR_H
#define FORMMARKDOWNEDITOR_H

#include "document.h"
#include <QPlainTextEdit>
#include <QWidget>

namespace Ui {
class FormMarkDownEditor;
}

class FormMarkDownEditor : public QWidget
{
    Q_OBJECT

public:
    explicit FormMarkDownEditor(QWidget *parent = 0);
    ~FormMarkDownEditor();

    void setText(const QString & text);
    QString text() const;



    QString prefixText() const;
    void setPrefixText(const QString &prefixText);

private:
    Ui::FormMarkDownEditor *ui;
    QString m_prefixText;
    Document m_content;
};

#endif // FORMMARKDOWNEDITOR_H
