#include "cs8program.h"
#include "cs8application.h"
#include "cs8variable.h"

#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QTextCodec>
#include <QXmlStreamWriter>

#define MAX_LENGTH 40
//
cs8Program::cs8Program(QObject *parent)
    : QObject(parent), m_public(false), m_withIfBlock(true),
      m_hasByteOrderMark(true) {
  m_localVariableModel = new cs8LocalVariableModel(this);
  connect(m_localVariableModel, &cs8LocalVariableModel::modified, this,
          &cs8Program::modified);
  m_parameterModel = new cs8ParameterModel(this);
  connect(m_parameterModel, &cs8LocalVariableModel::modified, this,
          &cs8Program::modified);
  m_globalDocContainer = false;
  m_programCode = "begin\nend";
  m_application = qobject_cast<cs8Application *>(parent);
}

cs8Program::cs8Program()
    : QObject(), m_application(0), m_public(false), m_withIfBlock(true),
      m_hasByteOrderMark(true) {
  m_localVariableModel = new cs8LocalVariableModel(this);
  connect(m_localVariableModel, &cs8LocalVariableModel::modified, this,
          &cs8Program::modified);
  m_parameterModel = new cs8ParameterModel(this);
  connect(m_parameterModel, &cs8LocalVariableModel::modified, this,
          &cs8Program::modified);
  m_globalDocContainer = false;
  m_programCode = "begin\nend";
}

bool cs8Program::open(const QString &filePath) {
  // qDebug() << "cs8Program::open () " << filePath;

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  m_hasByteOrderMark =
      QTextCodec::codecForUtfText(file.peek(4), nullptr) != nullptr;
  // if (m_hasByteOrderMark)
  // qDebug() << "File has BOM";

  QDomDocument doc;
  if (!doc.setContent(&file)) {
    file.close();
    return false;
  }
  file.close();

  m_filePath = filePath;
  return parseProgramDoc(doc);
}

bool cs8Program::deleteSourceFile() {
  // qDebug() << "delete file: " << m_filePath;
  return QFile::remove(m_filePath);
}

void cs8Program::printChildNodes(const QDomElement &element) {
  qDebug() << "Child nodes of: " << element.tagName();
  for (int i = 0; i < element.childNodes().count(); i++)
    qDebug() << i << element.childNodes().at(i).nodeName();
}

void cs8Program::updateCodeModel() {
  // qDebug() << __FUNCTION__ << name();
  parseDocumentation(val3Code());
  // update token list
  m_variableTokens = variableTokens(false);
  // update variable occurences
  QString c = val3Code(false);
  QRegularExpression rx;
  QString pattern = R"RX((\b(%1)\b)(?=([^\"]*\"[^\"]*\")*[^\"]*$))RX";
  foreach (cs8Variable *var, m_localVariableModel->variableList()) {
    var->clearSymbolReferences();
    rx.setPattern(pattern.arg(var->name()));
    QRegularExpressionMatchIterator i = rx.globalMatch(c);
    while (i.hasNext()) {
      QRegularExpressionMatch match = i.next();
      QString word = match.captured(2);
      int symbolPos = match.capturedStart(2);
      int line = c.left(symbolPos).count(QRegExp("\n")) + 1 + m_headerLines;
      int lineStart = c.left(symbolPos).lastIndexOf(QRegExp("\n"));
      int column = symbolPos - lineStart;
      if (!c.mid(lineStart, column).trimmed().startsWith("//"))
        var->addSymbolReference(line, column, "");
    }
    qDebug() << "occurence of local var:" << var->name() << ":"
             << var->symbolReferences();
  }

  foreach (cs8Variable *var, m_parameterModel->variableList()) {
    var->clearSymbolReferences();
    rx.setPattern(pattern.arg(var->name()));
    QRegularExpressionMatchIterator i = rx.globalMatch(c);
    while (i.hasNext()) {
      QRegularExpressionMatch match = i.next();
      QString word = match.captured(2);
      int symbolPos = match.capturedStart(2);
      int line = c.left(symbolPos).count(QRegExp("\n")) + 1 + m_headerLines;
      int lineStart = c.left(symbolPos).lastIndexOf(QRegExp("\n"));
      int column = symbolPos - lineStart;
      if (!c.mid(lineStart, column).trimmed().startsWith("//"))
        var->addSymbolReference(line, column, name());
    }
    qDebug() << "occurence of parameter:" << var->name() << ":"
             << var->symbolReferences();
  }

  if (m_application) {
    foreach (cs8Variable *var,
             m_application->globalVariableModel()->variableList()) {
      // var->clearLineOccurences();
      rx.setPattern(pattern.arg(var->name()));
      QRegularExpressionMatchIterator i = rx.globalMatch(c);
      while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(2);
        int symbolPos = match.capturedStart(2);
        int line = c.left(symbolPos).count(QRegExp("\n")) + 1 + m_headerLines;
        int lineStart = c.left(symbolPos).lastIndexOf(QRegExp("\n"));
        int column = symbolPos - lineStart;
        if (!c.mid(lineStart, column).trimmed().startsWith("//"))
          var->addSymbolReference(line, column, name());
      }
      if (var->symbolReferences().count() > 0)
        qDebug() << "occurence of global var" << var->name() << ":"
                 << var->symbolReferences();
    }
  }
}

