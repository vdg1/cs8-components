
#include <QProcess>
#include <QCoreApplication>
#include <cs8application.h>
#include <iostream>
#include <cs8codevalidation.h>

using namespace std;


// "C:\Program Files\Staubli\CS8\s7.2\VAL3Check.exe"  -R"D:\data\Staubli\CS8\Development_7.2" -E"<level>%e<line>%l<msg>%m<file>%f" -V"C:\Program Files\Staubli\CS8\s7.2" -KS -LE -s"C:\Program Files\Staubli\CS8\s7.2" -I+ "D:\data\Staubli\CS8\Development_7.2\usr\usrapp\Plastic\coreModules\modUserDisp" "D:\data\Staubli\CS8\Development_7.2\usr\usrapp\dispatcher"
// <level>Error<CLASS>PRG<P1>execLow<P2>CODE<line>10<msg>? : unexpected trailing characters<file>Disk://dispatcher/execLow.pgx
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName ("Val3PreCompiler");
    QCoreApplication::setOrganizationName("SAXE Swiss System");

    if (qApp->arguments ().count ()>1)
    {
        //
        //cs8Application app;
        QString cellPath;
        foreach(QString arg,qApp->arguments ())
        {
            if (arg.startsWith ("-R"))
            {
                cellPath=arg.remove ("-R");
                cellPath=cellPath.remove ("\"");
            }

        }

        cs8CodeValidation validator;
        if (validator.loadRuleFile(":/rules/compilerRules.xml"))
        {
            for(int i=qApp->arguments ().count (); i--; i>1)
            {
                if (qApp->arguments ().at(i).startsWith ("-"))
                    break;
                else {
                    qDebug () << qApp->arguments ().at(i);
                    cs8Application app;
                    app.setCellPath(cellPath);
                    app.openFromPathName (qApp->arguments ().at(i));
                    //cout << qPrintable(app.performPrecompilerChecks ()+"\n");
                    cout << qPrintable(validator.runValidation(&app).join("\n")+"\n");
                }

            }
        }

        // run original Val3Check.exe
        QProcess proc;
        QStringList arg=qApp->arguments ();
        qDebug() << "Arguments " << arg;
        // remove the first argument as that is the path of the executable
        arg.removeAt (0);
        proc.start ("Val3Check_orig.exe",arg);
        proc.waitForFinished ();
        QString out=proc.readAll ();
        qDebug() << "Result " << out;
        cout << qPrintable(out);
        return proc.exitCode ();
    }
    else
    {
        /*
                MainWindow w;
                w.show();

                return a.exec();
                */
    }
}
