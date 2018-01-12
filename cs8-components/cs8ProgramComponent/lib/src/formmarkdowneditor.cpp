#include "formmarkdowneditor.h"
#include "previewpage.h"
#include "ui_formmarkdowneditor.h"

#include <QSettings>
#include <QWebChannel>

FormMarkDownEditor::FormMarkDownEditor(QWidget *parent)
    : QWidget(parent), ui(new Ui::FormMarkDownEditor) {
  ui->setupUi(this);
  PreviewPage *page = new PreviewPage(this);
  ui->preview->setPage(page);

  connect(ui->editor, &QPlainTextEdit::textChanged, [=]() {
    QString text = ui->editor->toPlainText();
    emit textEdited(text);
    m_content.setText(previewText());
  });

  QWebChannel *channel = new QWebChannel(this);
  channel->registerObject(QStringLiteral("content"), &m_content);
  page->setWebChannel(channel);

  ui->preview->setUrl(QUrl("qrc:/index.html"));
  ui->editor->setWordWrapMode(QTextOption::WordWrap);

  ui->editor->document()->setTextWidth(40);
  QFont m_font;
  m_font.setFamily("courier");
  m_font.setPointSize(11);
  ui->editor->setFont(m_font);
  QSettings setting;
  ui->splitter->restoreState(
      setting.value("FormMarkDownEditorSplitter").toByteArray());
}

FormMarkDownEditor::~FormMarkDownEditor() {
  QSettings setting;
  setting.setValue("FormMarkDownEditorSplitter", ui->splitter->saveState());
  delete ui;
}

void FormMarkDownEditor::setText(const QString &text, bool blockSignal) {
  m_content.setText(previewText());
  if (!blockSignal)
    emit textEdited(text);
  emit textChanged(text);
  if (blockSignal)
    ui->editor->blockSignals(true);
  ui->editor->setPlainText(text);
  if (blockSignal)
    ui->editor->blockSignals(false);
}

QString FormMarkDownEditor::text() const { return ui->editor->toPlainText(); }

QString FormMarkDownEditor::prefixText() const { return m_prefixText; }

QString FormMarkDownEditor::previewText() const {
  return (m_prefixText.isEmpty() ? QString("")
                                 : m_prefixText + "<br><hr><br>") +
         ui->editor->toPlainText() +
         (m_postfixText.isEmpty() ? QString("") : "<br><br>" + m_postfixText);
}

void FormMarkDownEditor::setPrefixText(const QString &prefixText) {
  m_prefixText = prefixText;
  m_content.setText(previewText());
}

QString FormMarkDownEditor::postfixText() const { return m_postfixText; }

void FormMarkDownEditor::setPostfixText(const QString &postfixText) {
  m_postfixText = postfixText;
}
