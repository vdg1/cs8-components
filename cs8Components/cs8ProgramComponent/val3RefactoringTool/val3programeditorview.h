#ifndef VAL3PROGRAMEDITORVIEW_H
#define VAL3PROGRAMEDITORVIEW_H

#include <QWidget>

namespace Ui {
class val3ProgramEditorView;
}

class val3ProgramEditorView : public QWidget
{
    Q_OBJECT

public:
    explicit val3ProgramEditorView(QWidget *parent = nullptr);
    ~val3ProgramEditorView();

private:
    Ui::val3ProgramEditorView *ui;
};

#endif // VAL3PROGRAMEDITORVIEW_H
