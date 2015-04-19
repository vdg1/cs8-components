#ifndef DIALOGBUILDDOCUMENTATION_H
#define DIALOGBUILDDOCUMENTATION_H

#include <QDialog>
#include <QProcess>
#include <QTemporaryFile>
#include <cs8application.h>

namespace Ui {
class DialogBuildDocumentation;
}

class DialogBuildDocumentation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBuildDocumentation(QWidget *parent = 0);
    ~DialogBuildDocumentation();
    void build(cs8Application *application);

private:
    Ui::DialogBuildDocumentation *ui;
    QProcess *doxygenProcess;
    QTemporaryFile *configFile;

private slots:
    void slotProcessError(QProcess::ProcessError);
    void slotFinished(int);
    void slotReadyRead();
};

#endif // DIALOGBUILDDOCUMENTATION_H