void cs8Program::parseProgramSection(const QDomElement &programSection,
                                     const QString &code) {
  setPublic(programSection.attribute("access", "private") == "public" ? true
                                                                      : false);
  m_name = programSection.attribute("name");

  // printChildNodes(m_programSection);
  if (programSection.isNull())
    qDebug() << "Reading program section failed";

  QDomElement descriptionSection =
      programSection.elementsByTagName("Description").at(0).toElement();
  m_briefDescription = descriptionSection.text();

  QTextCodec::ConverterState state;

  QDomElement paramSection =
      programSection.elementsByTagName("Parameters").at(0).toElement();
  QDomElement localSection =
      programSection.elementsByTagName("Locals").at(0).toElement();

  if (code.isEmpty()) {
    QDomElement codeSection =
        programSection.elementsByTagName("Code").at(0).toElement();
    m_lineNumberCodeSection = codeSection.lineNumber();
    if (codeSection.isNull())
      qDebug() << "Reading code section failed";
    m_programCode = codeSection.text();

  } else {
    m_programCode = code;
  }

  QDomNodeList childList;
  childList = localSection.elementsByTagName("Local");
  for (int i = 0; i < childList.count(); i++) {
    QDomElement item = childList.at(i).toElement();
    m_localVariableModel->addVariable(item);
  }

  childList = paramSection.elementsByTagName("Parameter");
  for (int i = 0; i < childList.count(); i++) {
    QDomElement item = childList.at(i).toElement();
    m_parameterModel->addVariable(item);
  }

  //
  updateCodeModel();
}

bool cs8Program::parseProgramDoc(const QDomDocument &doc, const QString &code) {
  QDomElement programsSection = doc.documentElement();
  Q_ASSERT(!programsSection.isNull());
  // printChildNodes(m_programsSection);
  QDomElement programSection = programsSection.firstChild().toElement();
  parseProgramSection(programSection, code);
  return true;
}

QString &rtrim(QString &str) {
  while (str.size() > 0 && str.at(str.size() - 1).isSpace())
    str.chop(1);
  return str;
}

void cs8Program::tidyUpCode(QString &code) {
  // remove double \r
  code = code.remove("\r\r");
  // remove empty lines after "end"
  QStringList list = code.split("\n");
  while (list.length() > 0 && list.last().simplified().isEmpty())
    list.removeLast();

  for (auto &i : list) {
    i = rtrim(i);
  }
  code = list.join("\n");
}

void cs8Program::setLinterDirective(const QString &directive,
                                    const QString &symbolName) {
  auto var = m_localVariableModel->getVarByName(symbolName);
  if (!var)
    var = m_parameterModel->getVarByName(symbolName);
  if (!var)
    var = m_application->globalVariableModel()->getVarByName(symbolName);

  if (var)
    var->setLinterDirective(directive);
}

QString cs8Program::getAdditionalHintMessage() const {
  return m_additionalHintMessage;
}

void cs8Program::setAdditionalHintMessage(
    const QString &additionalHintMessage) {
  m_additionalHintMessage = additionalHintMessage;
}

QString cs8Program::getFilePath() const { return m_filePath; }

void cs8Program::setFilePath(const QString &filePath) { m_filePath = filePath; }

bool cs8Program::getHasByteOrderMark() const { return m_hasByteOrderMark; }

void cs8Program::setHasByteOrderMark(bool hasByteOrderMark) {
  m_hasByteOrderMark = hasByteOrderMark;
}

int cs8Program::getLineNumberCodeSection() const {
  return m_lineNumberCodeSection;
}

void cs8Program::undoTranslationTags() {
  // m_programCode
  QRegularExpression rx("(i18n:tr\\[\\\")(.*)(\\\"\\])");
  rx.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
  QRegularExpressionMatch m;

  m = rx.match(m_programCode);
  // qDebug() << m.hasMatch() << m.lastCapturedIndex() << m.capturedTexts();
  while (m.hasMatch() && m.lastCapturedIndex() == 3) {
    m_programCode.replace(m.capturedStart(3), m.capturedLength(3), "\")");
    m_programCode.replace(m.capturedStart(1), m.capturedLength(1), "(\"");
    m = rx.match(m_programCode);
  }
}
// QDomElement cs8Program::programsSection() const { return m_programsSection; }

