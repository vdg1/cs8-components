#include "cs8programcodeview.h"
#include "cs8highlighter.h"
//
cs8ProgramCodeView::cs8ProgramCodeView ( QWidget * parent )
    : QTextEdit()
{
  m_highlighter = new cs8Highlighter ( this->document() );
}
//

void cs8ProgramCodeView::setMasterView ( QAbstractItemView* value )
{
  m_masterView = value;
  connect ( m_masterView->selectionModel(),SIGNAL ( selectionChanged ( const QItemSelection &, const QItemSelection & ) ),
            this,SLOT ( slotSelectionChanged ( const QItemSelection & , const QItemSelection & ) ) );
            
}

void cs8ProgramCodeView::slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
  if ( deselected.count() >0 ) {
  }
  if ( selected.count() >0 ) {
    setText ( m_masterView->model()->data ( selected.indexes().at ( 0 ),Qt::UserRole ).toString() );
  }
}
