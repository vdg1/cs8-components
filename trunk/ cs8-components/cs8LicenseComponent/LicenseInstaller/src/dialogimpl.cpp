#include <QMessageBox>
#include <QUrl>
#include <QFile>
#include <QLineEdit>
#include <QDebug>
#include <QDomDocument>
#include <QTextStream>

#include "dialogimpl.h"
#include "cs8license.h"
//
DialogImpl::DialogImpl ( QWidget * parent, Qt::WFlags f )
		: QDialog ( parent, f )
{
	setupUi ( this );
	connect ( leMachineNumber,SIGNAL ( textChanged ( const QString& ) ),
	          this,SLOT ( slotChanged() ) );
	connect ( leMachineNumber,SIGNAL ( textChanged ( const QString& ) ),
	          this,SLOT ( slotMachineNumberChanged() ) );
	connect ( leKey,SIGNAL ( textChanged ( const QString& ) ),
	          this,SLOT ( slotChanged() ) );
}
//

void DialogImpl::on_btInstall_clicked()
{
	if ( QMessageBox::question ( this,
	                             tr ( "Confirmation" ),
	                             tr ( "Are you sure to transmit the license information to the controller?" ),
	                             QMessageBox::Yes | QMessageBox::No ) ==QMessageBox::Yes )
	{
		QUrl url ( QString ( "cs8://default@%1/usr/usrapp/z01_SysData/z01_SysData.dtx" )
		           .arg ( leAddress->text() ) );
		{
			QFile file;
			file.setFileName ( url.toString() );
			if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
			{
				if ( file.error() ==1 )
				{
					url.setUrl ( QString ( "cs8://default@%1/usr/usrapp/zData/zData.dtx" )
					             .arg ( leAddress->text() ) );
					file.setFileName ( url.toString() );
					if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
					{
						QMessageBox::warning ( this,
						                       tr ( "Error opening file" ),
						                       tr ( "File %3 could not be opened:\n (%1) %2" )
						                       .arg ( file.error() )
						                       .arg ( file.errorString() )
						                       .arg ( url.toString() ) );
						return;
					}
				}
				else
				{
					QMessageBox::warning ( this,
					                       tr ( "Error opening file" ),
					                       tr ( "File %3 could not be opened:\n (%1) %2" )
					                       .arg ( file.error() )
					                       .arg ( file.errorString() )
					                       .arg ( url.toString() ) );
					return;
				}
			}

			doc.setContent ( &file );
			file.close();
		}
		qDebug() << doc.toString();
		cs8License::setLicenseInformation ( doc,leMachineNumber->text(),leKey->text() );
		qDebug() << doc.toString();
		QFile file;
		file.setFileName ( url.toString() );

		if ( !file.open ( QIODevice::WriteOnly ) )
		{
			QMessageBox::warning ( this,
			                       tr ( "Error opening file" ),
			                       tr ( "File %3 could not be opened:\n (%1) %2" )
			                       .arg ( file.error() )
			                       .arg ( file.errorString() )
			                       .arg ( url.toString() ) );
			return;
		}
		QString text=doc.toString();
		QTextStream out ( &file );
		out << text;
		if ( file.error() !=0 )
			QMessageBox::warning ( this,
			                       tr ( "Error opening file" ),
			                       tr ( "File %3 could not be saved on the controller:\n (%1) %2" )
			                       .arg ( file.error() )
			                       .arg ( file.errorString() )
			                       .arg ( url.toString() ) );
		else
			QMessageBox::information ( this,
			                           tr ( "Installation complete" ),
			                           tr ( "The license information was sucessfully installed on the flash drive of the controller.\nPlease reload the controller." ) );
		file.close();
	}
}


/*!
    \fn DialogImpl::slotChanged()
 */
void DialogImpl::slotChanged()
{
	btInstall->setEnabled ( leKey->text() ==cs8License::createLicense ( leMachineNumber->text() ) );
}


/*!
    \fn DialogImpl::slotMachineNumberChanged()
 */
void DialogImpl::slotMachineNumberChanged()
{
	QString mask;
	QStringList keys=cs8License::createLicense ( leMachineNumber->text() ).split ( "/",QString::SkipEmptyParts );
	int i=1;
	for ( i=1;i<leMachineNumber->text().length();i++ )
	{
		mask+=QString ( "00000000" ).left ( keys[i-1].length() ) +"/";
	}
	mask+=QString ( "00000000" ).left ( keys[i-1].length() );
	leKey->setInputMask ( mask );
}
