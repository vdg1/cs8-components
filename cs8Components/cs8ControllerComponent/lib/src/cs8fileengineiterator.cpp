//
// C++ Implementation: cs8fileengineiterator
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8fileengineiterator.h"

#include <QVariant>
#include <QDebug>

cs8FileEngineIterator::cs8FileEngineIterator ( QDir::Filters filters, const QStringList & nameFilters, const QUrl & url )
    : QAbstractFileEngineIterator ( filters,nameFilters ),index(0)
{
    index=-1;
    engine.setFileName(url.toString());
    QString _path=path();
    entries=engine.entryList(filters,nameFilters);
    qDebug() << "cs8FileEngineIterator: " << entries;
}


cs8FileEngineIterator::~cs8FileEngineIterator()
{
}


bool cs8FileEngineIterator::hasNext() const
{
    return index < entries.size() - 1;
}

QFileInfo cs8FileEngineIterator::currentFileInfo() const
{
    return QFileInfo (engine.url().toString()+"/"+entries.at(index));
}

QString cs8FileEngineIterator::currentFileName() const
{
    if (index<0)
        return QString();
    //qDebug() << "cs8FileEngineIterator::currentFileName(): " << index << entries.at(index);
    if (!entries.isEmpty())
        return entries.at(index);
    else
        return QString();
}

QString cs8FileEngineIterator::next()
{
    if (!hasNext())
        return QString();
    ++index;
    return currentFilePath();
}

QVariant cs8FileEngineIterator::entryInfo ( EntryInfoType type ) const
{
    return QAbstractFileEngineIterator::entryInfo ( type );
}

QString cs8FileEngineIterator::currentFilePath() const
{
    return engine.url().path();
}
