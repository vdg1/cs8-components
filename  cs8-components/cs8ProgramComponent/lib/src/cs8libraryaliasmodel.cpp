/*
 * cs8libraryaliasmodel.cpp
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8libraryaliasmodel.h"
#include "cs8libraryalias.h"
#include <QDebug>
#include <QStringList>
//
cs8LibraryAliasModel::cs8LibraryAliasModel(QObject * parent) :
	QAbstractTableModel(parent) {
	//m_variableList = new QList<cs8Variable*>;
}
//

bool cs8LibraryAliasModel::addAlias(const QDomElement & element,
		const QString & description) {
        QString path = element.attribute("path");
        QString name = element.attribute("alias");
	bool autoLoad = element.attribute("autoload")=="true"?true:false;
	cs8LibraryAlias * alias = new cs8LibraryAlias(name, path, autoLoad);
	m_aliasList.append(alias);
	reset();
	return true;
}
int cs8LibraryAliasModel::rowCount(const QModelIndex& index) const {
	return m_aliasList.count();
}

int cs8LibraryAliasModel::columnCount(const QModelIndex & index) const {
	return 3;
}

void cs8LibraryAliasModel::clear() {
	m_aliasList.clear();
}

QVariant cs8LibraryAliasModel::data(const QModelIndex & index, int role) const {
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		int row = index.row();
		int column = index.column();

		cs8LibraryAlias* variable = m_aliasList.at(row);
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

QVariant cs8LibraryAliasModel::headerData(int section,
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
 \fn cs8LibraryAliasModel::flags ( const QModelIndex & index ) const
 */
Qt::ItemFlags cs8LibraryAliasModel::flags(const QModelIndex & index) const {
	if (!index.isValid())
		return 0;

	if (index.column() == 3)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

}

/*!
 \fn cs8LibraryAliasModel::setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
 */
bool cs8LibraryAliasModel::setData(const QModelIndex & index,
		const QVariant & value, int role) {
	if (role == Qt::EditRole) {
		if (index.column() == 3) {
			cs8LibraryAlias* variable = m_aliasList.at(index.row());
			//variable->setDocumentation(value.toString());
			emit (dataChanged(index, index));
		}
	}
}

/*!
 \fn cs8LibraryAliasModel::getVarByName(const QString & name)
 */
cs8LibraryAlias* cs8LibraryAliasModel::getAliasByName(const QString & name) {
	for (int i = 0; i < m_aliasList.count(); i++) {
		if (m_aliasList.at(i)->name().compare(name) == 0)
			return m_aliasList.at(i);
	}
	return 0;
}

QString cs8LibraryAliasModel::toDocumentedCode() {
	QString header;
	foreach ( cs8LibraryAlias* var, m_aliasList ) {
			QString descr = var->documentation();
			descr.replace("\n", "\n  //");
			header
					+= QString("\n  // %3 : %2") .arg(descr) .arg(
							var->name());
		}
	return header;
}
