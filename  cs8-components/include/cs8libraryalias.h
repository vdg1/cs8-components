/*
 * cs8libraryalias.h
 *
 *  Created on: 2009-jan-22
 *      Author: Volker.Drewer-Gutlan
 */

#ifndef CS8LIBRARYALIAS_H_
#define CS8LIBRARYALIAS_H_

#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QObject>

class cs8LibraryAlias : public QObject
{
    Q_OBJECT

public:
    cs8LibraryAlias(const QString & name, const QString & path, bool autoLoad=true);
    virtual ~cs8LibraryAlias();
    QString name() const
    {
        return m_name;
    }

    void setName(const QString &name);

    QString path() const
    {
        return m_path;
    }

    void setPath(const QString & path);

    bool autoLoad() const
    {
        return m_autoLoad;
    }

    void setAutoLoad(bool m_autoLoad);

    QString documentation() const;
    QString definition() const;
    void setDocumentation(const QString doc);

private:
    QString m_name;
    QString m_path;
    QString m_documentation;
    bool m_autoLoad;

signals:
    void modified();
};

#endif /* CS8LIBRARYALIAS_H_ */