// void cs8Program::setProgramsSection(const QDomElement &programsSection) {
//  m_programsSection = programsSection;
//}

void cs8Program::setApplicationDocumentation(
    const QString &applicationDocumentation) {
  m_applicationDocumentation = applicationDocumentation;
}

void cs8Program::setBriefModuleDocumentation(
    const QString &briefDocumentation) {
  m_briefModuleDocumentation = briefDocumentation;
}

void cs8Program::setMainPageDocumentation(
    const QString &applicationDocumentation) {
  m_mainPageDocumentation = applicationDocumentation;
}

// returns the code without documentation header
QString cs8Program::val3Code(bool withDocumentation) const {
  // qDebug() << m_codeSection.text();
  return withDocumentation ? m_programCode : extractCode(m_programCode);
}

QString cs8Program::toCSyntax() {
  QString val3_ = val3Code(false);
  val3_ = val3_.replace(QString("call "), QString(""), Qt::CaseSensitive);
  val3_ = val3_.replace(":", ".");
  return val3_;
}

QString cs8Program::extractCode(const QString &code_) const {
  QString code = code_;
  int headerLines;
  extractDocumentation(code, headerLines);
  // process only if a header was found
  if (headerLines != 0) {
    // extract code
    code.replace("\r\n", "\n");
    QStringList list = code.split("\n");
    // int start = list.indexOf(QRegExp("\\s*begin\\s*"));
    // int stop = start + 1;

    // * while ( list.at( stop ).contains( QRegExp( "^\\s*'//'.*" ) ) ||
    // list.at( stop ).simplified().isEmpty() )
    //      stop++;
    //   for ( int i = stop - 1; i > start; i-- )
    //       list.removeAt( i );
    //
    for (int i = 0; i <= headerLines; i++)
      list.removeFirst();
    // remove any comment or empty line in the beginning of code
    // while (list.at(0).contains(QRegExp("^(\\s*(/{2,}.*)|\\s+)$")))
    //  list.removeFirst();
    list.insert(0, "begin");
    return list.join("\n");
  } else
    return code_;
}

// return a list of variable tokens
QStringList cs8Program::variableTokens(bool onlyModifiedVars) {
  QStringList list;
  QString code = val3Code(true);
  foreach (const QString &line, code.split("\n")) {
    QString l = line.trimmed();
    // ignore comment lines
    if (!l.startsWith("//")) {
      // remove string from code line
      QRegExp rx;
      rx.setPattern(R"(\".*\")");
      rx.setMinimal(true);
      int pos = 0;
      while (rx.indexIn(l, pos) != -1)
        l.replace(rx, " ");
      //
      if (onlyModifiedVars)
        rx.setPattern("(\\w+)\\s*=");
      else
        rx.setPattern("(\\w+)");
      rx.setMinimal(false);
      pos = 0;
      while ((pos = rx.indexIn(l, pos)) != -1) {
        list << rx.cap(1);
        pos += rx.matchedLength();
      }
    }
  }
  return list;
}

void cs8Program::setCode(const QString &code, bool parseDoc_) {
  m_programCode = code;
  if (parseDoc_)
    updateCodeModel();
}

void cs8Program::copyFromParameterModel(cs8ParameterModel *sourceModel) {
  m_parameterModel->clear();
  foreach (cs8Variable *param, sourceModel->variableList()) {
    QDomElement element = param->element().cloneNode(true).toElement();
    m_parameterModel->addVariable(element, param->description());
  }
}

void cs8Program::addTag(const QString &tagType, const QString &tagName,
                        const QString &tagText) {
  m_tags.insertMulti(tagType, tagName + " " + tagText);
}

void cs8Program::clearDocumentationTags() { m_tags.clear(); }

void cs8Program::setWithUndocumentedSymbols(bool withUndocumentedSymbols) {
  m_parameterModel->setWithUndocumentedSymbols(withUndocumentedSymbols);
  m_localVariableModel->setWithUndocumentedSymbols(withUndocumentedSymbols);
}

QStringList cs8Program::referencedVariables() const { return m_variableTokens; }

QMap<int, QString> cs8Program::todos() const {
  QString code = val3Code(true);
  QMap<int, QString> todos;
  // qDebug() << "Check todos in " << name();
  QRegExp rx;
  rx.setPattern(R"(^\s*//[/\\]{1}\s*TODO.*)");
  int codeLine = 0;
  foreach (const QString &line, code.split("\n")) {
    if (rx.indexIn(line) != -1)
      todos.insert(codeLine, line.trimmed());
    codeLine++;
  }
  return todos;
}

