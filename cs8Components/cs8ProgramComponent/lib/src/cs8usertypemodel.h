/*
 * cs8libraryaliasmodel.h
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#ifndef CS8USERTYPEMODEL_H_
#define CS8USERTYPEMODEL_H_

#include "cs8usertype.h"
#include <QAbstractTableModel>
#include <QDomElement>

class cs8UserTypeModel: public QAbstractTableModel {

    Q_OBJECT

protected:
    QList<cs8UserType*> m_aliasList;
    uint m_projectVersion;

public:
    QString toDocumentedCode();
    QVariant headerData(int section, Qt::Orientation orientation, int role =
            Qt::DisplayRole) const;
    int rowCount(const QModelIndex& index) const;
    QVariant data(const QModelIndex & index, int role) const;
    int columnCount(const QModelIndex & index) const;
    bool add(const QDomElement & element, const QString & description =
            QString());
    bool add(const QString & aliasName, const QString &path, const bool autoLoad=true);
    cs8UserTypeModel(QObject * parent = 0);
    bool setData(const QModelIndex & index, const QVariant & value, int role =
            Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;
    cs8UserType* getAliasByName(const QString & name);
    QList<cs8UserType*> list() {
        return m_aliasList;
    }
    void clear();
    void setProjectVersion(uint version){m_projectVersion=version;}
    bool contains(const QString &alias_);

signals:
    void modified(bool);

protected slots:
    void slotModified();
};

#endif /* CS8LIBRARYALIASMODEL_H_ */
