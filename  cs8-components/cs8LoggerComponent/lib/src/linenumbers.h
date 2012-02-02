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
/********************************************************************************************************
 * Une partie de cette classe fait partie du projet Monkey Studio
 * de Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 ********************************************************************************************************/
#ifndef LINENUMBERS_H
#define LINENUMBERS_H
//
#include <QWidget>
//
class QTextEdit;
class TextEdit;
class Editor;
//
class LineNumbers : public QWidget
{
	Q_OBJECT
	Q_PROPERTY( int digitNumbers READ digitNumbers WRITE setDigitNumbers )
	Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
	Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
	//
public:
	LineNumbers( TextEdit*, Editor* );
	//
	void setDigitNumbers( int );
	int digitNumbers() const;
	//
	void setTextColor( const QColor& );
	const QColor& textColor() const;
	//
	void setBackgroundColor( const QColor& );
	const QColor& backgroundColor() const;
	//
	void setExecutedLine(int ligne);
protected:
	virtual void paintEvent( QPaintEvent* );
	virtual void mousePressEvent( QMouseEvent * event );
	//
private:
	TextEdit* m_textEdit;
	Editor * m_editor;
	int mDigitNumbers;
	QColor mTextColor;
	QColor mBackgroundColor;
	int m_executedLine;
	int m_currentLine;
	//
signals:	
	void digitNumbersChanged();
	void textColorChanged( const QColor& );
	void backgroundColorChanged( const QColor& );
	//
protected slots:
	void slotResetExecutedLine();
private slots:
	void slotToggleBookmark();
public slots:
	void setDefaultProperties();
};
//
#endif 

