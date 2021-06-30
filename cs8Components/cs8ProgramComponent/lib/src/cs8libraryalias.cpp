/*
 * cs8libraryalias.cpp
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8libraryalias.h"
#include "cs8application.h"
#include <QStringList>

cs8LibraryAlias::cs8LibraryAlias(QObject *parent,
                                 cs8Application *parentApplication,
                                 const QString &name, const QString &localPath,
                                 bool autoLoad)
    : m_application(0) {
  m_name = name;
  m_path = localPath;
  m_autoLoad = autoLoad;
  m_parentApplication = parentApplication;
}

cs8LibraryAlias::~cs8LibraryAlias() {
  // TODO Auto-generated destructor stub
}

QString cs8LibraryAlias::name() const { return m_name; }

void cs8LibraryAlias::setName(const QString &name) {
  this->m_name = name;
  emit modified();
}

QString cs8LibraryAlias::path(bool val3S6Format) const {
  if (val3S6Format) {
    QStringList l = m_path.split("/");
    if (l.last().endsWith(".pjx"))
      l.removeLast();
    QString s = l.join("/");
    return s;
  } else {
    return m_path;
  }
}
void cs8LibraryAlias::setPath(const QString &path) {
  this->m_path = path;
  emit modified();
}

QString cs8LibraryAlias::autoLoadString() const {
  return m_autoLoad ? "true" : "false";
}

void cs8LibraryAlias::setAutoLoad(bool m_autoLoad) {
  this->m_autoLoad = m_autoLoad;
  emit modified();
}

bool cs8LibraryAlias::autoLoad() const { return m_autoLoad; }

QString cs8LibraryAlias::documentation() const { return m_documentation; }

QString cs8LibraryAlias::definition() const {
  // qDebug() << "alias: " << m_name << ":" << m_path.split("/").last();
  QString pth = m_path.split("/").last();
  pth.chop(4);
  return QString("%1 %2").arg(pth, m_name);
}

void cs8LibraryAlias::setDocumentation(const QString doc) {
  m_documentation = doc;
}

cs8Application *cs8LibraryAlias::application() {
  if (!m_parentApplication) {
    return nullptr;
  } else if (!m_application) {
    m_application = new cs8Application(this);
    m_application->setCellPath(m_parentApplication->cellPath() + "usr/usrapp");
    m_application->openFromPathName(m_path);
  }
  return m_application;
}
