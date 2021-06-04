#include "../lib/src/cs8program.h"
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  cs8Program prog(nullptr);
  // prog.open(
  // R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation/Modules/modFrontPanel/ioFrontPanel/init.pgx)");

  // prog.save(
  // R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation\Modules/modFrontPanel/ioFrontPanel/)");

  prog.open(
      R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation/coreModules/modSettings/updateView.pgx)");

  prog.save(
      R"(D:\data\Cpp\_SAXESwissSystem\cs8i18n\SAXEAutomation\coreModules/modSettings/)");
  return 0;
}
