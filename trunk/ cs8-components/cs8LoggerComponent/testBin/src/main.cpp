#include <QApplication>
#include <QPlastiqueStyle>
#include "dialogimpl.h"
//
int main(int argc, char ** argv)
{
	//QApplication::setStyle(new QPlastiqueStyle);
	QApplication app( argc, argv );
	DialogImpl win;
	win.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
