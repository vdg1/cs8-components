#include "mainwindow.h"
#include <QApplication>
#include <cs8application.h>

int main(int argc, char *argv[]) {

  // cs8Application app;
  // app.open("D:/data/Staubli/SRS/_Development/DevelopmentLegacyVersions/Development_72/usr/usrapp/aPhilalock/"
  //         "aPhilalock.pjx");
  // app.save("D:/data/Staubli/SRS/_Development/DevelopmentLegacyVersions/Development_72/usr/usrapp/aPhilalock_/",
  //         "aPhilalock_");
  // return 0;

  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
