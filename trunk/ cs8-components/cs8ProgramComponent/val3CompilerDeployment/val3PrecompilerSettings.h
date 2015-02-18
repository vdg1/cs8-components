#ifndef VAL3PRECOMPILERSETTINGS_H
#define VAL3PRECOMPILERSETTINGS_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QDialog>

#include <QSettings>
#endif

namespace Ui {
class val3PrecompilerSettings;
}

class val3PrecompilerSettings : public QDialog
{
        Q_OBJECT

    public:
        explicit val3PrecompilerSettings(QDialog *parent = 0);
        ~val3PrecompilerSettings();

    protected:
        void changeEvent(QEvent *e);

    private slots:
        void on_cpUnusedPublicGlobalVar_clicked(bool checked);

        void on_buttonBox_accepted();

        void on_buttonBox_rejected();

        void on_cpHiddenGlobalVariables_toggled(bool checked);

        void on_cpParametersPostfix_toggled(bool checked);

        void on_cpReportsToDos_toggled(bool checked);

private:
        Ui::val3PrecompilerSettings *ui;

};

#endif // VAL3PRECOMPILERSETTINGS_H
