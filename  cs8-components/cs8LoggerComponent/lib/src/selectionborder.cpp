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
#include "selectionborder.h"
#include "textEdit.h"
//
#include <QTextEdit>
#include <QGridLayout>
#include <QScrollBar>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QDebug>
#include <QIcon>
//
SelectionBorder::SelectionBorder( TextEdit* edit )
	: QWidget( (QWidget *)edit ), m_textEdit( edit )
{
	setObjectName( "editorZone" );
	m_begin = m_end = 0;
	setBackgroundColor( QColor( "#ffffd2" ) );
	setFixedWidth( 10 );
	setCursor( QCursor(QPixmap(":/divers/images/curseurDroit.png"), 12, 0));
}
//
void SelectionBorder::paintEvent( QPaintEvent* )
{
	QPainter p( this );
	p.setBrush( backgroundColor() );
	p.setPen( backgroundColor() );
	p.drawRect( rect() );
}
//
void SelectionBorder::mousePressEvent ( QMouseEvent * event )
{
	if( event->modifiers() == Qt::NoModifier )
		m_begin = m_end = 0;
	mouseMoveEvent( event );
}
//
void SelectionBorder::mouseMoveEvent ( QMouseEvent * event )
{
	QTextCursor cursor = m_textEdit->cursorForPosition( event->pos() );
	int num = 1;
	for ( QTextBlock block = m_textEdit->document()->begin(); block.isValid() && block != cursor.block(); block = block.next(), num++ )
		;
	if( cursor.isNull() )
		return;
	if( m_begin == 0 )
		m_begin = num;
	m_end = num;
	//if( m_begin > m_end )
		//qSwap( m_begin, m_end);
	m_textEdit->selectLines(m_begin, m_end);	
}
//
void SelectionBorder::mouseReleaseEvent ( QMouseEvent * event )
{
	// TODO remove gcc warnings
	event = NULL;
}
//
//
void SelectionBorder::setBackgroundColor( const QColor& c )
{
	if ( c == mBackgroundColor )
		return;
	mBackgroundColor = c;
	QPalette p( palette() );
	p.setColor( backgroundRole(), mBackgroundColor );
	setPalette( p );
}
//
const QColor& SelectionBorder::backgroundColor() const
{
	return mBackgroundColor;
}
