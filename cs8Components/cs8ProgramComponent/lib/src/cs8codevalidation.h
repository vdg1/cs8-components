#ifndef CS8CODEVALIDATION_H
#define CS8CODEVALIDATION_H

#include "cs8application.h"

#include <QObject>

class cs8CodeValidation : public QObject {
  Q_OBJECT
public:
  explicit cs8CodeValidation(QObject *parent = 0);
  bool loadRuleFile(const QString &fileName);
  QStringList runValidation(const cs8Application *app, int reportingLevel);
  QStringList runDataValidationRule(const cs8Application *app,
                                    cs8Program *program,
                                    QList<cs8Variable *> *variableList,
                                    QDomNodeList ruleList, int nodeSeverity);
signals:
  void validationMessage(const QString &message);

public slots:

private:
  QDomDocument m_validationRules;
  QDomNodeList m_GlobalDataRules, m_ParameterRules, m_LocalDataRules,
      m_ProgramRules;
  int m_GlobalDataRulesSeverity, m_ParameterRulesSeverity,
      m_LocalDataRulesSeverity, m_ProgramRulesSeverity;
  int m_reportingLevel;
};

#endif // CS8CODEVALIDATION_H