QStringList cs8Program::getCalls() {
  QString code = val3Code(true);
  // qDebug() << "Get calls in " << name();
  QRegExp rxCall, rxTaskCreate;
  rxCall.setPattern("^\\s*call (\\w*)\\(");
  rxTaskCreate.setPattern(
      "^(\\s*taskCreate\\s*\\\"*\\w*\\\"*\\s*,\\s*\\w*\\s*,\\s*)(\\w*)\\(");

  QStringList list;
  foreach (const QString &line, code.split("\n")) {
    if (rxCall.indexIn(line) != -1) {
      if (rxCall.captureCount() == 1)
        list << rxCall.cap(1);
    } else if (rxTaskCreate.indexIn(line) != -1) {
      if (rxTaskCreate.captureCount() == 2)
        list << rxTaskCreate.cap(2);
    }
  }
  return list;
}

int cs8Program::cyclomaticComplexity() const {
  int c = 0;
  int nodes = 0, edges = 0;
  bool previousLineIsNode = false;
  QRegularExpression nodeRX(
      R"RX(^(if|for|elseIf|while|until|case)((\s){1,}|\())RX");
  // R"RX(^((if|for|else|elseIf|while|until|switch)((\s){1,}|\()|(endIf|endWhile|endFor|endSwitch)))RX");
  // qDebug() << "rx is valid: " << nodeRX.isValid() << nodeRX.errorString()
  //         << nodeRX.patternErrorOffset();
  nodeRX.setPatternOptions(QRegularExpression::DontCaptureOption);
  foreach (auto line, val3Code(false).split("\n")) {
    line = line.trimmed();

    bool isNode = nodeRX.match(line).hasMatch();
    if (isNode) {
      nodes++;
    } else {
      if (previousLineIsNode)
        edges++;
    }
    previousLineIsNode = isNode;
  }

  // c = edges - nodes + 2;
  c = nodes + 1;
  return c;
}

QStringList cs8Program::extractDocumentation(const QString &code_,
                                             int &headerLinesCount) const {
  //
  // extract documentation section
  QString documentationSection = code_;
  documentationSection.replace("\r\n", "\n");
  // revert automatic correction done by Val3 Studio
  documentationSection.replace("//\\endIf", "//\\endif");
  // documentationSection.remove(0, documentationSection.indexOf("begin\n"));
  QStringList documentationList = documentationSection.split("\n");
  // remove 'begin' token
  documentationList.removeFirst();
  QStringList documentation;
  bool isComment = false;
  bool isEndMarker = false;
  headerLinesCount = 0;
  int row = 0;
  //
  if (documentationList.length() == 0)
    return QStringList();
  // extract code until non-header line found
  QString line = documentationList[row].trimmed();
  isEndMarker = line.startsWith("//_") &&
                (row < documentationList.count() &&
                 !(documentationList[row + 1].trimmed().startsWith("//_")));

  isComment =
      (line.startsWith("//") ||
       ((line.startsWith("//!") || line.startsWith("//\\")) && row == 0) ||
       line.startsWith("if false") || line.startsWith("endIf") ||
       line.startsWith("//_"));
  while (isComment && !isEndMarker) {
    line = documentationList[row].trimmed();
    isEndMarker = line.startsWith("//_") &&
                  (row < documentationList.count() &&
                   !(documentationList[row + 1].trimmed().startsWith("//_")));
    if (line.startsWith("//") || line.startsWith("if false") ||
        line.startsWith("endIf") || line.startsWith("//_") || line.isEmpty()) {
      isComment = true;
    } else {
      isComment = false;
    }
    row++;
  }
  int endOfCommentBlock = qMax(0, row - (isEndMarker ? 0 : 1));
  //
  // check if a non-comment line is in if block

  int hasIfBlock = -1;
  int hasEndIf = -1;
  for (row = 0; row < endOfCommentBlock; row++) {
    line = documentationList[row].trimmed();
    if (line.startsWith("if false")) {
      hasIfBlock = row;
    } else if (hasIfBlock != -1 && !line.startsWith("//") &&
               !line.startsWith("endIf")) {
      // ok, we have hit a non-comment line in if block
      // ==> comment block actually stops just before if false line
      endOfCommentBlock = hasIfBlock;
    } else if (line.startsWith("endIf")) {
      hasEndIf = row;
    }
  }
  // check if documentation header is inclosed in if false .. endIf
  // if so, ignore all following comments and do not include them i
  // documentation header
  if (hasEndIf != -1) {
    endOfCommentBlock = hasEndIf + 1;
  }
  //
  // extract comment block
  documentation = documentationList.mid(0, endOfCommentBlock);
  for (auto &str : documentation)
    str = str.trimmed();
  // remove if false and endIf lines
  documentation.removeAll("if false");
  documentation.removeAll("endIf");
  while (documentation.count() > 0 && documentation.last() == "//_")
    documentation.removeLast();
  headerLinesCount = endOfCommentBlock;
  return documentation;
}

