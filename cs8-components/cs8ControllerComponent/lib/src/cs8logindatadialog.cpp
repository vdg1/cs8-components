#include "cs8logindatadialog.h"

cs8LoginDataDialog::cs8LoginDataDialog(QWidget *parent, const QString & host, const QString & username, const QString & password)
    : QDialog(parent)
{
	ui.setupUi(this);
	ui.cbHostname->setEditText(host); 
	ui.leUserName->setText(username);
	ui.lePassword->setText(password);
}

cs8LoginDataDialog::~cs8LoginDataDialog()
{

} 

QString cs8LoginDataDialog::password()
{
	return ui.lePassword->text();
}

QString cs8LoginDataDialog::userName()
{
	return ui.leUserName->text();
}

QString cs8LoginDataDialog::hostName()
{
	return ui.cbHostname->currentText();
}
