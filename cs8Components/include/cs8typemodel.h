#ifndef CS8TYPEMODEL_H
#define CS8TYPEMODEL_H

#include "cs8libraryaliasmodel.h"

class cs8TypeModel : public cs8LibraryAliasModel
{
public:
    cs8TypeModel(QObject* parent=0);
    bool add(const QDomElement & element,const QString & description="");
    bool add(const QString & aliasName, const QString &path, const bool autoLoad=true);
};

#endif // CS8TYPEMODEL_H
