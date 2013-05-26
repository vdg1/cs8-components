/*
 * cs8libraryalias.cpp
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8usertype.h"
#include <QStringList>

cs8UserType::cs8UserType(const QString & name, const QString & path,
                                 bool autoLoad) {
    m_name = name;
    m_path = path;
    m_autoLoad = autoLoad;
}

cs8UserType::~cs8UserType() {
    // TODO Auto-generated destructor stub
}

void cs8UserType::setName(const QString &name)
{
    this->m_name = name;
    emit modified();
}
void cs8UserType::setPath(const QString &path)
{
    this->m_path = path;
    emit modified();
}

void cs8UserType::setAutoLoad(bool m_autoLoad)
{
    this->m_autoLoad = m_autoLoad;
    emit modified();
}

QString cs8UserType::documentation() const {

    return m_documentation;
}

QString cs8UserType::definition() const {
    qDebug() << "alias: " << m_name << ":" << m_path.split("/").last();
    return QString("%1 %2").arg(m_path.split("/").last()).arg(m_name);
}

void cs8UserType::setDocumentation(const QString doc)
{
    m_documentation=doc;
}
