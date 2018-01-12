//
// C++ Implementation: cs8fileinfo
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8fileinfo.h"


#include <QDir>


cs8FileInfo::cs8FileInfo(const QUrlInfo & info)
	: QFileInfo()
{
	m_info=info;
}

cs8FileInfo::cs8FileInfo(cs8FileInfo* info)
	: QFileInfo()
{
	m_info=info->info();
}

cs8FileInfo::~cs8FileInfo()
{
}




/*!
    \fn cs8FileInfo::isDir() const
 */
bool cs8FileInfo::isDir() const
{
    return m_info.isDir();
}


/*!
    \fn cs8FileInfo::fileName() const
 */
QString cs8FileInfo::fileName() const
{
    return m_info.name();
}


/*!
    \fn cs8FileInfo::filePath() const
 */
QString cs8FileInfo::filePath() const
{
	return m_info.name();
}


/*!
    \fn cs8FileInfo::dir() const
 */
QDir cs8FileInfo::dir() const
{
	return QDir(m_info.name());
}


QUrlInfo cs8FileInfo::info() const
{
	return m_info;
}
