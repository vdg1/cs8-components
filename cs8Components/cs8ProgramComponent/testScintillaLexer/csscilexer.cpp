#include "csscilexer.h"
#include "cs8application.h"
#include <QDebug>
#include <Qsci/qsciscintilla.h>

void csSciLexer::defineStyles() {
  setDefaultColor(QColor("#ff000000"));
  setDefaultPaper(QColor("#ffffffff"));
  setDefaultFont(QFont("Consolas", 14));

  setColor(QColor("#ff000000"), Default);
  setColor(QColor("#ff7f0000"), Comment);
  setColor(QColor("#ff0000bf"), Keyword);
  setColor(QColor("#ff007f00"), Operator);
  setColor(QColor("#ffaaaabf"), String);
  setColor(QColor("#ff007f00"), Number);
  setColor(QColor("#ffff0000"), Unknown);
  setColor(QColor("#ff0000ff"), Brackets);
  setColor(QColor("#ff00ffff"), GlobalData);
  setColor(QColor("#ffddffff"), AliasOperator);
  setColor(QColor("#ff222222"), SubProgram);

  setPaper(QColor("#ffffffff"), Default);
  setPaper(QColor("#ffffffff"), Comment);
  setPaper(QColor("#ffffffff"), Keyword);
  setPaper(QColor("#ffffffff"), Operator);
  setPaper(QColor("#ffffffff"), String);
  setPaper(QColor("#ffffffff"), Number);
  setPaper(QColor("#ffffffff"), Unknown);
  setPaper(QColor("#ffffffff"), Brackets);
  setPaper(QColor("#fff00fff"), GlobalData);
  setPaper(QColor("#ff333333"), AliasOperator);
  setPaper(QColor("#ffffffff"), SubProgram);

  setFont(QFont("Consolas", 14, QFont::Normal), Default);
  setFont(QFont("Consolas", 14, QFont::Normal), Comment);
  setFont(QFont("Consolas", 14, QFont::Bold), Keyword);
  setFont(QFont("Consolas", 14, QFont::Bold, true), Operator);
  setFont(QFont("Consolas", 14, QFont::Thin), String);
  setFont(QFont("Consolas", 14, QFont::Bold), Number);
  setFont(QFont("Consolas", 14, QFont::Bold, true), Unknown);
  setFont(QFont("Consolas", 14, QFont::Thin), Brackets);
  setFont(QFont("Consolas", 14, QFont::Thin), GlobalData);
  setFont(QFont("Consolas", 14, QFont::Bold), AliasOperator);
  setFont(QFont("Consolas", 14, QFont::Thin), SubProgram);
}

cs8Application *csSciLexer::app() const { return m_app; }

void csSciLexer::setApp(cs8Application *newApp) {
  m_app = newApp;
  for (auto var : m_app->globalVariableModel()->variableNameList())
    m_api->add(var);
  for (auto prog : m_app->programModel()->programNameList(true))
    m_api->add(prog);
  m_api->prepare();
}

