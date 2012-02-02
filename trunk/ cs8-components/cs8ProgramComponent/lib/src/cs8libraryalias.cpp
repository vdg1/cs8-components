/*
 * cs8libraryalias.cpp
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8libraryalias.h"
#include <QStringList>

cs8LibraryAlias::cs8LibraryAlias(const QString & name, const QString & path,
		bool autoLoad) {
	m_name = name;
	m_path = path;
	m_autoLoad = autoLoad;
}

cs8LibraryAlias::~cs8LibraryAlias() {
	// TODO Auto-generated destructor stub
}

QString cs8LibraryAlias::documentation() {

	return QString();
}

QString cs8LibraryAlias::definition() {
	qDebug() << "alias: " << m_name << ":" << m_path.split("/").last();
	return QString("%1 %2").arg(m_path.split("/").last()).arg(m_name);
}
