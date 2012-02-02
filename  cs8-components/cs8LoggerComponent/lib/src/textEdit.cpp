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
#include "textEdit.h"
#include "editor.h"
#include "linenumbers.h"
#include "selectionborder.h"
#include "ui_gotoline.h"
#include "pluginsinterfaces.h"
//
#include <QTextCursor>
#include <QDialog>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMenu>
#include <QTextDocumentFragment>
#include <QScrollBar>
#include <QPainter>
#include <QDebug>
#include <QListWidget>
#include <QProcess>
#include <QClipboard>
#include <QFileInfo>
#include <QPrintDialog>
#include <QTime>
#include <QPrinter>
#include <QTextLayout>
#include <QTextCodec>

static const char * tabPixmap_img[] =
  {
    /* width height ncolors cpp [x_hot y_hot] */
    "8 8 3 2 0 0",
    /* colors */
    "  s none       m none  c none",
    "O s iconColor1 m black c black",
    "X s iconColor2 m black c #D0D0D0",
    /* pixels */
    "  X     X       ",
    "    X     X     ",
    "      X     X   ",
    "        X     X ",
    "      X     X   ",
    "    X     X     ",
    "  X     X       ",
    "                ",
  };
  
static const char * spacePixmap_img[] =
  {
    /* width height ncolors cpp [x_hot y_hot] */
    "8 8 3 2 0 0",
    /* colors */
    "  s none       m none  c none",
    "O s iconColor1 m black c black",
    "X s iconColor2 m black c #D0D0D0",
    /* pixels */
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "      X         ",
    "      X X       ",
    "                ",
  };
  
TextEdit::TextEdit ( Editor * parent, cs8LoggerWidget *mainimpl )
    : QTextEdit ( parent ), m_editor ( parent ), m_mainImpl ( mainimpl ), m_mouseHidden ( false )
{
  setObjectName ( "editorZone" );
  m_lineNumbers = 0;
  m_selectionBorder = 0;
  highlighter = 0;
  setAcceptRichText ( false );
  setLineWrapMode ( QTextEdit::FixedPixelWidth );
  setLineWrapColumnOrWidth ( 65535 );
  m_findOptions = 0;
  m_findExp = "";
  m_findImpl = 0;
  m_highlightCurrentLine = true;
  m_tabPixmap		= QPixmap ( tabPixmap_img );
  m_spacePixmap		= QPixmap ( spacePixmap_img );
  m_showWhiteSpaces	= true;
  
  
  connect ( document(), SIGNAL ( modificationChanged ( bool ) ), this, SIGNAL ( editorModified ( bool ) ) );
  connect ( this, SIGNAL ( cursorPositionChanged() ), this, SLOT ( slotCursorPositionChanged() ) );
  connect ( document(), SIGNAL ( contentsChange ( int, int, int ) ), this, SLOT ( slotContentsChange ( int, int, int ) ) );
  //
  setBackgroundColor ( m_backgroundColor );
  setSyntaxHighlight ( true );
  setSelectionBorder ( true );
  setCurrentLineColor ( Qt::lightGray );
  setHighlightCurrentLine ( true );
}
//
void TextEdit::setBackgroundColor ( QColor c )
{
  if ( c == m_backgroundColor )
    return;
  m_backgroundColor = c;
  QPalette pal = palette();
  pal.setColor ( QPalette::Base, m_backgroundColor );
  setPalette ( pal );
  viewport()->update();
}
//
void TextEdit::slotContentsChange ( int position, int charsRemoved, int charsAdded )
{
  // TODO remove gcc warnings
  position = 0;
  charsRemoved = 0;
  charsAdded = 0;
}
//
void TextEdit::setCurrentLineColor ( QColor c )
{
  if ( c == m_currentLineColor )
    return;
  m_currentLineColor = c;
  viewport()->update();
}
//
void TextEdit::print()
{
  QPrinter printer ( QPrinter::HighResolution );
  printer.setFullPage ( true );
  QPrintDialog dlg ( &printer, this );
  if ( dlg.exec() == QDialog::Accepted ) {
    document()->print ( &printer );
  }
}

