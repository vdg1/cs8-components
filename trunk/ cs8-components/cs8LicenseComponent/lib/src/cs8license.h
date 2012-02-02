#ifndef CS8LICENSE_H
#define CS8LICENSE_H
//

#include <QByteArray>
#include <QString>
#include <QDomDocument>
//
class cs8License  
{

public:
	static bool setLicenseInformation(QDomDocument doc, const QString & machineNumber, const QString & key);
	static QString createLicense(const QString & serialNumber);
	cs8License();
	
};
#endif
