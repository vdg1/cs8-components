#include "cs8application.h"
#include <QCoreApplication>
#include <cs8codevalidation.h>

// D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s882\usr\usrapp\SAXEAutomation
int main(int argc, char *argv[])
{
    cs8Application app;
    app.setCellPath(R"(D:\data\Staubli\SRS\_Customers\NovoNordisk\32971-sas-scaledrum-by-flex\Controller1\usr\usrapp)");
    if (!app.openFromPathName(R"(D:\data\Staubli\SRS\_Customers\NovoNordisk\32971-sas-scaledrum-by-flex\Controller1\usr\usrapp\aScaleDrum)")) {
        qDebug() << "Failed to open application";
    }
    qDebug() << app.projectFileList();
    // auto prog = app.programModel()->getProgramByName("SP_PlaceBox");
    // auto var = prog->localVariableModel()->getVarByName("lbFailed");
    // if (var)
    //  var->setName("newLbFailed", &app);

    // auto var = app.globalVariableModel()->getVarByName("nStatePart");
    // if (var)
    //  var->setName("nNewStatePart", &app);

    //auto prog = app.programModel()->getProgramByName("_aprStation");
    //qDebug() << prog->symbolReferences();
    //if (prog)
    //  prog->setName("newApr", &app);

    //app.save(false);
    /*
  cs8CodeValidation validator;
  if (validator.loadRuleFile(":/compilerRules.xml")) {
    QStringList msg = validator.runValidation(&app, 0);
    for (const auto &m : qAsConst(msg))
      qDebug() << m;
  } else {
    qDebug() << "Failed to load validation rules";
  }
*/
    return 0;
}
