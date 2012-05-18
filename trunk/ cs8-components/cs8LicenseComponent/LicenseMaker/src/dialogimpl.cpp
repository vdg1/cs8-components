#include "dialogimpl.h"
#include "cs8license.h"

#include <QDebug>
#include <QTextEdit>
#include <QFile>
#include <QPrinter>
#include <QPrintDialog>
//
DialogImpl::DialogImpl ( QWidget * parent, Qt::WFlags f )
    : QDialog ( parent, f )
{
  setupUi ( this );
  connect ( btCreate,SIGNAL ( clicked() ),this,SLOT ( slotCreateLicense() ) );
  connect ( btPrint,SIGNAL ( clicked() ),this,SLOT ( slotPrint() ) );
  connect ( leMachineNumber,SIGNAL ( textEdited ( QString ) ),this,SLOT ( slotMachineNumberChanged() ) );
  connect ( leKey,SIGNAL ( textChanged ( QString ) ),this,SLOT ( slotKeyChanged() ) );

  /*
  QFile file ( ":/template.htm" );
  if ( file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
    licenseText=file.readAll();
  file.close();
  */
  licenseText = "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
"p, li { white-space: pre-wrap; }"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">"
"<center><img src=\":/splash.png\"/></img><p></center>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:10pt; font-weight:600; color:#b22222;\">Swiss System License Information</span></p>"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:600;\"></p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:600;\">Software:		<span style=\" font-weight:400;\">%Software%</span></p>"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><span style=\" font-weight:600;\">Machine Number:	</span>%MachineNumber%</p>"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><span style=\" font-weight:600;\">License Key:	</span>%LicenseKey%</p></body></html>";
}
//

void DialogImpl::slotCreateLicense()
{
  QString license=cs8License::createLicense ( leMachineNumber->text() );
  leKey->setText ( license );
  QFile file(":\\LicenseTemp.txt");
  file.open (QFile::ReadOnly);
  QString license_=file.readAll ();
  license_.replace ( "%Software%","SAXE Automation System" );
  license_.replace ( "%MachineNumber%",leMachineNumber->text() );
  license_.replace ( "%LicenseKey%",license );
  textEdit->setText ( license_ );

}

void DialogImpl::slotMachineNumberChanged()
{
  btCreate->setEnabled ( leMachineNumber->text().length() >0 );
}

void DialogImpl::slotPrint()
{
  QPrinter printer;

  QPrintDialog *dialog = new QPrintDialog ( &printer, this );
  dialog->setWindowTitle ( tr ( "Print Document" ) );
  if ( textEdit->textCursor().hasSelection() )
    dialog->addEnabledOption ( QAbstractPrintDialog::PrintSelection );
  if ( dialog->exec() != QDialog::Accepted )
    return;
  textEdit->print ( &printer );
}

void DialogImpl::slotKeyChanged()
{
  btPrint->setEnabled ( leKey->text().length() >0 );
}

