//
// C++ Implementation: cs8fileenginehandler
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8fileenginehandler.h"
#include "cs8fileengine.h"

cs8FileEngineHandler::cs8FileEngineHandler()
 : QAbstractFileEngineHandler()
{
}


cs8FileEngineHandler::~cs8FileEngineHandler()
{
}


QAbstractFileEngine* cs8FileEngineHandler::create(const QString& fileName) const
{
	return fileName.toLower().startsWith("cs8:/") ? new cs8FileEngine(fileName) : 0;
}



