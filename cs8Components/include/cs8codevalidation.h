#ifndef CS8CODEVALIDATION_H
#define CS8CODEVALIDATION_H

#include "cs8application.h"
#include "qlist.h"

#include <QObject>

class cs8CodeValidation : public QObject {
  Q_OBJECT
public:
  explicit cs8CodeValidation(QObject *parent = 0);
  bool loadRuleFile(const QString &fileName);
  cs8ValidationResult runValidation(const cs8Application *app, int reportingLevel);
  cs8ValidationResult runDataValidationRule(const cs8Application *app,
                                            cs8Program *program,
                                            QList<cs8Variable *> *variableList,
                                            QDomNodeList ruleList,
                                            int nodeSeverity);
  void addValidationResult(QString msg, int nodeSeverity,
                           cs8ValidationResult &validationMessages);

signals:
  void validationMessage(const QString &message);

public slots:

protected:
  cs8ValidationResult checkObsoleteProgramFiles(const cs8Application *app);

private:
  QDomDocument m_validationRules;
  QDomNodeList m_GlobalDataRules, m_ParameterRules, m_LocalDataRules,
      m_ProgramRules;
  int m_GlobalDataRulesSeverity, m_ParameterRulesSeverity,
      m_LocalDataRulesSeverity, m_ProgramRulesSeverity;
  int m_reportingLevel;
};

#endif // CS8CODEVALIDATION_H
