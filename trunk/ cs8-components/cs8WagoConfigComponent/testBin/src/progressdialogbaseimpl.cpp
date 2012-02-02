#include "progressdialogbaseimpl.h"
#include <QDebug>
//
progressDialogBaseImpl::progressDialogBaseImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f)
{
	setupUi(this);
}
//


void progressDialogBaseImpl::setText(const QString & text)
{
	label->setText(text);
}

void progressDialogBaseImpl::show()
{
	qDebug() << "progressDialogBaseImpl::show()";
	QDialog::show();
}
void progressDialogBaseImpl::close()
{
	qDebug() << "progressDialogBaseImpl::close()";
		QDialog::close();
}