bool cs8Program::globalDocContainer() const { return m_globalDocContainer; }

void cs8Program::setGlobalDocContainer(bool globalDocContainer) {
  m_globalDocContainer = globalDocContainer;
}

/*!
 \fn cs8Program::parseDocumentation( QString & code)
 */

void cs8Program::parseDocumentation(const QString &code_) {

  int headerLines;
  QStringList documentation = extractDocumentation(code_, headerLines);
  QString tagType;
  QString tagName;
  QString tagText;
  m_tags.clear();

  // qDebug() << "parseDocumentation: " << name();
  foreach (QString line, documentation) {
    if (line.length() > 2)
      line = line.trimmed();
    // process a complete tag before starting the next tag
    if ((line.startsWith("//!") || line.startsWith("//\\")) &&
        !tagType.isEmpty()) {
      // remove trailing array indexes from tag name
      tagName = tagName.remove(QRegExp(R"(\[\d*\])")).remove("\\");
      // qDebug() << "process tag:" << tagType << ":" << tagName << ":" <<
      // tagText;
      if (tagType == "param") {
        auto var = m_parameterModel->getVarByName(tagName);
        if (var != nullptr)
          var->setDescription(tagText);
      } else if (tagType == "local" || tagType == "var") {
        if (m_localVariableModel->getVarByName(tagName) != nullptr)
          m_localVariableModel->getVarByName(tagName)->setDescription(tagText);
      } else if (tagType == "refGlobal") {
        if (m_application && m_application->globalVariableModel()->getVarByName(
                                 tagName) != nullptr)
          m_application->globalVariableModel()
              ->getVarByName(tagName)
              ->setDescription(tagText);
      } else if (tagType == "global") {
        // qDebug() << "global var: " << tagName << ":" << tagText;
        emit globalVariableDocumentationFound(tagName, tagText);
      } else if (tagType == "brief") {
        setDescription(tagText);
      } else if (tagType == "doc") {
        setDetailedDocumentation(tagText);
      } else if (tagType == "module") {
        // qDebug() << "module doc: " << tagName << ":" << tagText;
        emit moduleDocumentationFound(tagText);
      } else if (tagType == "briefmodule") {
        // qDebug() << " brief module doc: " << tagName << ":" << tagText;
        emit moduleBriefDocumentationFound(tagText);
      } else if (tagType == "mainpage") {
        // qDebug() << "main page doc: " << tagName << ":" << tagText;
        emit mainPageDocumentationFound(tagText);
      } else if (tagType == "export") {
        tagText = tagText.simplified();
        // qDebug() << "export directive: " << tagName << ":" << tagText;
        if (tagText.isEmpty())
          tagText = name();
        m_tags.insertMulti(tagType, tagName + " " + tagText);
        emit exportDirectiveFound(tagName, tagText);
      } else if (tagType == "copyright") {
        setCopyrightMessage(tagText);
      } else if (tagType == "linter") {
        setLinterDirective(tagName, tagText);
      } else {
        // unknown tag
        if (tagText.isEmpty())
          tagText = tagName;
        m_tags.insertMulti(tagType, tagText);
        emit unknownTagFound(tagType, tagName, tagText);
        // unknown tag type
        // m_description += QString("\n\\%1
        // %2\n%3").arg(tagType).arg(tagName).arg(tagText);
      }
    }
    // a new tag
    if (line.startsWith("//!") || line.startsWith("//\\")) {

      tagText = "";
      tagName = "";
      // set tag type
      tagType = line.remove("//!").remove("//\\").simplified();
      tagType = tagType.left(tagType.indexOf(" ")).simplified();
      // set tag name
      QRegExp rx;
      rx.setPattern("^\\s*" + tagType);
      tagText = line.remove(rx).trimmed();
      // match the tag name (optionally with [..] array indicator)
      rx.setPattern(R"(^[\w\\]*(|(\[\d*\]))(\s|$))");
      int pos = rx.indexIn(tagText);
      if (pos != -1)
        tagName = rx.cap().simplified();
      else
        tagName = "";
      // set tag text
      tagText.remove(rx);
      rx.setPattern("^:\\s*");
      tagText.remove(rx);
      // qDebug() << "new tag:" << tagType << ":" << tagName << ":" << tagText;
    } else {
      // ignore lines containing additional hint message marker
      if (!line.startsWith("//[="))
        // Read tag text from code lines and prepend a \n at end of each read
        // line. However, do not prepend a \n if it is the first line.
        tagText += (tagText.isEmpty() ? "" : "\n") +
                   line.remove(0, 2); //"\n" + line.remove(0, 2);
      // remove double \n at end of tag text
      while (tagText.right(2) == "\n\n")
        tagText.chop(1);
    }
  }
  // process a complete tag if there is one available at the end
  if (!tagType.isEmpty()) {
    // qDebug() << "process remaining tag:" << tagType << ":" << tagName << ":"
    //         << tagText;
    if (tagType == "param") {
      if (m_parameterModel->getVarByName(tagName) != nullptr)
        m_parameterModel->getVarByName(tagName)->setDescription(tagText);
    } else if (tagType == "local" || tagType == "var") {
      if (m_localVariableModel->getVarByName(tagName) != nullptr)
        m_localVariableModel->getVarByName(tagName)->setDescription(tagText);
    } else if (tagType == "global") {
      // qDebug() << "global var: " << tagName << ":" << tagText;
      emit globalVariableDocumentationFound(tagName, tagText);
    } else if (tagType == "refGlobal") {
      if (m_application && m_application->globalVariableModel()->getVarByName(
                               tagName) != nullptr)
        m_application->globalVariableModel()
            ->getVarByName(tagName)
            ->setDescription(tagText);
    } else if (tagType == "brief") {
      setDescription(tagText);
    } else if (tagType == "doc") {
      setDetailedDocumentation(tagText);
    } else if (tagType == "briefmodule") {
      // qDebug() << " brief module doc: " << tagName << ":" << tagText;
      emit moduleBriefDocumentationFound(tagText);
    } else if (tagType == "module") {
      // qDebug() << "module doc: " << tagName << ":" << tagText;
      emit moduleDocumentationFound(tagText);
    } else if (tagType == "mainpage") {
      // qDebug() << "mainpage doc: " << tagName << ":" << tagText;
      emit mainPageDocumentationFound(tagText);
    } else if (tagType == "export") {
      tagText = tagText.simplified();
      if (tagText.isEmpty())
        tagText = name();
      m_tags.insertMulti(tagType, tagName + " " + tagText);
      emit exportDirectiveFound(tagName, tagText);
    } else if (tagType == "copyright") {
      setCopyrightMessage(tagText);
    } else if (tagType == "linter") {
      setLinterDirective(tagName, tagText);
    } else {
      if (tagText.isEmpty())
        tagText = tagName;
      m_tags.insertMulti(tagType, tagText);
      emit unknownTagFound(tagType, tagName, tagText);
      // m_description += QString("\n\\%1
      // %2\n%3").arg(tagType).arg(tagName).arg(tagText);
    }
  }
  // qDebug() << documentationList;
  m_headerLines = headerLines;
}

