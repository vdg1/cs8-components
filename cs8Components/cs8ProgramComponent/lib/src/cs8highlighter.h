//
// C++ Interface: cs8highlighter
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8HIGHLIGHTER_H
#define CS8HIGHLIGHTER_H

#include <QSyntaxHighlighter>



/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8Highlighter : public QSyntaxHighlighter
{
		Q_OBJECT
	protected:
		struct HighlightingRule
		{
			QRegExp pattern;
			QTextCharFormat format;
		};
		QVector<HighlightingRule> highlightingRules;
		void highlightBlock ( const QString& text );
		QTextCharFormat singleLineCommentFormat;
		
	public:
		cs8Highlighter ( QTextDocument *parent = 0 );
		~cs8Highlighter();



};

#endif
