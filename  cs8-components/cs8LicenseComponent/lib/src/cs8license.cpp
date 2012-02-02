#include "cs8license.h"

#include <QList>
#include <QStringList>
#include <QDebug>
//
cs8License::cs8License(  )
{
  // TODO
}
//

QString cs8License::createLicense ( const QString & serialNumber )
{
  QList<int> keyList;
  QString keyString;
  for ( int i=0;i<serialNumber.length();i++ ) {
    int keyChar=serialNumber.toAscii().at ( i );
    keyList.append ( keyChar* ( i+1 ) );
    keyString+=QString ( "%1/" ).arg ( keyList.at ( i ) );
  }
  keyString.chop ( 1 );
  return keyString;
}

bool cs8License::setLicenseInformation ( QDomDocument doc, const QString & machineNumber, const QString & key )
{
  QDomElement root=doc.documentElement();
  QDomElement numSection=doc.elementsByTagName ( "numSection" ).at ( 0 ).toElement();
  
  QDomNode n = numSection.firstChild();
  while ( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if ( !e.isNull() ) {
      if ( e.attribute ( "name" ) =="nLic" ) {
        // remove existing key from dom
        while ( e.hasChildNodes() ) {
          qDebug() << e.firstChild().toElement().attribute ( "value" );
          e.removeChild ( e.firstChild() );
        }
        
        // add new key to dom
        QStringList keys=key.split ( "/",QString::SkipEmptyParts );
        int i=0;
        foreach ( QString part,keys ) {
          QDomElement newElement=doc.createElement ( "valueNum" );
          newElement.setAttribute ( "index",i++ );
          newElement.setAttribute ( "value",part );
          e.appendChild ( newElement );
        }
        break;
      }
    }
    n = n.nextSibling();
  }
  
  QDomElement stringSection=doc.elementsByTagName ( "stringSection" ).at ( 0 ).toElement();
  
  n = stringSection.firstChild();
  while ( !n.isNull() ) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if ( !e.isNull() ) {
      if ( e.attribute ( "name" ) =="sMachineNumber" ) {
        e.firstChild().toElement().setAttribute ( "value",machineNumber );
        break;
      }
    }
    n = n.nextSibling();
  }
  return true;
}


