#include "passwordimpl.h"
//
PasswordImpl::PasswordImpl ( QWidget * parent, Qt::WFlags f )
    : QDialog ( parent, f )
{
  setupUi ( this );
  connect ( btOk, SIGNAL ( clicked() ), this, SLOT ( accepted() ) );
  connect ( btCancel, SIGNAL ( clicked() ), this, SLOT ( rejected() ) );
}
//

void PasswordImpl::accepted()
{
  if ( password->text() =="Pa550rd" )
    accept();
  else
    reject();
}

void PasswordImpl::rejected()
{
  reject();
}