QString cs8Program::briefDescription(bool trimmed) const {
  return trimmed ? m_briefDescription.trimmed() : m_briefDescription;
}

void cs8Program::setCellPath(const QString &path) {
  // qDebug() << __FUNCTION__ << path;
  m_cellPath = path;
}

QString cs8Program::cellFilePath() const {
  QString pth = QDir::fromNativeSeparators(m_filePath);
  // qDebug() << __FUNCTION__ << pth << m_cellPath;
  pth = pth.replace(m_cellPath + "usr/usrapp/", "Disk://");
  return pth;
}

QString cs8Program::documentation(bool withPrefix, bool forCOutput) const {
  // qDebug() << "documentation: " << name();
  QString out;
  QString prefix = withPrefix ? "/// " : "";
  /*
  QStringList list = m_detailedDocumentation.split( "\\n" );
  list.prepend( "\n" );
  list.prepend(description() );
  list.prepend( "//\\brief" );
  */
  QStringList list;
  list << "\n"
       << "\\brief" << m_briefDescription.split("\n") << "\n"
       << m_detailedDocumentation.split("\n");
  // list.removeLast();
  bool inCodeSection = false;
  int indentation = 0;
  foreach (QString str, list) {
    if (str.contains("<pre>")) {
      inCodeSection = true;
      out += prefix + "\n";
    }
    if (str.contains("</pre>")) {
      inCodeSection = false;
      out += prefix + "\n";
    }
    if (inCodeSection) {
      // qDebug() << str << "indent: " << indentation;
      if (str.simplified().indexOf(
              QRegExp("begin|if |for | while |do |switch| case")) == 0) {
        str = QString().fill(QChar(' '), indentation * 4) + str.trimmed();
        indentation++;
      }
      if (str.simplified().indexOf(QRegExp("end|until |break")) == 0) {
        indentation--;
        indentation = qMax(0, indentation);
        str = QString().fill(QChar(' '), indentation * 4) + str.trimmed();
      }
    }
    out += prefix + str + (inCodeSection ? "" : "") + "\n";
  }
  // out += "\n";
  // out =list.join("\n");
  foreach (cs8Variable *parameter, m_parameterModel->variableList()) {
    out += parameter->documentation(withPrefix, forCOutput);
  }

  return out.trimmed();
}

