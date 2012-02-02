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
#include "editor.h"
#include "cs8loggerwidget.h"
#include "lineedit.h"
#include "linenumbers.h"

#include <QComboBox>
#include <QTextCursor>
#include <QTextBlock>
#include <QDialog>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPair>
#include <QToolButton>
#include <QTimer>
#include <QProcess>
#include <QTextDocumentFragment>
#include <QFileInfo>
#include <QDebug>
#include <QWidget>


Editor::Editor ( QWidget * parent, cs8LoggerWidget *mainimpl, QString name )
//    : QWidget ( parent ), m_parent ( parent ), m_mainimpl ( cs8LoggerWidget )
{
  m_filename =  name;
  
  int vposLayout = 0;
  m_textEdit = new TextEdit ( this, mainimpl );
  m_backward = false;
  m_activeEditor = false;
  //
  QGridLayout *gridLayout = new QGridLayout ( this );
  gridLayout->setSpacing ( 0 );
  gridLayout->setMargin ( 0 );
  gridLayout->setObjectName ( QString::fromUtf8 ( "gridLayout" ) );
  //
  //
  //
  //
  connect ( m_textEdit, SIGNAL ( editorModified ( bool ) ), this, SLOT ( slotModifiedEditor ( bool ) ) );
  connect ( m_textEdit, SIGNAL ( cursorPositionChanged() ), this, SLOT ( slotCursorPositionChanged() ) );
  //
  m_findWidget = new QWidget;
  uiFind.setupUi ( m_findWidget );
  connect ( uiFind.toolClose, SIGNAL ( clicked() ), m_findWidget, SLOT ( hide() ) );
  connect ( uiFind.editFind, SIGNAL ( textChanged ( QString ) ), this, SLOT ( slotFindWidget_textChanged ( QString ) ) );
  connect ( uiFind.editFind, SIGNAL ( returnPressed() ), this, SLOT ( slotFindNext() ) );
  connect ( uiFind.toolPrevious, SIGNAL ( clicked() ), this, SLOT ( slotFindPrevious() ) );
  connect ( uiFind.toolNext, SIGNAL ( clicked() ), this, SLOT ( slotFindNext() ) );
  //
  autoHideTimer = new QTimer ( this );
  autoHideTimer->setInterval ( 5000 );
  autoHideTimer->setSingleShot ( true );
  connect ( autoHideTimer, SIGNAL ( timeout() ), m_findWidget, SLOT ( hide() ) );
  //
  gridLayout->addWidget ( m_textEdit, vposLayout++, 0, 1, 1 );
  gridLayout->addWidget ( m_findWidget, vposLayout++, 0, 1, 1 );
  uiFind.labelWrapped->setVisible ( false );
  m_findWidget->hide();
  //
  connect ( &m_timerCheckLastModified, SIGNAL ( timeout() ), this, SLOT ( slotTimerCheckIfModifiedOutside() ) );
  
  setLineNumbers ( true );
  setSyntaxHighlight ( true );
  setHighlightCurrentLine ( true );
}
//
void Editor::setActiveEditor ( bool b )
{
  m_activeEditor = b;
}
//
void Editor::slotTimerCheckIfModifiedOutside()
{
  /*
   QFile file ( m_filename );
   if ( m_lastModified != QFileInfo ( file ).lastModified() ) {
     m_timerCheckLastModified.stop();
     int rep = QMessageBox::question ( this, "cs8 Logger",
                                       tr ( "The file \"%1\"\nwas modified outside editor.\n\n" ).arg ( m_filename ) +
                                       tr ( "What do you want to do?" ),
                                       tr ( "Overwrite" ), tr ( "Reload File" ) );
     if ( rep == 0 ) // Overwrite
     {
       m_textEdit->document()->setModified ( true );
       save();
     } else if ( rep == 1 ) // Reload
     {
       open ( false );
     }
     m_timerCheckLastModified.start ( 5000 );
   }
   */
  m_textEdit->updateContents();
  checkBookmarks();
}
//
void Editor::checkBookmarks()
{
  /*
      QMenu *menu = m_mainimpl->bookmarksMenu();
      QList<QAction *> actions = menu->actions();
      foreach(QAction *action, actions)
      {
          Bookmark bookmark = action->data().value<Bookmark>();
          QTextBlock block = bookmark.second;
          bool found = false;
          for ( QTextBlock b = m_textEdit->document()->begin(); b.isValid(); b = b.next() )
          {
              BlockUserData *blockUserData = (BlockUserData*)block.userData();
              if ( block==b && blockUserData && blockUserData->bookmark )
              {
                  found = true;
              }
          }
          if ( !found  )
          {
              m_mainimpl->toggleBookmark(this, "", false, block);
          }
      }
      */
}
//
void Editor::clearAllBookmarks()
{
  int line = 1;
  for ( QTextBlock block = m_textEdit->document()->begin(); block.isValid(); block = block.next(), line++ ) {
    BlockUserData *blockUserData = ( BlockUserData* ) block.userData();
    if ( blockUserData && blockUserData->bookmark )
      toggleBookmark ( line );
  }
}
//
Editor::~Editor()
{}
//
//
bool Editor::open ( bool silentMode )
{
  bool ret = m_textEdit->open ( silentMode, m_filename, m_lastModified );
  QByteArray array ( m_textEdit->toPlainText().toLocal8Bit() );
  if ( array.count() ) {
    char *ptr = array.data();
    quint16 check = qChecksum ( ptr, array.length() );
    m_checksum = check;
  }
  m_timerCheckLastModified.start ( 5000 );
  return ret;
}
//
bool Editor::close()
{
  bool ret = m_textEdit->close ( m_filename );
  if ( ret )
    clearAllBookmarks();
  return ret;
}
//
void Editor::setSyntaxHighlight ( bool activate )
{
  if ( activate && !QString ( ":c:cpp:cc:h:" ).contains ( ":"+m_filename.section ( ".", -1, -1 ).toLower() +":" ) )
    return;
  m_textEdit->setSyntaxHighlight ( activate );
}
//
bool Editor::save()
{
  return m_textEdit->save ( m_filename, m_lastModified );
}
//
void Editor::gotoLine ( int line, bool moveTop )
{
  m_textEdit->gotoLine ( line, moveTop );
}
//
void Editor::slotFindPrevious()
{
  m_backward = true;
  slotFindWidget_textChanged ( uiFind.editFind->text(), true );
}
//
void Editor::slotFindNext()
{
  m_backward = false;
  slotFindWidget_textChanged ( uiFind.editFind->text(), true );
}
//
void Editor::find()
{
  autoHideTimer->stop();
  m_findWidget->show();
  uiFind.editFind->setFocus ( Qt::ShortcutFocusReason );
  if ( m_textEdit->textCursor().selectedText().length() )
    uiFind.editFind->setText ( m_textEdit->textCursor().selectedText() );
  else
    uiFind.editFind->setText ( m_textEdit->wordUnderCursor() );
  uiFind.editFind->selectAll();
  autoHideTimer->start();
}

