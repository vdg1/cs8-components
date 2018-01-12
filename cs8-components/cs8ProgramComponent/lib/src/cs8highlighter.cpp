//
// C++ Implementation: cs8highlighter
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8highlighter.h"

cs8Highlighter::cs8Highlighter(QTextDocument *parent)
 : QSyntaxHighlighter(parent)
{
	HighlightingRule rule;
	
	singleLineCommentFormat.setForeground(Qt::lightGray);
	rule.pattern = QRegExp("//[^\n]*");
	rule.format = singleLineCommentFormat;
	highlightingRules.append(rule);
}


cs8Highlighter::~cs8Highlighter()
{
}


void cs8Highlighter::highlightBlock(const QString& text)
{
	foreach (HighlightingRule rule, highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = text.indexOf(expression);
		while (index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = text.indexOf(expression, index + length);
		}
	}
}

