#include "cs8validationresult.h"

cs8ValidationResult::cs8ValidationResult()
    : totalInfos(0), totalWarnings(0), totalErrors(0) {}

cs8ValidationResult::cs8ValidationResult(const cs8ValidationResult &result)
    : messages(result.messages), totalInfos(result.totalInfos),
      totalWarnings(result.totalWarnings), totalErrors(result.totalErrors) {}

cs8ValidationResult::cs8ValidationResult(QString msg, int nodeSeverity)
    : totalInfos(0), totalWarnings(0), totalErrors(0) {
  messages << msg;
  if (nodeSeverity == 0)
    totalInfos++;
  else if (nodeSeverity == 1)
    totalWarnings++;
  else
    totalErrors++;
}

cs8ValidationResult &cs8ValidationResult::operator<<(const cs8ValidationResult &result) {
  messages << result.messages;
  totalInfos += result.totalInfos;
  totalWarnings += result.totalWarnings;
  totalErrors += result.totalErrors;
  return *this;
}
