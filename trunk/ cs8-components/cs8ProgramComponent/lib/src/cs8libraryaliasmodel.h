/*
 * cs8libraryaliasmodel.h
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#ifndef CS8LIBRARYALIASMODEL_H_
#define CS8LIBRARYALIASMODEL_H_

#include "cs8libraryalias.h"
#include <QAbstractTableModel>
#include <QDomElement>

class cs8LibraryAliasModel: public QAbstractTableModel {

    Q_OBJECT

protected:
    QList<cs8LibraryAlias*> m_aliasList;
    uint m_projectVersion;

public:
    QString toDocumentedCode();
    QVariant headerData(int section, Qt::Orientation orientation, int role =
			Qt::DisplayRole) const;
    int rowCount(const QModelIndex& index) const;
    QVariant data(const QModelIndex & index, int role) const;
    int columnCount(const QModelIndex & index) const;
    bool addAlias(const QDomElement & element, const QString & description =
                  QString());
    cs8LibraryAliasModel(QObject * parent = 0);
    bool setData(const QModelIndex & index, const QVariant & value, int role =
                 Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;
    cs8LibraryAlias* getAliasByName(const QString & name);
    QList<cs8LibraryAlias*> aliasList() {
        return m_aliasList;
    }
    void clear();
    void setProjectVersion(uint version){m_projectVersion=version;}
};

#endif /* CS8LIBRARYALIASMODEL_H_ */
