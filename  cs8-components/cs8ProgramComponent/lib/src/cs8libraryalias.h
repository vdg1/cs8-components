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

class cs8LibraryAlias {
public:
	cs8LibraryAlias(const QString & name, const QString & path, bool autoLoad=true);
	virtual ~cs8LibraryAlias();
    QString name() const
    {
        return m_name;
    }

    void setName(QString m_name)
    {
        this->m_name = m_name;
    }

    QString path() const
    {
        return m_path;
    }

    void setPath(QString m_path)
    {
        this->m_path = m_path;
    }

    bool autoLoad() const
    {
        return m_autoLoad;
    }

    void setAutoLoad(bool m_autoLoad)
    {
        this->m_autoLoad = m_autoLoad;
    }

    QString documentation();
    QString definition();

private:
	QString m_name;
	QString m_path;
	bool m_autoLoad;
};

#endif /* CS8LIBRARYALIAS_H_ */
