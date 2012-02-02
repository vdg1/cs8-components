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
#ifndef EDITOR_H
#define EDITOR_H

#include "textEdit.h"
#include "cs8loggerwidget.h"
#include "ui_findwidget.h"
#include <QTextDocument>
#include <QTextEdit>
#include <QPointer>
#include <QTextCursor>
#include <QScrollBar>
#include <QTimer>
#include <QTextBlockUserData>
#include <QDateTime>
//
//Q_DECLARE_METATYPE(Tag)
//
typedef struct
{
	QStringList textFind;
	QStringList textReplace;
	bool caseSensitive;
	bool backwards;
	bool wholeWords;
	bool prompt;
} ReplaceOptions;
//
typedef QMap<QString, QStringList> classesMethodsList; 
//
class QToolButton;
class QComboBox;
class QTimer;
class TextEditInterface;
//
class BlockUserData : public QTextBlockUserData
{
public:
	bool bookmark;
	QTextBlock block;
};
//
class Editor : public QWidget
{
Q_OBJECT
public:
	TextEdit* textEdit() { return m_textEdit; }
	void open(const QString & fileName);
	Editor(QWidget *parent, cs8LoggerWidget *mainimpl, QString name="");
	~Editor();
	bool open(bool silentMode);
	bool save();
	bool close();
	void setFilename(QString name) { m_filename = name; m_textEdit->document()->setModified(true);};
	QString filename() { return m_filename; };
	QString shortFilename();
	QString directory();
	static QString shortFilename(QString nomLong);
	static QString suffixe(QString filename);
	void findContinue();
	void findPrevious();
	QStringList classes();
	void toggleBookmark(int line);
	void clearAllBookmarks();
	void setBackgroundColor( QColor c ){ m_textEdit->setBackgroundColor(c); };
	void setCurrentLineColor( QColor c ){ m_textEdit->setCurrentLineColor(c); };
	void setMatchingColor( QColor c ){ m_textEdit->setMatchingColor(c); };
	int currentLineNumber(){ return m_textEdit->currentLineNumber(); };
	int currentLineNumber(QTextBlock block){ return m_textEdit->currentLineNumber(block); };
	void print(){ m_textEdit->print(); };
	void copy() { m_textEdit->copy(); };
	void cut() { m_textEdit->cut(); };
	void paste() { m_textEdit->paste(); };
	void undo() { m_textEdit->document()->undo(); };
	void redo() { m_textEdit->document()->redo(); };
	void selectAll() { m_textEdit->selectAll(); };
	void dialogGotoLine() { m_textEdit->dialogGotoLine(); };
	QString wordUnderCursor() { return m_textEdit->wordUnderCursor(); };
	void textPlugin(TextEditInterface *iTextEdit) { m_textEdit->textPlugin(iTextEdit); };
	int verticalScrollBar() { return m_textEdit->verticalScrollBar()->value(); };
	void setVerticalScrollBar(int s) { m_textEdit->verticalScrollBar()->setValue(s); };
	void setHighlightCurrentLine(bool b) { m_textEdit->setHighlightCurrentLine(b); };
	bool isModified() { return m_textEdit->document()->isModified(); };
	void setFocus();
	void replace();
	void setActiveEditor(bool b);
	void toggleBookmark() { toggleBookmark( m_textEdit->currentLineNumber() ); };
	QList<int> bookmarksList();
	QString toPlainText() { return m_textEdit->toPlainText(); };
	void insertText(QString text, int insertAfterLine) { m_textEdit->insertText(text, insertAfterLine); };
	//
public slots:
	void togglePlot(int line);
	void gotoLine( int line, bool moveTop);
	void slotModifiedEditor(bool modified);
	void setLineNumbers(bool activate) { m_textEdit->activateLineNumbers(activate); };
	void setSelectionBorder(bool activate) { m_textEdit->setSelectionBorder(activate); };
	void setFont(QFont fonte) { m_textEdit->setFont(fonte); };
	void setSyntaxHighlight(bool activate);
	void setTabStopWidth(int size) { m_textEdit->setTabStopWidth(size) ; };
	void setTabSpaces(bool t) { m_textEdit->setTabSpaces(t) ; };
	void setSyntaxColors(QTextCharFormat a, QTextCharFormat b, QTextCharFormat c, QTextCharFormat d, QTextCharFormat e, QTextCharFormat f, QTextCharFormat g);
	void find();
	QString selection();
private slots:	
	void slotFindWidget_textChanged(QString text="", bool fromButton=false);
	void slotFindPrevious();
	void slotFindNext();
	void slotTimerCheckIfModifiedOutside();
	void slotCursorPositionChanged();

private:
	QWidget *m_parent;
	QPointer<LineNumbers> m_lineNumbers;
	QTextCursor m_previousCursor;
	QString m_findExp;
	QTextDocument::FindFlags m_findOptions;
	QToolButton *m_refreshButton;
	cs8LoggerWidget *m_mainimpl;
	QWidget *m_findWidget;
	TextEdit *m_textEdit;
	QString m_filename;
	Ui::FindWidget uiFind;
	QTimer *autoHideTimer;
	bool m_backward;
	ReplaceOptions m_replaceOptions;
	bool m_activeEditor;
	QTimer m_timerCheckLastModified;
	quint16 m_checksum;
	QDateTime m_lastModified;
	void checkBookmarks();
protected:
	QDateTime extractTimeStamp(const QString & line);
signals:
	void currentTimeStampChanged(QDateTime timeStamp);
	void markTimeStamp(QDateTime timeStamp, bool set);
	void editorModified(Editor *, bool);
	void togglePlotMessage(QString s);
};

#endif
