#include <QApplication>
#include "dialogimpl.h"
#include "cs8ControllerComponent.h"
//
int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	// register cs8 file engine to QT
	cs8FileEngineHandler engine;
	//
	DialogImpl win;
	win.show();
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
