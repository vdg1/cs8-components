#ifndef DIALOGCOPYRIGHTEDITOR_H
#define DIALOGCOPYRIGHTEDITOR_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QDialog>
#else
#include <QtGui/QDialog>
#endif

namespace Ui {
class DialogCopyRightEditor;
}

class DialogCopyRightEditor : public QDialog
{
        Q_OBJECT

    public:
        explicit DialogCopyRightEditor(QWidget *parent = 0);
        ~DialogCopyRightEditor();

        QString copyRightText() const;
        void setCopyRightText(const QString &copyRightText);

    protected:
        void changeEvent(QEvent *e);

    private:
        Ui::DialogCopyRightEditor *ui;
};

#endif // DIALOGCOPYRIGHTEDITOR_H