QString cs8Program::formattedDescriptionHeader() const {
  QString txt = m_briefDescription.trimmed();
  while (txt.endsWith("\n"))
    txt.chop(1);
  // retrieve documentation of parameters
  for (int i = 0; i <= m_parameterModel->variableList().count() - 1; i++) {
    if (i == 0)
      txt += "\n";
    if (m_parameterModel->variableList()
            .at(i)
            ->documentation(false, false)
            .length() > 2)
      txt += m_parameterModel->variableList().at(i)->name().leftJustified(18,
                                                                          ' ') +
             ": " + m_parameterModel->variableList().at(i)->description();
  }
  QRegularExpression rx("(\\s|^)((\\w*)\\\\_)");
  while (rx.match(txt).hasMatch()) {
    txt.replace(rx.match(txt).captured(2), rx.match(txt).captured(3) + "_");
  }

  return txt.trimmed();
}

void cs8Program::setDescription(const QString &theValue) {
  m_briefDescription = theValue;
  emit modified();
}

void cs8Program::setDetailedDocumentation(const QString &doc) {
  m_detailedDocumentation = doc;
  emit modified();
}

QString cs8Program::detailedDocumentation() const {
  return m_detailedDocumentation;
}

void cs8Program::writeXMLStream(QXmlStreamWriter &stream, bool withCode) {
  stream.writeStartElement("Program");
  stream.writeAttribute("name", name());
  if (isPublic())
    stream.writeAttribute("access", "public");
  //
  if (!m_briefDescription.isEmpty()) {
    stream.writeStartElement("Description");
    stream.writeCDATA(formattedDescriptionHeader());
    stream.writeEndElement();
  }
  //
  parameterModel()->writeXMLStream(stream);
  //
  localVariableModel()->writeXMLStream(stream);
  //
  stream.writeStartElement("Code");
  QString codeText;
  if (withCode) {
    codeText = toDocumentedCode();
  } else {
    if (name() != "stop") {
      codeText = "begin\n"
                 "logMsg(libPath())\n"
                 "put(sqrt(-1))\n"
                 "end";
    } else {
      codeText = "begin\n"
                 "end";
    }
  }
  stream.writeCDATA(codeText);
  stream.writeEndElement();
  stream.writeEndElement();
}

bool cs8Program::save(const QString &filePath, bool withCode) {
  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);

  QXmlStreamWriter stream(&buffer);
  stream.setAutoFormatting(true);
  stream.setAutoFormattingIndent(2);
  stream.setCodec("utf-8");
  stream.writeStartDocument();

  //
  stream.writeStartElement("Programs");
  stream.writeAttribute("xmlns:xsi",
                        "http://www.w3.org/2001/XMLSchema-instance");
  stream.writeAttribute("xmlns",
                        "http://www.staubli.com/robotics/VAL3/Program/2");
  writeXMLStream(stream, withCode);
  stream.writeEndDocument();

  // match our XML output to XML output of SRS
  buffer.buffer().replace("encoding=\"UTF-8", "encoding=\"utf-8");
  if (buffer.buffer().right(1) == "\n")
    buffer.buffer().chop(1);
  buffer.buffer().replace("\n", "\r\n");
  buffer.buffer().replace("\"/>", "\" />");
  //

  QFileInfo i(filePath);
  QString fileName_ = i.isDir() ? filePath + "/" + fileName() : filePath;
  // QString fileName_ = filePath + fileName();
  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly))
    return false;

  if (m_hasByteOrderMark) {
    buffer.buffer().insert(0, static_cast<char>(0xBF));
    buffer.buffer().insert(0, static_cast<char>(0xBB));
    buffer.buffer().insert(0, static_cast<char>(0xEF));
  }
  file.write(buffer.buffer());
  // qDebug() << "write file: " << fileName_;
  // qDebug() << "start of buffer: " << buffer.buffer().at(0)
  //         << buffer.buffer().at(1) << buffer.buffer().at(2);
  return true;
}

