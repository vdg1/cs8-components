#include "cs8codevalidation.h"
#include "cs8variable.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>

cs8CodeValidation::cs8CodeValidation(QObject *parent)
    : QObject(parent), m_reportingLevel(0) {}

bool cs8CodeValidation::loadRuleFile(const QString &fileName) {

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  QString errorMessage;
  int errorColumn, errorLine;
  if (!m_validationRules.setContent(&file, &errorMessage, &errorLine,
                                    &errorColumn)) {
    file.close();
    qDebug() << "Failed to load validation file: " << errorMessage << errorLine
             << errorColumn;
    // QString message =
    //    QString("The rule file %1 could not be loaded: %2, line %3, column
    //    %4")
    //        .arg(fileName)
    //        .arg(errorMessage)
    //        .arg(errorLine)
    //        .arg(errorColumn);
    return false;
  }
  file.close();

  m_GlobalDataRules = m_validationRules.documentElement()
                          .elementsByTagName("globalData")
                          .at(0)
                          .childNodes();
  m_GlobalDataRulesSeverity = m_validationRules.documentElement()
                                  .elementsByTagName("globalData")
                                  .at(0)
                                  .toElement()
                                  .attribute("severity", 0)
                                  .toInt();
  m_ParameterRules = m_validationRules.documentElement()
                         .elementsByTagName("parameter")
                         .at(0)
                         .childNodes();
  m_ParameterRulesSeverity = m_validationRules.documentElement()
                                 .elementsByTagName("parameter")
                                 .at(0)
                                 .toElement()
                                 .attribute("severity", 0)
                                 .toInt();
  m_LocalDataRules = m_validationRules.documentElement()
                         .elementsByTagName("localData")
                         .at(0)
                         .childNodes();
  m_LocalDataRulesSeverity = m_validationRules.documentElement()
                                 .elementsByTagName("localData")
                                 .at(0)
                                 .toElement()
                                 .attribute("severity", 0)
                                 .toInt();
  m_ProgramRules = m_validationRules.documentElement()
                       .elementsByTagName("program")
                       .at(0)
                       .childNodes();
  m_ProgramRulesSeverity = m_validationRules.documentElement()
                               .elementsByTagName("program")
                               .at(0)
                               .toElement()
                               .attribute("severity", 0)
                               .toInt();
  return true;
}

