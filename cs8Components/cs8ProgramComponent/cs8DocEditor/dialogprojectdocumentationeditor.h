#ifndef DIALOGPROJECTDOCUMENTATIONEDITOR_H
#define DIALOGPROJECTDOCUMENTATIONEDITOR_H

#include <QDialog>

namespace Ui {
class DialogProjectDocumentationEditor;
}

class DialogProjectDocumentationEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProjectDocumentationEditor(QWidget *parent = 0);
    ~DialogProjectDocumentationEditor();
    void setText(const QString & text);
    QString text() const;
    void setTitle(const QString &text);

private:
    Ui::DialogProjectDocumentationEditor *ui;
};

#endif // DIALOGPROJECTDOCUMENTATIONEDITOR_H
