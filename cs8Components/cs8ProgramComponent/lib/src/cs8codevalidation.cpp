#include "cs8codevalidation.h"
#include "cs8variable.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>

cs8CodeValidation::cs8CodeValidation(QObject *parent) : QObject(parent) {}

bool cs8CodeValidation::loadRuleFile(const QString &fileName) {

  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  QString errorMessage;
  int errorColumn, errorLine;
  if (!rules.setContent(&file, &errorMessage, &errorLine, &errorColumn)) {
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

  globalDataRules = rules.documentElement()
                        .elementsByTagName("globalData")
                        .at(0)
                        .childNodes();
  parameterRules =
      rules.documentElement().elementsByTagName("parameter").at(0).childNodes();
  localDataRules =
      rules.documentElement().elementsByTagName("localData").at(0).childNodes();
  programRules =
      rules.documentElement().elementsByTagName("program").at(0).childNodes();
  return true;
}

QStringList cs8CodeValidation::runDataValidationRule(
    const cs8Application *app, cs8Program *program,
    QList<cs8Variable *> *variableList, QDomNodeList ruleList) {
  QStringList validationMessages;
  for (int i = 0; i < ruleList.count(); i++) {
    QDomElement ruleNode = ruleList.at(i).toElement();
    QString expressionString = ruleNode.attribute("expression");

    QRegExp rx(expressionString);
    QString checkProperty = ruleNode.attribute("checkProperty");
    QString message =
        ruleNode.elementsByTagName("message").at(0).toElement().text();
    int minNameLength = ruleNode.attribute("minLength", "-1").toInt();

    if (ruleNode.nodeName() == "variable") {
      QString varType = ruleNode.attribute("type");
      qDebug() << "Check rule " << varType;

      //
      if (variableList != nullptr)
        foreach (cs8Variable *var, *variableList) {
          qDebug() << "apply rule to var:" << var->name() << ":" << var->type();

          if (varType == "%USER%")
            if (!var->isBuildInType()) {
              varType = var->type();
              QRegExp r(R"(%(\w*)\((\d*)\)%)");
              int pos = r.indexIn(expressionString);
              int matchedLength = r.matchedLength();
              if (pos != -1 && r.captureCount() == 2) {
                if (r.cap(1) == "PREFIX") {
                  qDebug() << r.pattern();
                  expressionString.replace(pos, matchedLength,
                                           var->type().left(r.cap(2).toInt()));
                  rx.setPattern(expressionString);
                }
              }
            }
          if (checkProperty == "name" &&
              (minNameLength == -1 || var->name().length() >= minNameLength) &&
              (varType == var->type() || varType.isEmpty())) {
            // apply name rule on variable name
            if (rx.indexIn(var->name()) == -1 || rx.pattern().isEmpty()) {
              QString msg = message;
              msg.replace("%varType%", var->type());
              msg.replace("%varName%", var->name());
              msg.replace("%lineNumber%", var->name());

              if (program == nullptr) {
                msg.replace("%fileName%", app->cellDataFilePath(true));
              } else {
                msg.replace("%progName%", program->name());
                msg.replace("%fileName%", program->cellFilePath());
              }
              validationMessages << msg;
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
          // apply name rule on variable name
          if (rx.indexIn(var->name()) != -1 || rx.pattern().isEmpty()) {
            if (checkProperty == "notReferenced") {
              bool referenced =
                  program == nullptr
                      ? app->getReferencedMap()[var->name()] == true
                      : program->referencedVariables().contains(var->name());
              if (((var->isPublic() && (variableScope < 2)) ||
                   (!var->isPublic() && variableScope == 1)) &&
                  !referenced) {
                // apply name rule on variable name
                if (rx.indexIn(var->name()) == -1 || rx.pattern().isEmpty()) {
                  QString msg = message;
                  msg.replace("%varType%", var->type());
                  msg.replace("%varName%", var->name());
                  if (program == nullptr) {
                    msg.replace("%fileName%", app->cellDataFilePath(true));
                  } else {
                    msg.replace("%progName%", program->name());
                    msg.replace("%fileName%", program->cellFilePath());
                  }
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

                  msg.replace("%progName%", prog->name());
                  msg.replace("%fileName%", prog->cellFilePath());
                  validationMessages << msg;
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
            qDebug() << "check program  " << program->name()
                     << "referenced: " << referenced
                     << "public: " << program->isPublic();
            if (((program->isPublic() && (variableScope < 2)) ||
                 (!program->isPublic() && variableScope == 2)) &&
                !referenced) {
              // apply name rule on variable name
              // if (rx.indexIn(program->name())==-1 || rx.pattern().isEmpty())
              {
                QString msg = message;
                msg.replace("%progName%", program->name());
                msg.replace("%fileName%", program->cellFilePath());

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

          validationMessages << msg;
        }
      }
    }
  }

  return validationMessages;
}

QStringList cs8CodeValidation::runValidation(const cs8Application *app) {
  QStringList validationMessages;
  // run global data rules

  validationMessages << runDataValidationRule(
      app, 0, &app->globalVariableModel()->variableList(), globalDataRules);
  foreach (cs8Program *program, app->programModel()->programList()) {

    validationMessages << runDataValidationRule(
        app, program, &program->parameterModel()->variableList(),
        parameterRules);
    validationMessages << runDataValidationRule(
        app, program, &program->localVariableModel()->variableList(),
        localDataRules);
    validationMessages << runDataValidationRule(app, program, 0, programRules);

    // check TODOs in code
    QMapIterator<int, QString> i(program->todos());
    while (i.hasNext()) {
      i.next();
      validationMessages << QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<"
                                    "line>%4<msg>%2<file>%3")
                                .arg(program->name())
                                .arg("Warning: " + i.value())
                                .arg(program->cellFilePath())
                                .arg(i.key());
    }
  }

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
