//
// C++ Interface: cs8fileengineiterator
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8FILEENGINEITERATOR_H
#define CS8FILEENGINEITERATOR_H

#include "cs8fileengine.h"

#include <QAbstractFileEngineIterator>
#include <QUrl>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8FileEngineIterator : public QAbstractFileEngineIterator
{
	public:
		cs8FileEngineIterator ( QDir::Filters filters, const QStringList & nameFilters, const QUrl & url );

		~cs8FileEngineIterator();

		virtual bool hasNext() const;
		virtual QFileInfo currentFileInfo() const;
		virtual QString currentFileName() const;
		virtual QString currentFilePath() const;
		virtual QString next();
		virtual QVariant entryInfo ( EntryInfoType type ) const;

	protected:
		QStringList entries;
		cs8FileEngine engine;
		int index;
};

#endif
