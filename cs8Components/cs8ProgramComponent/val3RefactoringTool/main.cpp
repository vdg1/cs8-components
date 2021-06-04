
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("Saxe Group");
  QCoreApplication::setOrganizationDomain("saxe-group.com");
  QCoreApplication::setApplicationName("Val3 Refactoring Tool");
  MainWindow w;
  w.show();
  return a.exec();
}
