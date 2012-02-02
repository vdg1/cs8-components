#include <QApplication>
#include <QDialog>

#include "passwordimpl.h"
#include "dialogimpl.h"
//
int main ( int argc, char ** argv )
{
  QApplication app ( argc, argv );
  
  PasswordImpl pwd;
  if ( pwd.exec() ==QDialog::Accepted ) {
    DialogImpl win;
    win.show();
    app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );
    return app.exec();
  }
  app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );
  
  return 1;
}
