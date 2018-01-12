/*
 * cs8libraryaliasmodel.cpp
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8usertypemodel.h"
#include "cs8usertype.h"
#include <QDebug>
#include <QStringList>
//
cs8UserTypeModel::cs8UserTypeModel(QObject * parent) :
    QAbstractTableModel(parent) {
    //m_variableList = new QList<cs8Variable*>;
}
//

bool cs8UserTypeModel::add(const QDomElement & element, const QString & description)
{
    QString path = element.attribute("path");
    QString name = element.attribute("alias");
    bool autoLoad = element.attribute("autoload")=="true"?true:false;

    add (name,path,autoLoad);

    //cs8UserType * alias = new cs8UserType(name, path, autoLoad);
    //m_aliasList.append(alias);
    //reset();

    return true;
}

bool cs8UserTypeModel::add(const QString &aliasName, const QString &path, const bool autoLoad)
{
    cs8UserType * alias = new cs8UserType(aliasName, path, autoLoad);
    connect (alias,SIGNAL(modified()),this,SLOT(slotModified()));
    m_aliasList.append(alias);
    reset();
    return true;
}

int cs8UserTypeModel::rowCount(const QModelIndex& index) const {
    Q_UNUSED(index);
    return m_aliasList.count();
}

int cs8UserTypeModel::columnCount(const QModelIndex & index) const {
    Q_UNUSED(index);
    return 3;
}

void cs8UserTypeModel::clear() {
    m_aliasList.clear();
}

bool cs8UserTypeModel::contains(const QString &alias_)
{
    foreach(cs8UserType *alias,m_aliasList)
    {
        if (alias->name ()==alias_)
            return true;
    }
    return false;
}

QVariant cs8UserTypeModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int row = index.row();
        int column = index.column();

        cs8UserType* variable = m_aliasList.at(row);
        switch (column) {
        case 0:
            return variable->name();
            break;

        case 1:
            return variable->path();
            break;

        case 2:
            return variable->autoLoad();
            break;

        }
    }

    return QVariant();
}

QVariant cs8UserTypeModel::headerData(int section,
                                          Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section) {
        case 0:
            return tr("Name");
            break;

        case 1:
            return tr("Interface");
            break;

        case 2:
            return tr("Autoload");
            break;

        }

    return QVariant();
}

/*!
 \fn cs8UserTypeModel::flags ( const QModelIndex & index ) const
 */
Qt::ItemFlags cs8UserTypeModel::flags(const QModelIndex & index) const {
    if (!index.isValid())
        return 0;

    if (index.column() == 3)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

}

/*!
 \fn cs8UserTypeModel::setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
 */
bool cs8UserTypeModel::setData(const QModelIndex & index,
                                   const QVariant & value, int role) {
    if (role == Qt::EditRole) {
        if (index.column() == 3) {
            cs8UserType* variable = m_aliasList.at(index.row());
            variable->setDocumentation(value.toString());
            emit (dataChanged(index, index));
        }
        emit modified (true);
    }
    return true;
}

/*!
 \fn cs8UserTypeModel::getVarByName(const QString & name)
 */
cs8UserType* cs8UserTypeModel::getAliasByName(const QString & name) {
    for (int i = 0; i < m_aliasList.count(); i++) {
        if (m_aliasList.at(i)->name().compare(name) == 0)
            return m_aliasList.at(i);
    }
    return 0;
}

QString cs8UserTypeModel::toDocumentedCode() {
    QString header;
    foreach ( cs8UserType* var, m_aliasList ) {
        QString descr = var->documentation();
        descr.replace("\n", "\n  //");
        header
                += QString("\n  // %3 : %2") .arg(descr) .arg(
                    var->name());
    }
    return header;
}

void cs8UserTypeModel::slotModified()
{
    emit modified (true);
}
