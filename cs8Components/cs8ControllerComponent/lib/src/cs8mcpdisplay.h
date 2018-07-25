#ifndef CS8MCPDISPLAY_H
#define CS8MCPDISPLAY_H

#include <QPoint>
#include <QWidget>

namespace Ui {
class cs8MCPDisplay;
}

class QLabel;
class cs8MCPDisplayRow;

class cs8MCPDisplay : public QWidget {
  Q_OBJECT

public:
  enum TextMode { Normal, Inversed, Blinking, Inversed_Blinking };
  Q_ENUM(TextMode)

  explicit cs8MCPDisplay(QWidget *parent = 0);
  ~cs8MCPDisplay();
  void put(const QString &text);
  void put(QPoint pos, const QString &text);
  void put(int x, int y, const QString &text);
  void setTextMode(TextMode mode);
  void cls();
  void setFontSize(int size);

  QColor background() const;
  void setBackground(const QColor &background);

  QColor foreground() const;
  void setForeground(const QColor &foreground);

signals:
  void toggle(bool);
  void textModeChanged(TextMode);

protected slots:
  void slotToggle(bool value);

private:
  Ui::cs8MCPDisplay *ui;
  QList<cs8MCPDisplayRow *> m_displayRows;
  QTimer *m_flashTimer;
  QPoint m_cursorPos;
  QColor m_background;
  QColor m_foreground;
  TextMode m_textMode;
  bool m_flash;
};

class cs8MCPDisplayRow : public QObject {
  Q_OBJECT

public:
  explicit cs8MCPDisplayRow(QWidget *parent = 0);
  void put(int y, cs8MCPDisplay::TextMode m_mode, const QString &m_text);
  void setFontSize(int size);

  QLabel *getLabel() const;

  QString &getText();

  QByteArray &getMode();

  QColor getForeground() const;
  void setForeground(const QColor &foreground);

  QColor getBackground() const;
  void setBackground(const QColor &background);

public slots:
  void setToggle(bool toggle);

private:
  QLabel *m_label;
  QString m_text;
  QByteArray m_mode;
  QColor m_foreground;
  QColor m_background;
  bool m_toggle;
  void update();
};

#endif // CS8MCPDISPLAY_H
