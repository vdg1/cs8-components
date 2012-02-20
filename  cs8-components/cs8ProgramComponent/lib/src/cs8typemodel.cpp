#include "cs8typemodel.h"

cs8TypeModel::cs8TypeModel(QObject *parent):
    cs8LibraryAliasModel(parent)
{
}

bool cs8TypeModel::add(const QDomElement & element,const QString & description)
{
    QString path = element.attribute("path");
    QString name = element.attribute("name");

    cs8LibraryAliasModel::add (name,path,true);
    return true;
}
