#ifndef CS8CODEVALIDATION_H
#define CS8CODEVALIDATION_H

#include "cs8application.h"

#include <QObject>

class cs8CodeValidation : public QObject
{
    Q_OBJECT
public:
    explicit cs8CodeValidation(QObject *parent = 0);
    bool loadRuleFile(const QString & fileName);
    QStringList runValidation(cs8Application *app);
    QStringList runDataValidationRule(cs8Application *app, cs8Program *program, QList<cs8Variable *> variableList, QDomNodeList ruleList);
signals:
    void validationMessage(const QString & message);

public slots:

private:
    QDomDocument rules;
    QDomNodeList globalDataRules, parameterRules, localDataRules;
};

#endif // CS8CODEVALIDATION_H
