#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include <QDialog>
#include "ui_dialog.h"
//
class DialogImpl : public QDialog, public Ui::licenseMakerWidget
{
Q_OBJECT
protected slots:
	void slotKeyChanged();
	void slotPrint();
	void slotMachineNumberChanged();
	void slotCreateLicense();
public:
	DialogImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	
	protected:
	QString licenseText;
private slots:
};
#endif




