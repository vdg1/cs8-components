#include "cs8typemodel.h"

cs8TypeModel::cs8TypeModel(QObject *parent):
    cs8LibraryAliasModel(parent)
{
}

bool cs8TypeModel::add(const QDomElement & element,const QString & /*description*/)
{
    QString path = element.attribute("path");
    QString name = element.attribute("name");

    cs8LibraryAliasModel::add (name,path,true);
    return true;
}

bool cs8TypeModel::add(const QString &aliasName, const QString &path, const bool autoLoad)
{
    return cs8LibraryAliasModel::add (aliasName, path, autoLoad);
}
