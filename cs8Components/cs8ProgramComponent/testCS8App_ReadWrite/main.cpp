#include "cs8ProjectComponent.h"
#include "cs8application.h"
#include <QCoreApplication>
#include <QDomDocument>
#include <QTableView>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  cs8Application app;
  app.open(R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation\coreModules\modScreen\zScreen\zScreen.pjx)");
  app.save(R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation\coreModules\modScreen\zScreen\)");

  // app.open("D:\\data\\Staubli\\SRS\\_Development\\Dev_SAXEAutomation_"
  //         "CS9\\Controller_s882\\usr\\usrapp\\aMotionID\\aMotionID.pjx");
  // app.save("D:\\data\\Staubli\\SRS\\_Development\\Dev_SAXEAutomation_"
  //         "CS9\\Controller_s882\\usr\\usrapp\\aMotionID_saved\\",
  //         "aMotionID_saved");
}
