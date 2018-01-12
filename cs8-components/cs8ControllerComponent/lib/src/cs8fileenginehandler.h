//
// C++ Interface: cs8fileenginehandler
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8FILEENGINEHANDLER_H
#define CS8FILEENGINEHANDLER_H

#include "cs8fileengine.h"
#include <QAbstractFileEngineHandler>


/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8FileEngineHandler : public QAbstractFileEngineHandler
{
public:
    cs8FileEngineHandler();

    ~cs8FileEngineHandler();

		virtual QAbstractFileEngine* create(const QString& fileName) const;
    

};

#endif
