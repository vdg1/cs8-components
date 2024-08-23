#ifndef CS8VALIDATIONRESULT_H
#define CS8VALIDATIONRESULT_H

#include <QString>
#include <QStringList>

class cs8ValidationResult {
public:
  // constructor
  cs8ValidationResult();
  cs8ValidationResult(const cs8ValidationResult &result);
  cs8ValidationResult(QString msg, int nodeSeverity);
  QStringList messages;
  int totalInfos = 0;
  int totalWarnings = 0;
  int totalErrors = 0;

  // add << operator to make it easier to add messages
  cs8ValidationResult &operator<<(const cs8ValidationResult &result);
};

#endif // CS8VALIDATIONRESULT_H
