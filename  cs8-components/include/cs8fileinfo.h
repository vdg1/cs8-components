//
// C++ Interface: cs8fileinfo
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8FILEINFO_H
#define CS8FILEINFO_H

#include "qurlinfo.h"
#include <QFileInfo>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8FileInfo : public QFileInfo
{

public:
		cs8FileInfo(const QUrlInfo & info);
		cs8FileInfo(cs8FileInfo* info);

    ~cs8FileInfo();
    bool isDir() const;
    QString fileName() const;
    QString filePath() const;
    QDir dir() const;

	QUrlInfo info() const;
	

protected:
    QUrlInfo m_info;
};

#endif
