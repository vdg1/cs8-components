#include "cs8mcpdisplay.h"
#include "ui_cs8mcpdisplay.h"

#include <QLabel>
#include <QTimer>

cs8MCPDisplay::cs8MCPDisplay(QWidget *parent)
    : QWidget(parent), ui(new Ui::cs8MCPDisplay), m_cursorPos(QPoint(0, 0)), m_textMode(Normal),
      m_background(QColor(220, 220, 220)), m_foreground(QColor(0, 0, 0)) {
  ui->setupUi(this);

  m_flashTimer = new QTimer(this);
  m_flashTimer->start(500);
  connect(m_flashTimer, &QTimer::timeout, [=]() {
    m_flash = !m_flash;
    emit toggle(m_flash);
  });
  connect(this, &cs8MCPDisplay::toggle, this, &cs8MCPDisplay::slotToggle);

  setStyleSheet(QString("background-color: %1;").arg(m_background.name()));

  QString t = QString("0123456789012345678901234567890123456789");
  for (int i = 0; i < 14; i++) {
    cs8MCPDisplayRow *displayRow = new cs8MCPDisplayRow(this);
    displayRow->setForeground(m_foreground);
    displayRow->setBackground(m_background);
    displayRow->put(0, m_textMode, t);
    connect(this, &cs8MCPDisplay::toggle, displayRow, &cs8MCPDisplayRow::setToggle);

    ui->verticalLayout_3->addWidget(displayRow->getLabel());
    m_displayRows.append(displayRow);
  }
  setFontSize(12);
}

cs8MCPDisplay::~cs8MCPDisplay() { delete ui; }

void cs8MCPDisplay::put(const QString &text) { put(m_cursorPos, text); }

void cs8MCPDisplay::put(QPoint pos, const QString &text) {
  m_cursorPos = pos;
  m_displayRows[m_cursorPos.y()]->put(m_cursorPos.x(), m_textMode, text);
}

void cs8MCPDisplay::put(int x, int y, const QString &text) { put(QPoint(x, y), text); }

void cs8MCPDisplay::setTextMode(cs8MCPDisplay::TextMode mode) {
  if (m_textMode != mode)
    emit textModeChanged(mode);
  m_textMode = mode;
}

void cs8MCPDisplay::cls() {
  m_textMode = Normal;
  QString t;
  t.fill(QChar(' '), 40);
  foreach (cs8MCPDisplayRow *l, m_displayRows) { l->put(0, m_textMode, t); }
}

void cs8MCPDisplay::setFontSize(int size) {
  QFontMetrics fontMetric = QFontMetrics(QFont("courier", size));
  QSize fontSize = fontMetric.size(0, "Q");
  setFixedSize(fontSize.width() * 40, fontSize.height() * 14);
  foreach (cs8MCPDisplayRow *l, m_displayRows) { l->setFontSize(size); }
}

void cs8MCPDisplay::slotToggle(bool value) {}

cs8MCPDisplayRow::cs8MCPDisplayRow(QWidget *parent)
    : QObject(parent), m_label(new QLabel(parent)), m_text(QString().fill(' ', 40)),
      m_mode(QByteArray().fill(cs8MCPDisplay::Normal, 40)), m_toggle(false) {}

void cs8MCPDisplayRow::update() {
  // take mode into account
  /*
   * <html><head/><body><p>Text<span style=" color:#ff5500;">Label </span><span style="
   * color:#55aa00;">inversed</span></p></body></html>
   * */
  QString labeltext("<html><head/><body><p>");
  QColor fg = m_foreground;
  QColor bg = m_background;
  for (int i = 0; i < 40; i++) {
    if (i == 0 || (m_mode[i] != m_mode[i - 1])) {
      switch (m_mode[i]) {
      case cs8MCPDisplay::Normal:
        fg = m_foreground;
        bg = m_background;
        break;

      case cs8MCPDisplay::Inversed:
        fg = m_background;
        bg = m_foreground;
        break;

      case cs8MCPDisplay::Blinking:
        fg = m_toggle ? m_foreground : m_background;
        bg = m_background;
        break;

      case cs8MCPDisplay::Inversed_Blinking:
        fg = m_toggle ? m_background : m_foreground;
        bg = m_foreground;
        break;
      }
      if (i != 0) {
        labeltext += "</span>";
      }
      labeltext += QString("<span style=\"color:%1; background-color:%2;\">").arg(fg.name()).arg(bg.name());
    }
    // replace spaces with HTML space
    labeltext += m_text[i] == ' ' ? QString("&#160;") : QString(m_text[i]);
  }
  labeltext += "</span></p></body></html>";
  m_label->setText(labeltext);
}

void cs8MCPDisplayRow::put(int y, cs8MCPDisplay::TextMode mode, const QString &text) {
  m_text.replace(y, text.length(), text);
  m_text = m_text.left(40);
  QByteArray mb;
  mb.fill(mode, text.length());
  m_mode.replace(y, text.length(), mb);

  update();

  // m_label->setText(m_text);
}

void cs8MCPDisplayRow::setFontSize(int size) {
  QFont font = QFont("courier", size);
  QFontMetrics fontMetric = QFontMetrics(font);
  QSize fontSize = fontMetric.size(0, "Q");
  m_label->setFont(font);
  m_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_label->setGeometry(0, 0, fontSize.width() * 40, fontSize.height());
}

QLabel *cs8MCPDisplayRow::getLabel() const { return m_label; }

QString &cs8MCPDisplayRow::getText() { return m_text; }

QByteArray &cs8MCPDisplayRow::getMode() { return m_mode; }

QColor cs8MCPDisplayRow::getForeground() const { return m_foreground; }

void cs8MCPDisplayRow::setForeground(const QColor &foreground) { m_foreground = foreground; }

QColor cs8MCPDisplayRow::getBackground() const { return m_background; }

void cs8MCPDisplayRow::setBackground(const QColor &background) { m_background = background; }

void cs8MCPDisplayRow::setToggle(bool toggle) {
  m_toggle = toggle;
  update();
}