csSciLexer::csSciLexer(QObject *parent) : QsciLexerCustom(parent) {
  m_api = new QsciAPIs(this);
  defineStyles();

  // tokenRx.setPattern(R"RX([*]\/|\/[*]|\s+|\w+|\W)RX");
  // tokenRx.setPattern(R"RX(\n+|\s+|\w+|\W+)RX");

  tokenRx.setPattern(
      R"RX((?<number>[-]?([0-9]*[.])?[0-9]+)|:|,|\)|\(|"|\+|=|//|\n{1}|\s+|\w+|\W+)RX");

  keywords =
      QString(
          "begin end for to step endFor if else elseIf endIf while endWhile "
          "do until switch case break default endSwitch call return ?")
          .split(" ");

  operators = QString("and or xor true false ! * / + - = ,").split(" ");

  brackets = QString("( ) [ ] { }").split(" ");

  setAutoIndentStyle(QsciScintilla::AiOpening | QsciScintilla::AiClosing);
}

void csSciLexer::styleText(int start, int end) {
  static int prevStart = -1, prevEnd = -1;
  if (prevStart == start && prevEnd == end) {
    return;
  }

  prevStart = start;
  prevEnd = end;
  if (!editor())
    return;

  startStyling(start);
  bool inComment = false;
  bool inString = false;
  QString t = editor()->text(start, end);
  QRegularExpressionMatchIterator i = tokenRx.globalMatch(t);
  int previousStyle = Unknown;
  //
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    // qDebug() << "token:" << match.captured() << match.capturedLength()
    //         << " comment:" << inComment << " string:" << inString;

    if (inComment) {
      setStyling(match.capturedLength(), Comment);
      previousStyle = Comment;
      if (match.captured().at(0) == "\n")
        inComment = false;
    } else if (match.captured().trimmed().startsWith("//")) {
      setStyling(match.capturedLength(), Comment);
      previousStyle = Comment;
      inComment = true;
    } else if (inString) {
      setStyling(match.capturedLength(), String);
      previousStyle = String;
      if (match.captured() == "\"")
        inString = false;
    } else if (match.captured() == "\"") {
      setStyling(match.capturedLength(), String);
      previousStyle = String;
      inString = true;
    } else if (!match.captured("number").isEmpty()) {
      setStyling(match.capturedLength(), Number);
      previousStyle = Number;
    } else if (match.captured() == ":") {
      setStyling(match.capturedLength(), AliasOperator);
      previousStyle = AliasOperator;
    } else if (keywords.contains(match.captured())) {
      setStyling(match.capturedLength(), Keyword);
      previousStyle = Keyword;
    } else if (brackets.contains(match.captured())) {
      setStyling(match.capturedLength(), Brackets);
      previousStyle = Brackets;
    } else if (operators.contains(match.captured())) {
      setStyling(match.capturedLength(), Operator);
      previousStyle = Operator;
    } else if (previousStyle != AliasOperator) {
      if (m_app->globalVariableModel()->variableNameList().contains(
              match.captured())) {
        setStyling(match.capturedLength(), GlobalData);
        previousStyle = GlobalData;
      } else if (m_app->programModel()->programNameList(false).contains(
                     match.captured())) {
        setStyling(match.capturedLength(), SubProgram);
        previousStyle = SubProgram;
      } else if (m_app->libraryModel()->aliasNameList().contains(
                     match.captured())) {
        setStyling(match.capturedLength(), Alias);
        previousStyle = Alias;
      } else {
        setStyling(match.capturedLength(), Unknown);
        previousStyle = Unknown;
      }
    } else {
      setStyling(match.capturedLength(), Unknown);
      previousStyle = Unknown;
    }
  }
}

// Return the list of keywords that can start a block.
const char *csSciLexer::blockStartKeyword_(int *style) const {
  // qDebug() << __FUNCTION__;
  if (style)
    *style = Keyword;

  return "begin case default do else for if while switch elseIf";
}

const char *csSciLexer::blockStart_(int *style) const {
  // qDebug() << __FUNCTION__;
  if (style)
    *style = Operator;

  return "for";
}

// Return the list of characters that can end a block.
const char *csSciLexer::blockEnd_(int *style) const {
  // qDebug() << __FUNCTION__;
  if (style)
    *style = Operator;

  return "endFor";
}

const char *csSciLexer::language() const { return "Val3"; }

QString csSciLexer::description(int style) const {
  switch (style) {
  case Default:
    return "Default";
  case Comment:
    return "Comment";
  case Keyword:
    return "Keyword";
  case Operator:
    return "Operator";
  case String:
    return "String";
  case Unknown:
    return "Unknown";
  case Brackets:
    return "Brackets";
  case Number:
    return "Numbers";
  case GlobalData:
    return "Global Data";
  case LocalData:
    return "Local Data";
  case ParameterData:
    return "Parameter";
  case AliasOperator:
    return "Alias Operator";
  case SubProgram:
    return "Sub Program";
  }

  return QString();
}
