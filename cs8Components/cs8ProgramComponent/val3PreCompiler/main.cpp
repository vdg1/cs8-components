
#include "cs8linter.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QProcess>
#include <cs8application.h>
#include <cs8codevalidation.h>

//"C:\Program Files (x86)\Staubli\CS8\s8.12cs9_BS2378\VAL3Check.exe"
//-R"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812"
//-E"<level>%e<line>%l<msg>%m<file>%f" -V"C:\Program Files
//(x86)\Staubli\CS8\s8.12cs9_BS2378" -KA -LE -s"C:\Program Files
//(x86)\Staubli\CS8\s8.12cs9_BS2378" -I-
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation"
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\dispatcher"
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\i18n"
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\coreModules\modRobotCycle"
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\coreModules\modRobotCycle\tempApplication"
//"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\aGripperTest"

// "C:\Program Files\Staubli\CS8\s7.2\VAL3Check.exe"
// -R"D:\data\Staubli\CS8\Development_7.2" -E"<level>%e<line>%l<msg>%m<file>%f"
// -V"C:\Program Files\Staubli\CS8\s7.2" -KS -LE -s"C:\Program
// Files\Staubli\CS8\s7.2" -I+
// "D:\data\Staubli\CS8\Development_7.2\usr\usrapp\Plastic\coreModules\modUserDisp"
// "D:\data\Staubli\CS8\Development_7.2\usr\usrapp\dispatcher"
// <level>Error<CLASS>PRG<P1>execLow<P2>CODE<line>10<msg>? : unexpected trailing
// characters<file>Disk://dispatcher/execLow.pgx
int main(int argc, char *argv[]) {

  QString VAL3CHECKORIG = "VAL3Check_Orig.exe";

  QCoreApplication a(argc, argv);
  QCoreApplication::setOrganizationName("Saxe Group");
  QCoreApplication::setOrganizationDomain("saxe-group.com");
  QCoreApplication::setApplicationName("Val3 Linter");

  QStringList args = qApp->arguments();

  /*
  args
      = QStringList()
        << R"("C:\Program Files (x86)\Staubli\CS8\s8.12.2cs9_BS2561\VAL3Check.exe)"
        << R"(-R"D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812)"
        << R"(-E"<level>%e<line>%l<msg>%m<file>%f" -V"C:\Program Files (x86)\Staubli\CS8\s8.12.2cs9_BS2561)"
        << R"(-KA)"
        << R"(-LE)"
        << R"(-s"C:\Program Files (x86)\Staubli\CS8\s8.12.2cs9_BS2561)"
        << R"(-I+)"
        << R"(D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\coreModules\modUserDisp)"
        << R"(D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\coreModules\modStatusModel)"
        << R"(D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\io_User)"
        << R"(D:\data\Staubli\SRS\_Development\Dev_SAXEAutomation_CS9\Controller_s812\usr\usrapp\SAXEAutomation\coreModules\modSignalSlot)";

*/
  qDebug() << "Linter started with arguments " << args;
  // remove 1st argument, that points to the executable itself
  args.removeAt(0);

  cs8Linter linter(args, VAL3CHECKORIG, true);

  a.exec();
}
