#include "cs8loggerwidget.h"
#include "cs8statisticmodel.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include "editor.h"

//
cs8LoggerWidget::cs8LoggerWidget ( QWidget * parent )
    : QWidget ( parent )
{
  setup();
  
}

Highlighter::Highlighter ( QTextDocument *parent )
    : QSyntaxHighlighter ( parent )
{
  HighlightingRule rule;
  
  keywordFormat.setForeground ( Qt::darkBlue );
  keywordFormat.setFontWeight ( QFont::Bold );
  QStringList keywordPatterns;
  keywordPatterns << "\\bUSR:\\b";
  foreach ( QString pattern, keywordPatterns ) {
    rule.pattern = QRegExp ( pattern );
    rule.format = keywordFormat;
    highlightingRules.append ( rule );
  }
  
  
  emergencyFormat.setFontWeight ( QFont::Bold );
  emergencyFormat.setForeground ( Qt::red );
  emergencyFormat.setFontUnderline ( true );
  rule.pattern = QRegExp ( "\\bemergency stop\\b" );
  rule.format = emergencyFormat;
  highlightingRules.append ( rule );
  
  modeFormat.setFontWeight ( QFont::Normal );
  modeFormat.setForeground ( Qt::cyan );
  modeFormat.setFontUnderline ( false );
  rule.pattern = QRegExp ( "\\bWorking mode\\b" );
  rule.format = modeFormat;
  highlightingRules.append ( rule );
  
  /*
      singleLineCommentFormat.setForeground(Qt::red);
      rule.pattern = QRegExp("//[^\n]*");
      rule.format = singleLineCommentFormat;
      highlightingRules.append(rule);
  
      multiLineCommentFormat.setForeground(Qt::red);
  
      quotationFormat.setForeground(Qt::darkGreen);
      rule.pattern = QRegExp("\".*\"");
      rule.format = quotationFormat;
      highlightingRules.append(rule);
  
      functionFormat.setFontItalic(true);
      functionFormat.setForeground(Qt::blue);
      rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
      rule.format = functionFormat;
      highlightingRules.append(rule);
  */
  //  commentStartExpression = QRegExp("/\\*");
  //  commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock ( const QString &text )
{
  //qDebug() << "new block: " << text;
  foreach ( HighlightingRule rule, highlightingRules ) {
    QRegExp expression ( rule.pattern );
    int index = text.indexOf ( expression );
    /*while (index >= 0)
    {
        int length = expression.matchedLength();
        setFormat(index, length, rule.format);
        index = text.indexOf(expression, index + length);
    }
    */
    
    if ( index >= 0 ) {
      setFormat ( 0, text.length(), rule.format );
    }
  }
  
  return;
  setCurrentBlockState ( 0 );
  
  int startIndex = 0;
  if ( previousBlockState() != 1 )
    startIndex = text.indexOf ( commentStartExpression );
    
  while ( startIndex >= 0 ) {
    int endIndex = text.indexOf ( commentEndExpression, startIndex );
    int commentLength;
    if ( endIndex == -1 ) {
      setCurrentBlockState ( 1 );
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex
                      + commentEndExpression.matchedLength();
    }
    setFormat ( startIndex, commentLength, multiLineCommentFormat );
    startIndex = text.indexOf ( commentStartExpression,
                                startIndex + commentLength );
  }
}
//

void cs8LoggerWidget::setup()
{
  m_editor=new Editor ( this,this );
  m_editor->textEdit()->setReadOnly(true);
  QHBoxLayout *hboxLayout = new QHBoxLayout();
  hboxLayout->setSpacing ( 2 );
  hboxLayout->setMargin ( 2 );
  hboxLayout->setObjectName ( QString::fromUtf8 ( "hboxLayout" ) );
  //
  hboxLayout->addWidget ( m_editor );
  setLayout ( hboxLayout );
  
  connect ( m_editor->textEdit()->document(), SIGNAL ( contentsChange ( int,int,int ) ),
            this, SLOT ( slotContentsChange ( int,int,int ) ) );
  connect ( m_editor,SIGNAL ( markTimeStamp ( QDateTime, bool ) ),
            this,SLOT ( slotTimeStampMark ( QDateTime, bool ) ) );
  connect ( m_editor,SIGNAL ( currentTimeStampChanged ( QDateTime ) ),
            this,SLOT ( slotCurrentTimeStampMarker ( QDateTime ) ) );
  connect ( m_editor,SIGNAL ( togglePlotMessage ( const QString & ) ),
            this,SLOT ( slotTogglePlotMessage ( const QString & ) ) );
            
            
            
  m_model = new cs8StatisticModel ( this );
  
  m_connection = new QTcpSocket ( this );
  connect ( m_connection, SIGNAL ( readyRead() ),
            this, SLOT ( slotReadyRead() ) );
  connect ( m_connection, SIGNAL ( connected() ),
            this, SLOT ( slotConnected() ) );
  connect ( m_connection, SIGNAL ( disconnected() ),
            this, SLOT ( slotDisconnected() ) );
  connect ( m_connection, SIGNAL ( error ( QAbstractSocket::SocketError ) ),
            this, SLOT ( displayError ( QAbstractSocket::SocketError ) ) );
            
  m_reconnectTimer = new QTimer ( this );
  connect ( m_reconnectTimer,SIGNAL ( timeout() ),this, SLOT ( slotReconnectTimer() ) );
  qDebug() << "ctor: " << m_model;
}

void cs8LoggerWidget::open ( const QString & fileName )
{
  clear();
  m_editor->open ( fileName );
  
}

void cs8LoggerWidget::save ( const QString & fileName )
{
  QFile file ( fileName ); // Write the text to a file
  if ( file.open ( QIODevice::WriteOnly ) ) {
    QTextStream stream ( &file );
    //stream << toPlainText();
    //document()->setModified(false);
  }
}

void cs8LoggerWidget::parseContents ( const QString & text )
{
  QString machineNumber;
  QStringList list=text.split ( "\n" );
  int lineNumber=1;
  foreach ( QString line,list ) {
  
    if ( line.contains ( "Machine Number: " ) ) { 
      qDebug() << "parseContents(): found machine number line "<< lineNumber;
      QString machineNumber=line.right ( line.length()-line.indexOf ( "r: " )-3 ).trimmed();
      if ( machineNumber!=m_machineNumber ) {
      	setMachineNumber(machineNumber);
      }
    }else
    if ( line.contains ( "Arm serial number:" ) ) {
      qDebug() << "parseContents(): found arm serial number line "<< lineNumber;
      QString armSerialNumber=line.right ( line.length()-line.indexOf ( "r: " )-3 ).trimmed();
      if ( armSerialNumber!=m_armSerialNumber ) {
      	setArmSerialNumber(armSerialNumber);
      }
    }else
    if ( line.contains ( "Power hour count = " ) ) {
      qDebug() << "parseContents(): found power hour count line "<<lineNumber;
      int powerHourCount=line.right ( line.length()-line.indexOf ( " = " )-3 ).trimmed().toInt();
      //if (powerHourCount!=m_powerHourCount)
      {
      	setPowerHourCount(powerHourCount);
      }
    }else
    if ( line.contains ( "Configuration Version" ) ) {
      qDebug() << "parseContents(): Configuration Version line "<<lineNumber;
      QString configurationVersion =line.right ( line.length()-line.indexOf ( "n: " )-3 ).trimmed();
      if ( configurationVersion!=m_configurationVersion ) {
      	setConfigurationVersion(configurationVersion);
      }
    }
    else
    if ( line.contains ( "VAL3 " ) ) {
      qDebug() << "parseContents(): Val3 Version line "<<lineNumber;
      QString val3Version =line.right ( line.length()-line.indexOf ( " - " )-3 ).trimmed();
      if ( val3Version!=m_val3Version ) {
      	setVal3Version(val3Version);
      }
    }
    
    m_model->addStatisticInfo ( line, lineNumber );
    lineNumber++;
  }
}

void cs8LoggerWidget::slotTextChanged()
{
  parseContents ( m_editor->toPlainText() );
}

void cs8LoggerWidget::slotContentsChange ( int position, int charsRemoved, int charsAdded )
{
  if ( charsAdded>0 ) {
    qDebug() << charsAdded << " at " << position;
    QString text=m_editor->toPlainText().mid ( position,charsAdded );
    parseContents ( text );
  }
}


void cs8LoggerWidget::connectToCS8 ( const QString & host, bool autoReconnect )
{
  m_connection->abort();
  m_connection->connectToHost ( host,23 );
  m_host=host;
  m_autoReconnect=autoReconnect;
  clear();
}


void cs8LoggerWidget::displayError ( QAbstractSocket::SocketError socketError )
{
  qDebug() << "connection error: " << socketError;
  switch ( socketError ) {
    case QAbstractSocket::RemoteHostClosedError:
      if ( m_autoReconnect ) {
        m_reconnectTimer->start ( 2000 );
      }
      break;
    case QAbstractSocket::HostNotFoundError:
      QMessageBox::information ( this, tr ( "cs8LoggerWidget" ),
                                 tr ( "The host was not found. Please check the "
                                      "host name and port settings." ) );
      break;
    case QAbstractSocket::ConnectionRefusedError:
      QMessageBox::information ( this, tr ( "cs8LoggerWidget" ),
                                 tr ( "The connection was refused by the peer. " ) );
      break;
    default:
      QMessageBox::information ( this, tr ( "cs8LoggerWidget" ),
                                 tr ( "The following error occurred: %1." )
                                 .arg ( m_connection->errorString() ) );
  }
}

void cs8LoggerWidget::slotReadyRead()
{
  while ( m_connection->canReadLine() ) {
    QString line=m_connection->readLine();
    line.chop ( 2 );
    if ( line.length() >4 )
      m_editor->textEdit()->append ( line );
  }
}

void cs8LoggerWidget::disconnectFromCS8()
{
  m_connection->abort();
}

void cs8LoggerWidget::clear()
{
  m_editor->textEdit()->clear();
  m_model->clear();
  setArmSerialNumber ( "" );
  setConfigurationVersion ( "" );
  setMachineNumber ( "" );
  setPowerHourCount ( 0 );
}

void cs8LoggerWidget::slotReconnectTimer()
{
  qDebug() << "reconnect";
  if ( m_autoReconnect ) {
    m_connection->abort();
    m_connection->connectToHost ( m_host,23 );
  }
}

void cs8LoggerWidget::slotConnected()
{
  qDebug() << "connected";
  emit connected();
  m_reconnectTimer->stop();
}

void cs8LoggerWidget::slotDisconnected()
{
  qDebug() << "disconnected";
  emit disconnected();
}


/*!
    \fn cs8LoggerWidget::gotoLine(int lineNumber)
 */
void cs8LoggerWidget::gotoLine ( int lineNumber )
{
  m_editor->textEdit()->gotoLine ( lineNumber,true );
}

void cs8LoggerWidget::slotCurrentTimeStampMarker(QDateTime timeStamp)
{
	emit currentTimeStampChanged(timeStamp);
}


void cs8LoggerWidget::slotTogglePlotMessage(const QString & line)
{
	emit togglePlotMessage(line);
}