void TextEdit::printWhiteSpaces ( QPainter &p )
{
  const int contentsY = verticalScrollBar()->value();
  const qreal pageBottom = contentsY + viewport()->height();
  const QFontMetrics fm = QFontMetrics ( currentFont() );
  
  for ( QTextBlock block = document()->begin(); block.isValid(); block = block.next() ) {
    QTextLayout* layout = block.layout();
    const QRectF boundingRect = layout->boundingRect();
    QPointF position = layout->position();
    
    if ( position.y() +boundingRect.height() < contentsY )
      continue;
    if ( position.y() > pageBottom )
      break;
      
    const QString txt = block.text();
    const int len = txt.length();
    
    for ( int i=0; i<len; i++ ) {
      QPixmap *p1 = 0;
      
      if ( txt[i] == ' ' )
        p1 = &m_spacePixmap;
      else if ( txt[i] == '\t' )
        p1 = &m_tabPixmap;
      else
        continue;
        
      // pixmaps are of size 8x8 pixels
      QTextCursor cursor = textCursor();
      cursor.setPosition ( block.position() + i, QTextCursor::MoveAnchor );
      
      QRect r = cursorRect ( cursor );
      int x = r.x() + 4;
      int y = r.y() + fm.height() / 2 - 5;
      p.drawPixmap ( x, y, *p1 );
    }
  }
}

