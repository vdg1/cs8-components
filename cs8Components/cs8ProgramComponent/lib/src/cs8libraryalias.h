/*
 * cs8libraryalias.h
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#ifndef CS8LIBRARYALIAS_H_
#define CS8LIBRARYALIAS_H_

#include <QDebug>
#include <QHash>
#include <QObject>
#include <QString>
#include <QVariant>

class cs8Application;
class cs8LibraryAlias : public QObject {
  Q_OBJECT

public:
  cs8LibraryAlias(QObject *parent, cs8Application *parentApplication,
                  const QString &name, const QString &localPath,
                  bool autoLoad = true);
  virtual ~cs8LibraryAlias();
  QString name() const;

  void setName(const QString &name);

  QString path(bool val3S6Format = false) const;

  void setPath(const QString &path);

  QString autoLoadString() const;

  void setAutoLoad(bool m_autoLoad);
  bool autoLoad() const;

  QString documentation() const;
  QString definition() const;
  void setDocumentation(const QString doc);

  cs8Application *application();

private:
  QString m_name;
  QString m_path;
  QString m_documentation;
  bool m_autoLoad;
  cs8Application *m_application;
  cs8Application *m_parentApplication;

signals:
  void modified();
};

#endif /* CS8LIBRARYALIAS_H_ */