QStringList cs8CodeValidation::runDataValidationRule(
    const cs8Application *app, cs8Program *program,
    QList<cs8Variable *> *variableList, QDomNodeList ruleList,
    int nodeSeverity) {
  QStringList validationMessages;
  for (int i = 0; i < ruleList.count(); i++) {
    QDomElement ruleNode = ruleList.at(i).toElement();
    QString expressionString = ruleNode.attribute("expression");

    QRegExp rx(expressionString);
    QString checkProperty = ruleNode.attribute("checkProperty");
    QString message =
        ruleNode.elementsByTagName("message").at(0).toElement().text();
    int minNameLength = ruleNode.attribute("minLength", "-1").toInt();
    nodeSeverity =
        ruleNode.attribute("severity", QString("%1").arg(nodeSeverity)).toInt();
    QString level;

    switch (qMax(0, nodeSeverity + m_reportingLevel)) {
    case 0:
      level = "Message";
      break;
    case 1:
      level = "Warning";
      break;
    default:
      level = "Error";
      break;
    }
    if (ruleNode.nodeName() == "variable") {
      QString varType = ruleNode.attribute("type");
      // qDebug() << "Check rule " << varType;

      //
      if (variableList != nullptr)
        foreach (cs8Variable *var, *variableList) {
          // qDebug() << "apply rule to var:" << var->name() << ":" <<
          // var->type();
          if (!var->linterDirective().contains("ignore")) {

            if (varType == "%USER%")
              if (!var->isBuildInType()) {
                varType = var->type();
                QRegExp r(R"(%(\w*)\((\d*)\)%)");
                int pos = r.indexIn(expressionString);
                int matchedLength = r.matchedLength();
                if (pos != -1 && r.captureCount() == 2) {
                  if (r.cap(1) == "PREFIX") {
                    // qDebug() << r.pattern();
                    expressionString.replace(
                        pos, matchedLength, var->type().left(r.cap(2).toInt()));
                    rx.setPattern(expressionString);
                  }
                }
              }
            if (checkProperty == "name" &&
                (minNameLength == -1 || var->name().length() > minNameLength) &&
                (varType == var->type() || varType.isEmpty())) {
              // apply name rule on variable name
              if (rx.indexIn(var->name()) == -1 || rx.pattern().isEmpty()) {
                QString msg = message;
                msg.replace("%varType%", var->type());
                msg.replace("%varName%", var->name());
                msg.replace("%level%", level);
                msg.replace(
                    "%lineNumber%",
                    QString("%1").arg(var->symbolReferences().value(0).line));

                if (program == nullptr) {
                  msg.replace("%fileName%", app->cellDataFilePath(true));
                } else {
                  msg.replace("%progName%", program->name());
                  msg.replace("%fileName%", program->cellFilePath());
                }
                var->setLinterMessages(msg);
                validationMessages << msg;
              }
            }
          }
        }
    } else if (ruleNode.nodeName() == "enumeration") {
      // check if variable list is passed
      if (variableList != nullptr) {
        foreach (cs8Variable *var, *variableList) {
          if (!var->linterDirective().contains("ignore")) {
            if (checkProperty == "value") {
            }
          }
        }
      }
    } else if (ruleNode.nodeName() == "reference") {
      QString scope = ruleNode.attribute("scope", "");
      int variableScope = 0;
      if (scope == "public")
        variableScope = 1;
      if (scope == "private")
        variableScope = 2;

      // check if variable list is passed
      if (variableList != nullptr) {
        foreach (cs8Variable *var, *variableList) {
          if (!var->linterDirective().contains("ignore")) {
            // apply name rule on variable name
            if (rx.indexIn(var->name()) != -1 || rx.pattern().isEmpty()) {
              if (checkProperty == "notReferenced") {
                bool referenced = (var->symbolReferences().count() != 0);
                //: program->referencedVariables().contains(var->name());

                if (((var->isPublic() && variableScope == 1) ||
                     (!var->isPublic() && variableScope == 2) ||
                     variableScope == 0) &&
                    !referenced) {
                  // apply name rule on variable name
                  if (rx.indexIn(var->name()) == -1 || rx.pattern().isEmpty()) {
                    QString msg = message;
                    msg.replace("%varType%", var->type());
                    msg.replace("%varName%", var->name());
                    msg.replace("%lineNumber%", "1");
                    msg.replace("%level%", level);
                    if (program == nullptr) {
                      msg.replace("%fileName%", app->cellDataFilePath(true));
                    } else {
                      msg.replace("%progName%", program->name());
                      msg.replace("%fileName%", program->cellFilePath());
                    }
                    var->setLinterMessages(msg);
                    validationMessages << msg;
                  }
                }
              } else if (checkProperty == "hidden") {

                foreach (cs8Program *prog, app->programModel()->programList()) {
                  if (prog->localVariableModel()->variableNameList().contains(
                          var->name())) {

                    QString msg = message;
                    msg.replace("%progName%", prog->name());
                    msg.replace("%varName%", var->name());
                    msg.replace("%fileName%", prog->cellFilePath());
                    msg.replace("%level%", level);
                    msg.replace("%lineNumber%",
                                QString("%1").arg(
                                    var->symbolReferences().value(0).line));
                    var->setLinterMessages(msg);
                    validationMessages << msg;
                  }
                }
              }
            }
          }
        }
      } else
      // if no variable list is passed apply rules to program
      {
        // apply name rule on program name
        if (rx.indexIn(program->name()) == -1 || rx.pattern().isEmpty()) {
          if (checkProperty == "notReferenced") {
            bool referenced = app->getCallList(program).count() != 0;
            // qDebug() << "check program  " << program->name()
            //                     << "referenced: " << referenced
            //                     << "public: " << program->isPublic();
            if (((program->isPublic() && (variableScope < 2)) ||
                 (!program->isPublic() && variableScope == 2)) &&
                !referenced) {
              // apply name rule on variable name
              // if (rx.indexIn(program->name())==-1 ||
              // rx.pattern().isEmpty())
              {
                QString msg = message;
                msg.replace("%progName%", program->name());
                msg.replace("%fileName%", program->cellFilePath());
                msg.replace("%lineNumber%", "0");
                msg.replace("%level%", level);
                program->setLinterMessages(msg);
                validationMessages << msg;
              }
            }
          }
        }
      }
    } else if (ruleNode.nodeName() == "documentation") {
      if (checkProperty == "brief") {
        if (program->briefDescription(true).isEmpty()) {
          QString msg = message;
          msg.replace("%progName%", program->name());
          msg.replace("%fileName%", program->cellFilePath());
          msg.replace("%lineNumber%", "1");
          msg.replace("%level%", level);
          program->setLinterMessages(msg);
          validationMessages << msg;
        }
      }
    }
  }

  return validationMessages;
}

QStringList cs8CodeValidation::runValidation(const cs8Application *app,
                                             int reportingLevel) {
  QStringList validationMessages;
  m_reportingLevel = reportingLevel;
  // run global data rules

  validationMessages << runDataValidationRule(
      app, 0, &app->globalVariableModel()->variableListByType(),
      m_GlobalDataRules, m_GlobalDataRulesSeverity);

  foreach (cs8Program *program, app->programModel()->programList()) {

    validationMessages << runDataValidationRule(
        app, program, &program->parameterModel()->variableListByType(),
        m_ParameterRules, m_ParameterRulesSeverity);

    validationMessages << runDataValidationRule(
        app, program, &program->localVariableModel()->variableListByType(),
        m_LocalDataRules, m_LocalDataRulesSeverity);

    validationMessages << runDataValidationRule(app, program, 0, m_ProgramRules,
                                                m_ProgramRulesSeverity);

    // check TODOs in code
    QMapIterator<int, QString> i(program->todos());
    while (i.hasNext()) {
      i.next();
      validationMessages << QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<"
                                    "line>%4<msg>%2<file>%3")
                                .arg(program->name())
                                .arg(i.value())
                                .arg(program->cellFilePath())
                                .arg(i.key());
    }
  }

  // check values of ENUMS
  validationMessages << app->checkEnumerations();

  // check for obsolete PGX files
  // retrieve list of pgx files from file system

  QDir dir;
  dir.setPath(app->projectPath());
  QStringList pgxFileList = dir.entryList(QStringList() << "*.pgx");
  foreach (QString pgxFileName, pgxFileList) {
    bool found = false;
    foreach (cs8Program *program, app->programModel()->programList()) {
      if (program->fileName() == pgxFileName) {
        found = true;
        break;
      }
    }
    if (!found) {
      validationMessages << QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<"
                                    "line>%4<msg>%2<file>%3")
                                .arg(pgxFileName)
                                .arg("Warning: Program file is obsolete")
                                .arg(pgxFileName)
                                .arg("1");
    }
  }
  return validationMessages;
}
