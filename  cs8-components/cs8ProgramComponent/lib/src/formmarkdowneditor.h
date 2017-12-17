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

    QString text() const;

    QString prefixText() const;
    QString postfixText() const;
    QString previewText() const;

public slots:
    void setPostfixText(const QString &postfixText);
    void setText(const QString & text, bool blockSignal);
    void setPrefixText(const QString &prefixText);

signals:
    void textChanged(const QString &text);
    void textEdited(const QString &text);

private:
    Ui::FormMarkDownEditor *ui;
    QString m_prefixText;
    QString m_postfixText;
    Document m_content;
};

#endif // FORMMARKDOWNEDITOR_H
