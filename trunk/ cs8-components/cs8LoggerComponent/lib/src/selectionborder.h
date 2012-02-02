/*
* This file is part of QDevelop, an open-source cross-platform IDE
* Copyright (C) 2006  Jean-Luc Biord
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Jean-Luc Biord <jl.biord@free.fr>
* Program URL   : http://qdevelop.org
*
*/
#ifndef SELECTIONBORDER_H
#define SELECTIONBORDER_H
//
#include <QWidget>
//
class QTextEdit;
class TextEdit;
//
class SelectionBorder : public QWidget
{
public:
	SelectionBorder( TextEdit* );
	void setBackgroundColor( const QColor& );
	const QColor& backgroundColor() const;
	//
protected:
	virtual void paintEvent( QPaintEvent* );	
	virtual void mousePressEvent( QMouseEvent * event );
	virtual void mouseMoveEvent( QMouseEvent * event );
	virtual void mouseReleaseEvent( QMouseEvent * event );
	//
private:
	TextEdit* m_textEdit;
	QColor mBackgroundColor;
	int m_begin;
	int m_end;
signals:	
protected slots:
public slots:
};
//
#endif 

