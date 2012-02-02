#include <QApplication>
#include "mainwindowimpl.h"
#include "cs8ControllerComponent.h"
//
int main ( int argc, char ** argv )
{

  // register cs8 file engine to QT
  cs8FileEngineHandler engine;

  QApplication app ( argc, argv );
  MainWindowImpl win;
  win.show();
  app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );
  return app.exec();
}

