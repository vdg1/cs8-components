#include "wagoconfig.h"

#include <QtGui>
#include <QApplication>
#include "cs8ControllerComponent.h"

int main(int argc, char *argv[])
{
	// register cs8 file engine to QT
	cs8FileEngineHandler engine;	

    QApplication a(argc, argv);
    WagoConfig w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