//return current selected text or word under cursor
QString Editor::selection()
{
  if ( m_textEdit->textCursor().selectedText().length() )
    return ( m_textEdit->textCursor().selectedText() );
  else
    return ( m_textEdit->wordUnderCursor() );
}

//
void Editor::setFocus()
{
  m_findWidget->hide();
  m_textEdit->setFocus ( Qt::OtherFocusReason );
}
//
void Editor::findContinue()
{
  //slotFindWidget_textChanged(uiFind.editFind->text(), true);
  slotFindNext();
}
//
void Editor::findPrevious()
{
  slotFindPrevious();
}
//
void Editor::slotFindWidget_textChanged ( QString text, bool fromButton )
{
  int options = 0;
  if ( m_backward )
    options |= QTextDocument::FindBackward;
  if ( uiFind.checkWholeWords->isChecked() )
    options |= QTextDocument::FindWholeWords;
  if ( uiFind.checkCase->isChecked() )
    options |= QTextDocument::FindCaseSensitively;
  m_textEdit->slotFind ( uiFind, text, ( QTextDocument::FindFlags ) options,fromButton );
  autoHideTimer->start();
}
//
//
void Editor::toggleBookmark ( int line )
{
  QTextCursor save = m_textEdit->textCursor();
  int scroll = verticalScrollBar();
  gotoLine ( line, false );
  m_textEdit->textCursor().movePosition ( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
  m_textEdit->textCursor().movePosition ( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
  QString s = m_textEdit->textCursor().block().text().simplified();
  //
  QTextCursor cursor = m_textEdit->textCursor();
  bool activate;
  BlockUserData *blockUserData = ( BlockUserData* ) cursor.block().userData();
  if ( !blockUserData ) {
    blockUserData = new BlockUserData();
    blockUserData->bookmark = false;
    blockUserData->block = cursor.block();
    
    
  }
  blockUserData->bookmark = !blockUserData->bookmark;
  cursor.block().setUserData ( blockUserData );
  activate = blockUserData->bookmark;
  //
  QDateTime date=extractTimeStamp(s);
  if ( date.isValid() )
    emit markTimeStamp ( date, activate );
  //
  m_textEdit->setTextCursor ( cursor );
  //
  //emit bookmark(this, s, QPair<bool,QTextBlock>(activate, cursor.block()));
//    m_mainimpl->toggleBookmark(this, s, activate, cursor.block());
  m_textEdit->setTextCursor ( save );
  setVerticalScrollBar ( scroll );
  m_textEdit->lineNumbers()->update();
}
//
QList<int> Editor::bookmarksList()
{
  QList<int> list;
  int line = 1;
  for ( QTextBlock block = m_textEdit->document()->begin(); block.isValid(); block = block.next(), line++ ) {
    BlockUserData *blockUserData = ( BlockUserData* ) block.userData();
    if ( blockUserData && blockUserData->bookmark )
      list << line;
  }
  return list;
}
//
QString Editor::shortFilename()
{
  QString name = m_filename;
  int pos = m_filename.lastIndexOf ( "/" );
  if ( pos != -1 )
    name = name.mid ( pos+1 );
  return name;
}
//
QString Editor::directory()
{
  return m_filename.left ( m_filename.length()-shortFilename().length()-1 );
}
QString Editor::shortFilename ( QString nomLong )
{
  QString name = nomLong;
  int pos = nomLong.lastIndexOf ( "/" );
  if ( pos != -1 )
    name = name.mid ( pos+1 );
  return name;
}
//
QString Editor::suffixe ( QString filename )
{
  return filename.section ( ".", -1, -1 );
}
//
void Editor::slotModifiedEditor ( bool modified )
{
  emit editorModified ( this, modified );
}
//
//
//
void Editor::setSyntaxColors ( QTextCharFormat a, QTextCharFormat b, QTextCharFormat c, QTextCharFormat d, QTextCharFormat e, QTextCharFormat f, QTextCharFormat g )
{

  m_textEdit->setSyntaxColors ( a, b, c, d, e, f, g );
}
//
//

void Editor::open ( const QString & fileName )
{
  m_filename=fileName;
  open ( true );
}



QDateTime Editor::extractTimeStamp(const QString & line)
{
  QString dateStr=line.left ( line.indexOf ( ']' ) ).remove ( 0,1 );
  QDateTime date;
  if ( dateStr.contains ( "/" ) ) {
    date=QDateTime::fromString ( dateStr,QString ( "MM/dd/yy HH:mm:ss" ) );
    // Y2K problem
    date=date.addYears ( 100 );
  } else {
    date=QDateTime::fromString ( dateStr,QString ( "MMM dd yyyy HH:mm:ss" ) );
  }
  //qDebug() << "extracted timestamp " << date;
  return date;
}

void Editor::slotCursorPositionChanged()
{
  QString text=m_textEdit->textCursor().block().text();
  QDateTime date=extractTimeStamp(text);
  if (date.isValid())
  emit currentTimeStampChanged(date);
}

void Editor::togglePlot(int line)
{
  QTextCursor save = m_textEdit->textCursor();
  int scroll = verticalScrollBar();
  gotoLine ( line, false );
  m_textEdit->textCursor().movePosition ( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
  m_textEdit->textCursor().movePosition ( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
  QString s = m_textEdit->textCursor().block().text().simplified();
  //
  emit togglePlotMessage(s);
  qDebug() << "plot message : " << s;
  QTextCursor cursor = m_textEdit->textCursor();
}

