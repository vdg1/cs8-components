#include "mainwindow.h"
#include "cs8application.h"
#include "cs8applicationmodel.h"
#include "csscilexer.h"
#include "qscilexerval3.h"
#include "ui_mainwindow.h"
#include <QDebug>

void MainWindow::cursorPositionChanged(int currLine, int currPos) {
  qDebug() << __FUNCTION__ << currLine << currPos;
  static int prevLine = -1;
  if (prevLine != currLine) {
    if ((prevLine >= 0) && (currLine != prevLine) &&
        m_updateIndentationNeeded) {
      int indentation = 0;
      for (int line = 0; line < ui->textEdit->lines(); line++) {
        QString lineText = ui->textEdit->text(line);

        QString firstWord = m_rxFirstWord.match(lineText).captured(1);
        if (m_blockStart.contains(firstWord)) {
          // indentation = ui->textEdit->indentation(line);
          ui->textEdit->setIndentation(line, indentation);
          indentation = indentation + 2;
          ui->textEdit->setIndentation(line + 1, indentation);
          // ui->textEdit->setCursorPosition(line + 1, lineText.length() - 1);
        } else if (m_blockEnd.contains(firstWord)) {
          // indentation = ui->textEdit->indentation(line);
          indentation = qMax(indentation - 2, 0);
          ui->textEdit->setIndentation(line, indentation);
        } else {
          ui->textEdit->setIndentation(line, indentation);
        }
        if (line == currLine) {
          qDebug() << "Adjust cursor position" << line << lineText.length();
          ui->textEdit->setCursorPosition(line, lineText.length());
        }
      }
      m_updateIndentationNeeded = false;
    }
    /*
            int prevIndentation = ui->textEdit->indentation(prevLine);
            qDebug() << "prev indentation" << prevIndentation << " of line "
                     << prevLine;
            QString prevText = ui->textEdit->text(prevLine);
            if (prevText.trimmed().startsWith("for")) {
              QString currLine = ui->textEdit->text(line);
              if (currLine.isEmpty()) {
                QString text = ui->textEdit->text();
              } else {
                ui->textEdit->setIndentation(line, prevIndentation + 1);
              }
            }
          }*/
    prevLine = currLine;
  }
}

void MainWindow::textChanged() { m_updateIndentationNeeded = true; }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_app(0) {
  ui->setupUi(this);
  m_pool = new cs8ApplicationModel(this);
  m_app = m_pool->openApplication(
      R"(D:\data\Staubli\SRS\01 Novo Nordisk, Kalundborg\31983 API\Controller1\usr\usrapp\aInsulinHandlin)");
  qDebug() << "app" << m_app;
  ui->textEdit->setText(
      m_app->programModel()->programList()[0]->val3Code(true));
  ui->textEdit->setIndentationGuides(true);
  ui->textEdit->setMarginWidth(0, "0000");
  ui->textEdit->setMarginLineNumbers(0, true);
  ui->textEdit->setFolding(QsciScintilla::CircledTreeFoldStyle);
  ui->textEdit->setEolMode(QsciScintilla::EolUnix);

  ui->textEdit->setCaretForegroundColor(QColor("#ff0000ff"));
  ui->textEdit->setCaretLineVisible(true);
  ui->textEdit->setCaretLineBackgroundColor(QColor("#1f0000ff"));
  ui->textEdit->setCaretWidth(6);

  ui->textEdit->setIndentationsUseTabs(false);
  ui->textEdit->setTabWidth(2);
  ui->textEdit->setIndentationGuides(true);
  ui->textEdit->setIndentationWidth(2);
  ui->textEdit->setTabIndents(false);
  // ui->textEdit->setAutoIndent(true);

  ui->textEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);
  ui->textEdit->setAutoCompletionThreshold(2);

  csSciLexer *lexer = new csSciLexer(this);
  lexer->setApp(m_app);
  ui->textEdit->setLexer(lexer);
  connect(ui->textEdit, &QsciScintilla::cursorPositionChanged, this,
          &MainWindow::cursorPositionChanged);
  connect(ui->textEdit, &QsciScintilla::textChanged, this,
          &MainWindow::textChanged);

  // ui->textEdit->setLexer(new QsciLexerVal3(this));
  m_blockStart =
      QString("begin if switch case default for do while").split(" ");
  m_blockEnd =
      QString("end endWhile endFor endIf until break endSwitch").split(" ");
  m_rxFirstWord.setPattern(R"RX(^\s*([a-zA-Z0-9]+))RX");
}

MainWindow::~MainWindow() { delete ui; }
