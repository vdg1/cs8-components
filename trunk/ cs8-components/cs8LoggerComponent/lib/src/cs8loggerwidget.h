#ifndef CS8LOGGERWIDGET_H
#define CS8LOGGERWIDGET_H
//
#include <QTextEdit>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>
#include <QDateTime>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "cs8logstatisticswidget.h"

class Highlighter;
class Editor;
class cs8StatisticModel;
//
class cs8LoggerWidget : public QWidget
{
    Q_OBJECT
    
  public:
    enum Attribute{MachineNumber,
                   ArmNumber,
                   Val3Version,
                   ConfigurationVersion,
                   PowerHourCounter} ;
                   
  signals:
	void togglePlotMessage(const QString &);
    void attributeChanged ( cs8LoggerWidget::Attribute attribute, const QString & value );
    void currentTimeStampChanged ( QDateTime timeStamp );
    void disconnected();
    void connected();
    void markTimeStamp ( QDateTime timeStamp, bool set );
    
  protected slots:
	void slotTogglePlotMessage(const QString & line);
    void slotDisconnected();
    void slotConnected();
    void slotReconnectTimer();
    void slotReadyRead();
    void slotContentsChange ( int position, int charsRemoved, int charsAdded );
    void slotTextChanged();
    void displayError ( QAbstractSocket::SocketError socketError );
    void slotTimeStampMark ( QDateTime timeStamp, bool set )
    {
      qDebug() << "timeStamp";
      emit markTimeStamp ( timeStamp, set );
    }
    void slotCurrentTimeStampMarker ( QDateTime timeStamp );
    
  public slots:
    void save ( const QString & fileName );
    void open ( const QString & fileName );
    void clear();
    void disconnectFromCS8();
    void connectToCS8 ( const QString & host,bool autoReconnect=true );
    void gotoLine ( int lineNumber );
    
  public:
    void setMachineNumber ( QString value )
    {
      m_machineNumber = value;
      emit attributeChanged ( MachineNumber, value );
    }
    void setPowerHourCount ( int value )
    {
      m_powerHourCount = value;
      emit attributeChanged ( PowerHourCounter, QString ( "%1" ).arg ( value ) );
    }
    void setConfigurationVersion ( QString value )
    {
      m_configurationVersion = value;
      emit attributeChanged ( ConfigurationVersion, value );
    }
    void setArmSerialNumber ( QString value )
    {
      m_armSerialNumber = value;
      emit attributeChanged ( ArmNumber, value );
    }
    void setVal3Version ( QString value )
    {
      m_val3Version = value;
      emit attributeChanged ( Val3Version, value );
    }
    QString val3Version()
    {
      return m_val3Version;
    }
    cs8StatisticModel* model()
    {
		qDebug() << m_model;
      return m_model;
    }
    QString configurationVersion()
    {
      return m_configurationVersion;
    }
    int powerHourCount()
    {
      return m_powerHourCount;
    }
    QString machineNumber()
    {
      return m_machineNumber;
    }
    
    Editor* editor() const
    {
      return m_editor;
    }
    
    cs8LoggerWidget ( QWidget * parent=0 );
    
  private:
    QTimer* m_reconnectTimer;
    QString m_host;
    bool m_autoReconnect;
    QTcpSocket* m_connection;
    QString m_configurationVersion;
    int m_powerHourCount;
    QString m_machineNumber;
    QString m_armSerialNumber;
    QString m_val3Version;
    Highlighter *highlighter;
    Editor *m_editor;
    void parseContents ( const QString & text );
    void setup();
  protected:
    cs8StatisticModel* m_model;
};


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    
  public:
    Highlighter ( QTextDocument *parent = 0 );
    
  protected:
    void highlightBlock ( const QString &text );
    
  private:
    struct HighlightingRule
    {
      QRegExp pattern;
      QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    
    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    
    QTextCharFormat keywordFormat;
    QTextCharFormat modeFormat;
    QTextCharFormat emergencyFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};
#endif
