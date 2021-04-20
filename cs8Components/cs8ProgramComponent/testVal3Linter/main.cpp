#include "cs8application.h"
#include <QCoreApplication>
#include <cs8codevalidation.h>

// D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s882\usr\usrapp\SAXEAutomation
int main(int argc, char *argv[]) {
  cs8Application app;
  if (!app.openFromPathName(
          R"(D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s882\usr\usrapp\SAXEAutomation\modules\modIMM)")) {
    qDebug() << "Failed to open application";
  }

  cs8CodeValidation validator;
  if (validator.loadRuleFile(":/compilerRules.xml")) {
    QStringList msg = validator.runValidation(&app);
    for (const auto &m : msg)
      qDebug() << m;
  } else {
    qDebug() << "Failed to load validation rules";
  }
  return 0;
}