cs8ParameterModel *cs8Program::parameterModel() const {
  return m_parameterModel;
}

cs8VariableModel *cs8Program::localVariableModel() const {
  return m_localVariableModel;
}

QList<cs8Variable *> cs8Program::referencedGlobalVariables() const {
  return m_application->globalVariableModel()->findVariablesReferencedByProgram(
      name());
}

void cs8Program::setPublic(bool value) { m_public = value; }

bool cs8Program::isPublic() const { return m_public; }

QString cs8Program::name() const {
  // qDebug() << m_programSection.attribute("name");
  return m_name;
}

QString cs8Program::fileName() const { return name() + ".pgx"; }

void cs8Program::setName(const QString &name) { m_name = name; }

QString cs8Program::toDocumentedCode() {
  // QString documentation;
  QString detailedDocumentation;
  QStringList list;
  int startIfRow = -1;
  int endIfRow = -1;

  if (!briefDescription(true).isEmpty()) {
    list << "\\brief";
    list << briefDescription(true).split("\n");
  }

  if (!m_additionalHintMessage.isEmpty()) {
    list << "\n";
    list << QString("[= %1 =]").arg(m_additionalHintMessage);
    list << "\n ";
  }

  if (!m_detailedDocumentation.isEmpty()) {
    detailedDocumentation += "\\doc\n";
    detailedDocumentation += m_detailedDocumentation + "\n";
  }

  detailedDocumentation += m_parameterModel->toDocumentedCode();
  detailedDocumentation += m_localVariableModel->toDocumentedCode();
  // if (m_application)
  // detailedDocumentation +=
  // m_application->globalVariableModel()->toDocumentedCode();

  if (!m_briefModuleDocumentation.isEmpty()) {
    detailedDocumentation += "\\briefmodule\n";
    detailedDocumentation += m_briefModuleDocumentation + "\n";
  }

  if (!m_applicationDocumentation.isEmpty()) {
    detailedDocumentation += "\\module\n";
    detailedDocumentation += m_applicationDocumentation + "\n";
  }

  if (!m_mainPageDocumentation.isEmpty()) {
    detailedDocumentation += "\\mainpage\n";
    detailedDocumentation += m_mainPageDocumentation + "\n";
  }

  if (!m_tags.isEmpty()) {
    detailedDocumentation += "\n";
    for (auto &key : m_tags.uniqueKeys()) {
      for (auto &value : m_tags.values(key)) {
        detailedDocumentation += QString("\\%1 %2\n").arg(key, value);
      }
    }
  }

  if (!m_copyRightMessage.isEmpty()) {
    detailedDocumentation += "\n\\copyright\n";
    detailedDocumentation += m_copyRightMessage + "\n";
  }

  if (!detailedDocumentation.isEmpty()) {
    if (m_withIfBlock) {
      startIfRow = list.count();
      list << "  if false";
    }
    list << detailedDocumentation.split("\n");
    if (m_withIfBlock) {
      endIfRow = list.count();
      list << "  endIf";
    } else
      list << "_";
  } else {
    list << "_";
  }
  // if (!documentation.endsWith("\n"))
  //  documentation += "\n";

  // QStringList list = documentation.split("\n");
  while (list.last().isEmpty())
    list.removeLast();
  // prepend '//' markers
  // list.replaceInStrings(QRegExp("^(?!(\\s*(if false|endIf)))"), "  //");
  for (int i = 0; i < list.count(); i++) {
    if (i != startIfRow && i != endIfRow)
      list[i] = QString("  //%1").arg(rtrim(list[i]));
  }

  // indent lines in if block
  bool inIfBlock = false;
  for (int i = 0; i < list.count(); i++) {
    if (list[i].trimmed().startsWith("if false"))
      inIfBlock = true;
    else if (list[i].trimmed().startsWith("endIf"))
      inIfBlock = false;
    else if (inIfBlock)
      list[i] = "  " + list[i];
  }
  QString documentation = list.join("\n") + "\n";

  QString code = val3Code(false);

  QRegExp rx;
  rx.setPattern(R"(\s*begin\s*\n)");
  rx.setMinimal(true);
  int start = rx.indexIn(code, 0);
  start += rx.matchedLength();

  code.insert(start++, documentation);

  tidyUpCode(code);
  return code;
}

QString cs8Program::definition() const {
  return name() + "(" + m_parameterModel->toString() + ")";
}

void cs8Program::setCopyrightMessage(const QString &text) {
  m_copyRightMessage = text.trimmed();
  emit modified();
}

QString cs8Program::copyrightMessage() const { return m_copyRightMessage; }
