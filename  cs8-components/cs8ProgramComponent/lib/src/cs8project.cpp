/*
 * cs8project.cpp
 *
 *  Created on: 2008-nov-02
 *      Author: Volker.Drewer-Gutlan
 */

#include "cs8project.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QRegExp>

cs8Project::cs8Project(QObject * parent) :
    QObject(parent) {
    // TODO Auto-generated constructor stub

}

cs8Project::~cs8Project() {
    // TODO Auto-generated destructor stub
}

bool cs8Project::openDoxyFile(const QString & fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);
    QStringList applicationList;
    bool inputTag = false;
    m_applications.clear();
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("INPUT ")) {
            inputTag = true;
            line.remove(QRegExp(".*=\\b*"));
            qDebug() << line;
        }
        if (inputTag) {
            line.chop(1);
            applicationList << line.simplified();
            while (!in.atEnd() && inputTag) {
                line = in.readLine();
                if (line.indexOf(QRegExp("^\\w")) == 0)
                    inputTag = false;
                else {
                    line.remove(QRegExp("\\\\$"));
                    line = line.simplified();
                    applicationList << line;
                }
            }
            qDebug() << applicationList;
        }
    }
    foreach(QString appName,applicationList) {
        m_applications.append(new cs8Application(this));
    }
    file.close();
    return true;
}

/*

QModelIndex cs8Project::index(int r, int c, const QModelIndex& parent) const {
 if (!hasIndex(r, c))
  return QModelIndex();
 return createIndex(r, c);
}

QVariant cs8Project::data(const QModelIndex& ind, int role) const {
 if (ind.isValid())
  return QVariant();
 if (ind.parent().isValid() == false) {
  if (role!=Qt::DisplayRole)
   return QVariant();

  return m_applications.at(ind.row())->name();
 } else
  return m_applications.at(ind.row())->programModel()->data(ind, role);
}

QModelIndex cs8Project::parent(const QModelIndex& index) const {
 if (!index.isValid()) {
  return QModelIndex();
 }
 else if (!index.parent().isValid()){
  return QModelIndex();
 }
 return m_applications.at(index.row())->programModel()->parent(index);
}

int cs8Project::rowCount(const QModelIndex& ind) const {
 if (ind.parent().isValid() == false) {

 } else {
  return m_applications.at(ind.row())->programModel()->rowCount(ind);
 }
}

int cs8Project::columnCount(const QModelIndex& ind) const {
 if (ind.parent().isValid() == false) {

 } else
  return m_applications.at(ind.row())->programModel()->columnCount(ind);
}
*/