//
//
void TextEdit::setFocus ( Qt::FocusReason reason )
{
  QTextEdit::setFocus ( reason );
}
//
void TextEdit::mousePressEvent ( QMouseEvent * event )
{
  QTextEdit::mousePressEvent ( event );
}
//
TextEdit::~TextEdit()
{
  delete lineNumbers();
}
//
void TextEdit::setTabStopWidth ( int taille )
{
  bool m = document()->isModified();
  int posScrollbar = verticalScrollBar()->value();
  QTextEdit::setTabStopWidth ( fontMetrics().width ( " " ) * taille );
  setPlainText ( toPlainText() );
  verticalScrollBar()->setValue ( posScrollbar );
  document()->setModified ( m );
}
//
bool TextEdit::open ( bool silentMode, QString filename, QDateTime &lastModified, bool followContents )
{
  /*    if ( highlighter)
      {
          delete highlighter;
          highlighter = 0;
      }
  */
  m_fileName=filename;
  QFile file ( filename );
  if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) ) {
    if ( !silentMode )
      QMessageBox::critical ( 0, "QDevelop", tr ( "The file \"%1\" could not be loaded." ).arg ( filename ),tr ( "Cancel" ) );
    return false;
  }
  QByteArray data = file.readAll();
  QTextStream in ( &data );
  QApplication::setOverrideCursor ( Qt::WaitCursor );
  in.setAutoDetectUnicode ( false );
  QString decodedStr = in.readAll();
  setPlainText ( decodedStr );
  lastModified = QFileInfo ( file ).lastModified();
  file.close();
  m_bytesRead=in.pos();
  if ( m_lineNumbers )
    m_lineNumbers->setDigitNumbers ( QString::number ( linesCount() ).length() );
  QApplication::restoreOverrideCursor();
  return true;
}
//
void TextEdit::activateLineNumbers ( bool activate )
{
  if ( activate && m_lineNumbers==0 )
    setLineNumbers ( new LineNumbers ( this, m_editor ) );
  else if ( !activate && m_lineNumbers )
    setLineNumbers ( 0 );
}
//
void TextEdit::setSelectionBorder ( bool activate )
{
  if ( activate && m_selectionBorder==0 )
    setSelectionBorder ( new SelectionBorder ( this ) );
  else if ( !activate && m_selectionBorder )
    setSelectionBorder ( ( SelectionBorder* ) 0 );
}
//
//
void TextEdit::comment ( ActionComment action )
{
  // Trent's implementation
  QTextCursor cursor = textCursor();
  
  //when there is no selection startPos and endPos are equal to position()
  int startPos = cursor.selectionStart();
  int endPos = cursor.selectionEnd();
  QTextBlock startBlock = document()->findBlock ( startPos );
  QTextBlock endBlock = document()->findBlock ( endPos );
  
  //special case : the end of the selection is at the beginning of a line
  if ( startPos != endPos && cursor.atBlockStart() ) {
    endBlock = document()->findBlock ( endPos ).previous();
  }
  
  int firstLine = lineNumber ( startBlock );
  int lastLine = lineNumber ( endBlock );
  QTextBlock block = startBlock;
  cursor.beginEditBlock();
  cursor.setPosition ( startPos );
  while ( ! ( endBlock < block ) ) {
    QString text = block.text();
    if ( !text.isEmpty() ) {
      int i = 0;
      while ( i < text.length() && text.at ( i ).isSpace() )
        i++;
      if ( action == Comment ) {
        if ( text.mid ( i, 2 ) != "//" )
          text.insert ( i, "//" );
      } else if ( action == Uncomment ) {
        if ( text.mid ( i, 2 ) == "//" )
          text.remove ( i, 2 );
      } else if ( action == Toggle ) {
        if ( text.mid ( i, 2 ) == "//" )
          text.remove ( i, 2 );
        else
          text.insert ( i, "//" );
      }
      cursor.movePosition ( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
      cursor.movePosition ( QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
      cursor.insertText ( text );
    }
    cursor.movePosition ( QTextCursor::NextBlock );
    block = cursor.block();
  }
  cursor.endEditBlock();
  // Reselect blocks
  selectLines ( firstLine, lastLine );
}
//
//
void TextEdit::setSyntaxHighlight ( bool activate )
{
  if ( activate && highlighter==0 )
    highlighter = new Highlighter ( this->document() );
  else if ( !activate && highlighter ) {
    delete highlighter;
    highlighter = 0;
  }
}
//
void TextEdit::setSyntaxColors ( QTextCharFormat a, QTextCharFormat b, QTextCharFormat c, QTextCharFormat d, QTextCharFormat e, QTextCharFormat f, QTextCharFormat g )
{
  /*   if (!highlighter)
         return;
     highlighter->setPreprocessorFormat( a );
     highlighter->setClassFormat( b );
     highlighter->setSingleLineCommentFormat( c );
     highlighter->setMultiLineCommentFormat( d );
     highlighter->setQuotationFormat( e );
     highlighter->setFunctionFormat( f );
     highlighter->setKeywordFormat( g );
     */
  highlighter->setDocument ( document() );
  
}

bool TextEdit::close ( QString filename )
{
  if ( document()->isModified() ) {
    // Proposer sauvegarde
    int rep = QMessageBox::question ( this, "QDevelop",
                                      tr ( "Save \"%1\"" ).arg ( filename ), tr ( "Yes" ), tr ( "No" ), tr ( "Cancel" ), 0, 2 );
    if ( rep == 2 )
      return false;
    if ( rep == 0 ) {
      QDateTime info;
      return save ( filename, info );
    }
  }
  return true;
}
//
bool TextEdit::save ( QString filename, QDateTime &lastModified )
{
  if ( !document()->isModified() )
    return true;
  QFile file ( filename );
  if ( !file.open ( QIODevice::WriteOnly ) ) {
    QMessageBox::about ( 0, "QDevelop",tr ( "Unable to save %1" ).arg ( filename ) );
    return false;
  }
  QApplication::setOverrideCursor ( Qt::WaitCursor );
  QString s = toPlainText();
  QTextStream out ( &file );
  out << s;
  file.close();
  QFile last ( filename );
  lastModified = QFileInfo ( last ).lastModified();
  QApplication::restoreOverrideCursor();
  document()->setModified ( false );
  return true;
}
//
void TextEdit::setMouseHidden ( bool hidden )
{
  if ( hidden == m_mouseHidden )
    return;
  viewport()->setCursor ( hidden ? Qt::BlankCursor : Qt::IBeamCursor );
  setMouseTracking ( hidden );
  m_mouseHidden = hidden;
}
//
void TextEdit::resizeEvent ( QResizeEvent* e )
{
  QTextEdit::resizeEvent ( e );
  QRect margeNumerotationGeometry;
  if ( m_lineNumbers ) {
    margeNumerotationGeometry = QRect ( viewport()->geometry().topLeft(), QSize ( m_lineNumbers->width(), viewport()->height() ) );
    margeNumerotationGeometry.moveLeft ( margeNumerotationGeometry.left() -m_lineNumbers->width() );
    if ( m_selectionBorder )
      margeNumerotationGeometry.moveLeft ( margeNumerotationGeometry.left() -m_selectionBorder->width() );
    if ( m_lineNumbers->geometry() != margeNumerotationGeometry )
      m_lineNumbers->setGeometry ( margeNumerotationGeometry );
  } else {
    margeNumerotationGeometry.setTopRight ( viewport()->geometry().topLeft() );
  }
  
  QRect margeSelectionGeometry;
  if ( m_selectionBorder ) {
    margeSelectionGeometry = QRect (  margeNumerotationGeometry.topRight(), QSize ( m_selectionBorder->width(), viewport()->height() ) );
    if ( m_lineNumbers )
      margeSelectionGeometry.moveLeft ( m_lineNumbers->width() );
    else
      margeSelectionGeometry.moveLeft ( margeSelectionGeometry.left() -m_selectionBorder->width() );
      
    if ( m_selectionBorder->geometry() != margeSelectionGeometry )
      m_selectionBorder->setGeometry ( margeSelectionGeometry );
  }
}

QTextCursor TextEdit::getLineCursor ( int line ) const
{
  int count = 1;
  for ( QTextBlock b = document()->begin(); b.isValid(); b = b.next(), count++ ) {
    if ( count == line ) {
      return QTextCursor ( b );
      break;
    }
  }
  QTextCursor c = textCursor();
  c.movePosition ( QTextCursor::End );
  c.movePosition ( QTextCursor::StartOfLine );
  return c;
}
//
void TextEdit::dialogGotoLine()
{
  QDialog *dial = new QDialog;
  Ui::GotoLine ui;
  ui.setupUi ( dial );
  ui.horizontalSlider->setMaximum ( linesCount()-1 );
  ui.horizontalSlider->setPageStep ( ( linesCount()-1 ) /10 );
  ui.spinBox->setMaximum ( linesCount()-1 );
  ui.spinBox->setValue ( currentLineNumber() );
  ui.spinBox->selectAll();
  ui.spinBox->setFocus();
  if ( dial->exec() == QDialog::Accepted )
    gotoLine ( ui.spinBox->value(), true );
  delete dial;
}
//
void TextEdit::gotoLine ( int line, bool moveTop )
{
  if ( moveTop )
    setTextCursor ( getLineCursor ( linesCount() ) );
  setTextCursor ( getLineCursor ( line ) );
  setFocus ( Qt::OtherFocusReason );
  //
  ensureCursorVisible();
  if ( moveTop ) {
    QTextCursor cursor = textCursor();
    if ( cursor.isNull() )
      return;
    QTextCursor c = textCursor();
    bool mouvementReussi;
    do {
      mouvementReussi = c.movePosition ( QTextCursor::Down, QTextCursor::MoveAnchor, 1 );
      setTextCursor ( c );
      ensureCursorVisible();
    } while ( mouvementReussi && cursorRect ( cursor ).y() > 25 );
    cursor.movePosition ( QTextCursor::StartOfLine, QTextCursor::MoveAnchor );
    cursor.movePosition ( QTextCursor::StartOfLine, QTextCursor::KeepAnchor, 1 );
    setTextCursor ( cursor );
  }
}
//
void TextEdit::setLineNumbers ( LineNumbers* g )
{
  if ( g == m_lineNumbers )
    return;
  if ( m_lineNumbers )
    delete m_lineNumbers;
  m_lineNumbers = g;
  connect ( m_lineNumbers, SIGNAL ( digitNumbersChanged() ), this, SLOT ( slotAdjustSize() ) );
  int margeGaucheSelection = 0;
  if ( m_selectionBorder )
    margeGaucheSelection = m_selectionBorder->width();
  if ( m_lineNumbers ) {
    setViewportMargins ( m_lineNumbers->width() +margeGaucheSelection, 0, 0, 0 );
    if ( !m_lineNumbers->isVisible() )
      m_lineNumbers->show();
  } else
    setViewportMargins ( margeGaucheSelection, 0, 0, 0 );
}
//
void TextEdit::slotAdjustSize()
{
  int margeGaucheSelection = 0;
  int margeGaucheMargeNumerotation = 0;
  if ( m_selectionBorder )
    margeGaucheSelection = m_selectionBorder->width();
  if ( m_lineNumbers )
    margeGaucheMargeNumerotation = m_lineNumbers->width();
  setViewportMargins ( margeGaucheMargeNumerotation+margeGaucheSelection, 0, 0, 0 );
}
//
LineNumbers* TextEdit::lineNumbers()
{
  return m_lineNumbers;
}
//
void TextEdit::setSelectionBorder ( SelectionBorder* m )
{
  if ( m == m_selectionBorder )
    return;
  if ( m_selectionBorder )
    delete m_selectionBorder;
  m_selectionBorder = m;
  int margeGaucheMargeNumerotation = 0;
  if ( m_lineNumbers )
    margeGaucheMargeNumerotation = m_lineNumbers->width();
  if ( m_selectionBorder ) {
    setViewportMargins ( m_selectionBorder->width() +margeGaucheMargeNumerotation, 0, 0, 0 );
    if ( !m_selectionBorder->isVisible() )
      m_selectionBorder->show();
  } else {
    setViewportMargins ( margeGaucheMargeNumerotation, 0, 0, 0 );
  }
}
//
SelectionBorder* TextEdit::selectionBorder()
{
  return m_selectionBorder;
}
//
void TextEdit::findText()
{
  m_editor->find();
}
//
void TextEdit::slotFind ( Ui::FindWidget ui, QString ttf,QTextDocument::FindFlags options, bool fromButton )
{
  qDebug() << "slotFind";
  QTextDocument *doc = document();
  QString oldText = ui.editFind->text();
  QTextCursor c = textCursor();
  QPalette p = ui.editFind->palette();
  p.setColor ( QPalette::Active, QPalette::Base, Qt::white );
  
  if ( c.hasSelection() ) {
    if ( options & QTextDocument::FindBackward || fromButton )
      c.setPosition ( c.position(), QTextCursor::MoveAnchor );
    else
      c.setPosition ( c.anchor(), QTextCursor::MoveAnchor );
  }
  
  
  QTextCursor newCursor = c;
  
  if ( !ttf.isEmpty() ) {
    newCursor = doc->find ( ttf, c, options );
    ui.labelWrapped->hide();
    
    if ( newCursor.isNull() ) {
      QTextCursor ac ( doc );
      ac.movePosition ( options & QTextDocument::FindBackward
                        ? QTextCursor::End : QTextCursor::Start );
      newCursor = doc->find ( ttf, ac, options );
      if ( newCursor.isNull() ) {
        p.setColor ( QPalette::Active, QPalette::Base, QColor ( 255, 102, 102 ) );
        newCursor = c;
      } else
        ui.labelWrapped->show();
    }
  }
  
  setTextCursor ( newCursor );
  ui.editFind->setPalette ( p );
}
//
void TextEdit::paintEvent ( QPaintEvent * event )
{
  QPainter painter ( viewport() );
  if ( m_highlightCurrentLine && m_currentLineColor.isValid() ) {
    QRect r = cursorRect();
    r.setX ( 0 );
    r.setWidth ( viewport()->width() );
    painter.fillRect ( r, QBrush ( m_currentLineColor ) );
  }
  
  if ( m_showWhiteSpaces )
    printWhiteSpaces ( painter );
    
  QTextEdit::paintEvent ( event );
  
}
//
void TextEdit::mouseMoveEvent ( QMouseEvent * event )
{
  setMouseHidden ( false );
  event->setAccepted ( false );
  QTextEdit::mouseMoveEvent ( event );
}
//
void TextEdit::slotCursorPositionChanged()
{
  if ( m_currentLineColor.isValid() )
    viewport()->update();
  m_plainText = toPlainText();
}
//
void TextEdit::keyPressEvent ( QKeyEvent * event )
{
  /*
      QTextCursor cursor = textCursor();
      clearMatch();
      setMouseHidden( true );
      if ( event->key() == Qt::Key_Tab )
      {
          slotIndent( !(event->modifiers() == Qt::ControlModifier) );
      }
      else if ( event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter )
      {
          if ( m_autoindent )
          {
              QTextEdit::keyPressEvent ( event );
              autoIndent();
          }
          else
          {
              QTextEdit::keyPressEvent ( event );
          }
      }
      else if ( event->key() == Qt::Key_Home && !event->modifiers() )
      {
          key_home();
      }
      else if ( m_autoindent && event->key() == '{' || event->key() == '}' )
      {
          QTextEdit::keyPressEvent ( event );
          autoUnindent();
          if ( m_autobrackets && event->key() == '{' )
              autobrackets();
      }
      else if ( event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace )
      {
          QTextEdit::keyPressEvent ( event );
      }
      else if ( QKeySequence(event->key() | event->modifiers()) == m_mainImpl->shortcutCut() )
      {
          cut();
      }
      else if ( QKeySequence(event->key() | event->modifiers()) == m_mainImpl->shortcutPaste() )
      {
          paste();
      }
      else if ( QKeySequence(event->key() | event->modifiers()) == m_mainImpl->shortcutUndo() )
      {
          document()->undo();
      }
      else if ( QKeySequence(event->key() | event->modifiers()) == m_mainImpl->shortcutRedo() )
      {
          document()->redo();
      }
      else
      {
          QTextEdit::keyPressEvent ( event );
      }
      */
  QTextEdit::keyPressEvent ( event );
  event->accept();
}
//
void TextEdit::key_home()
{
  QTextCursor cursor = textCursor();
  int col = cursor.columnNumber();
  
  cursor.movePosition ( QTextCursor::StartOfLine );
  QTextBlock b = cursor.block();
  int firstWordCol = cursor.columnNumber();
  while ( b.text().at ( firstWordCol ) == ' ' || b.text().at ( firstWordCol ) == '\t' ) {
    cursor.movePosition ( QTextCursor::NextCharacter );
    firstWordCol++;
  }
  
  if ( col > 0
       &&   col == firstWordCol ) {
    cursor.movePosition ( QTextCursor::StartOfLine );
  }
  
  setTextCursor ( cursor );
}

void TextEdit::textPlugin ( TextEditInterface *iTextEdit )
{
  QApplication::setOverrideCursor ( Qt::WaitCursor );
  QTextCursor cursor = textCursor();
  QString s = iTextEdit->text ( m_plainText, cursor.selection().toPlainText() );
  if ( s.isEmpty() ) {
    QApplication::restoreOverrideCursor();
    return;
  }
  if ( iTextEdit->action() == TextEditInterface::ReplaceAll ) {
    int posScrollbar = verticalScrollBar()->value();
    int line = currentLineNumber();
    setPlainText ( s );
    document()->setModified ( true );
    gotoLine ( line, false );
    verticalScrollBar()->setValue ( posScrollbar );
  } else {
    cursor.insertText ( s );
    setTextCursor ( cursor );
  }
  QApplication::restoreOverrideCursor();
}
//
void TextEdit::dropEvent ( QDropEvent * event )
{
  int posScrollbar = verticalScrollBar()->value();
  QString text;
  if ( event->mimeData()->hasText() ) {
    QTextCursor save = textCursor();
    setTextCursor ( cursorForPosition ( QPoint ( event->pos().x(),  event->pos().y() ) ) );
    text = event->mimeData()->text();
    int linesAdded = 0;
    if ( text.length() )
      linesAdded = text.count ( QChar ( '\n' ) );
    setTextCursor ( save );
  }
  QTextEdit::dropEvent ( event );
  if ( event->keyboardModifiers() == Qt::NoModifier ) {
    QTextCursor cursor = textCursor();
    int pos = textCursor().position();
    cursor.setPosition ( pos-text.length(), QTextCursor::MoveAnchor );
    cursor.setPosition ( pos, QTextCursor::KeepAnchor );
    setTextCursor ( cursor );
  }
  verticalScrollBar()->setValue ( posScrollbar );
}

//
//
void TextEdit::mouseDoubleClickEvent ( QMouseEvent * event )
{
  mousePosition = event->pos();
  QTextCursor cursor = textCursor();
  int pos = cursor.position();
  while ( pos>0  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( m_plainText.at ( pos-1 ).toUpper()  ) )
    pos--;
  cursor.setPosition ( pos, QTextCursor::MoveAnchor );
  setTextCursor ( cursor );
  //
  while ( pos < m_plainText.length()  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( m_plainText.at ( pos ).toUpper()  ) )
    pos++;
  cursor.setPosition ( pos, QTextCursor::KeepAnchor );
  setTextCursor ( cursor );
}
//
void TextEdit::contextMenuEvent ( QContextMenuEvent * e )
{
  mousePosition = e->pos();
  m_lineNumber = lineNumber ( e->pos() );
  QMenu *menu = createStandardContextMenu();
  menu->clear();
//    connect(menu->addAction(QIcon(":/toolbar/images/undo.png"), tr("Undo")), SIGNAL(triggered()), this, SLOT(undo()) );
//    connect(menu->addAction(QIcon(":/toolbar/images/redo.png"), tr("Redo")), SIGNAL(triggered()), this, SLOT(redo()) );
  menu->addSeparator();
  connect ( menu->addAction ( QIcon ( ":/toolbar/images/editcut.png" ), tr ( "Cut" ) ), SIGNAL ( triggered() ), this, SLOT ( cut() ) );
  connect ( menu->addAction ( QIcon ( ":/toolbar/images/editcopy.png" ), tr ( "Copy" ) ), SIGNAL ( triggered() ), this, SLOT ( copy() ) );
  connect ( menu->addAction ( QIcon ( ":/toolbar/images/editpaste.png" ), tr ( "Paste" ) ), SIGNAL ( triggered() ), this, SLOT ( paste() ) );
  QTextCursor cursor = textCursor();
  menu->addSeparator();
  connect ( menu->addAction ( tr ( "Select All" ) ), SIGNAL ( triggered() ), this, SLOT ( selectAll() ) );
  menu->addSeparator();
  connect ( menu->addAction ( QIcon ( ":/toolbar/images/find.png" ), tr ( "Find..." ) ), SIGNAL ( triggered() ), m_editor, SLOT ( find() ) );
  menu->addSeparator();
  connect ( menu->addAction ( QIcon ( ":/divers/images/bookmark.png" ), tr ( "Toggle Bookmark" ) ), SIGNAL ( triggered() ), this, SLOT ( slotToggleBookmark() ) );
  connect ( menu->addAction ( QIcon ( ":/divers/images/bookmark.png" ), tr ( "Plot Message" ) ), SIGNAL ( triggered() ), this, SLOT ( slotPlotMessage() ) );
  //
  //
  menu->exec ( e->globalPos() );
  delete menu;
}
//
void TextEdit::selectLines ( int debut, int fin )
{
  if ( debut > fin )
    qSwap ( debut, fin );
  QTextCursor c = textCursor();
  c.movePosition ( QTextCursor::Start );
  c.movePosition ( QTextCursor::Down, QTextCursor::MoveAnchor, debut-1 );
  c.movePosition ( QTextCursor::Down, QTextCursor::KeepAnchor, fin-debut+1 );
  setTextCursor ( c );
  ensureCursorVisible();
}
//
//
QString TextEdit::wordUnderCursor ( const QPoint & pos, bool select )
{
  QTextCursor save ( textCursor() );
  QTextCursor cursor;
  if ( pos.isNull() )
    cursor = textCursor();
  else
    cursor = cursorForPosition ( pos );
  //
  int curpos = cursor.position();
  while ( curpos>0  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( m_plainText.at ( curpos-1 ).toUpper()  ) )
    curpos--;
  cursor.setPosition ( curpos, QTextCursor::MoveAnchor );
  setTextCursor ( cursor );
  //
  while ( curpos < m_plainText.length()  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( m_plainText.at ( curpos ).toUpper()  ) )
    curpos++;
  cursor.setPosition ( curpos, QTextCursor::KeepAnchor );
  QString word = cursor.selectedText().simplified();
  //
  if ( select )
    setTextCursor ( cursor );
  else
    setTextCursor ( save );
  return word;
}
//
QString TextEdit::wordUnderCursor ( const QString text )
{
  int begin = text.length();
  while ( begin>0  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( text.at ( begin-1 ).toUpper()  ) )
    begin--;
  //
  int end = begin;
  while ( end < text.length()  && QString ( "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_" ).contains ( text.at ( end ).toUpper()  ) )
    end++;
  QString word = text.mid ( begin, end-begin );
  return word;
}
//
//
int TextEdit::currentLineNumber ( QTextCursor cursor )
{
  if ( cursor.isNull() )
    return lineNumber ( textCursor() );
  else
    return lineNumber ( cursor );
}
//
int TextEdit::currentLineNumber ( QTextBlock block )
{
  int lineNumber = 1;
  for ( QTextBlock b =document()->begin(); b.isValid(); b = b.next(), lineNumber++ ) {
    if ( b == block ) {
      return lineNumber;
    }
  }
  return -1;
}
//
int TextEdit::lineNumber ( QTextCursor cursor )
{
  QTextBlock blocCurseur = cursor.block();
  int m_lineNumber = 1;
  for ( QTextBlock block =document()->begin(); block.isValid() && block != blocCurseur; block = block.next() )
    m_lineNumber++;
  return m_lineNumber++;
}
//
int TextEdit::lineNumber ( QTextBlock b )
{
  int m_lineNumber = 1;
  for ( QTextBlock block =document()->begin(); block.isValid() && block != b; block = block.next() )
    m_lineNumber++;
  return m_lineNumber++;
}
//
int TextEdit::linesCount()
{
  int line = 1;
  for ( QTextBlock block = document()->begin(); block.isValid(); block = block.next() )
    line++;
  return line;
}
//
int TextEdit::lineNumber ( QPoint point )
{
  return lineNumber ( cursorForPosition ( point ) );
}
//
//
void TextEdit::slotToggleBookmark()
{
  m_editor->toggleBookmark ( m_lineNumber );
  m_lineNumbers->update();
}
//
//
void TextEdit::insertText ( QString text, int insertAfterLine )
{
  if ( m_tabSpaces ) {
    int nbSpaces = tabStopWidth() / fontMetrics().width ( " " );
    QString spaces;
    for ( int i = 0; i<nbSpaces; i++ )
      spaces += " " ;
    text.replace ( "\t", spaces );
  }
  if ( insertAfterLine == -1 ) {
    QTextCursor c = textCursor();
    c.movePosition ( QTextCursor::End );
    c.movePosition ( QTextCursor::EndOfLine );
    c.insertText ( "\n" + text );
    setTextCursor ( c );
  } else {
    gotoLine ( insertAfterLine, false );
    textCursor().insertText ( text );
  }
}
//
//






void TextEdit::updateContents()
{
	if (!m_followContents)
		return;
  QFile file;
  file.setFileName ( m_fileName );
  if ( file.open ( QFile::ReadOnly ) ) {
    QTextStream in ( &file );
    in.seek ( m_bytesRead );
    while ( !in.atEnd() ) {
      QString line=in.readLine();
      append ( line );
    }
    m_bytesRead=in.pos();
  }
  file.close();
}


void TextEdit::slotPlotMessage()
{
  m_editor->togglePlot ( m_lineNumber );
  m_lineNumbers->update();
}

