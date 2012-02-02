#include "cs8modbuswidget.h"
#include "cs8modbusdelegate.h"
#include "cs8modbusconfigfile.h"

#include <QTableView>
#include <QToolBar>
#include <QMessageBox>
//
cs8ModbusWidget::cs8ModbusWidget ( QWidget * parent )
    : QWidget ( parent ), m_toolBar ( 0 )
{
  setupUi ( this );
  tableView->setItemDelegate ( new cs8ModbusDelegate ( this ) );
  connect ( actionInsert_Item,SIGNAL ( triggered() ),this,SLOT ( slotInsertItem() ) );
  connect ( actionDelete_Item,SIGNAL ( triggered() ),this,SLOT ( slotDeleteItem() ) );
  connect ( actionAppend_Item,SIGNAL ( triggered() ),this,SLOT ( slotAppendItem() ) );
}
//

void cs8ModbusWidget::on_leName_editingFinished()
{
  ( ( cs8ModbusConfigFile* ) tableView->model() )->setTopicName ( leName->text() );
}

void cs8ModbusWidget::on_spPort_editingFinished()
{
  ( ( cs8ModbusConfigFile* ) tableView->model() )->setPort ( spPort->value() );
}

void cs8ModbusWidget::on_spConnections_editingFinished()
{
  ( ( cs8ModbusConfigFile* ) tableView->model() )->setConnections ( spConnections->value() );
}


void cs8ModbusWidget::setModel ( QAbstractItemModel* model )
{
  tableView->setModel ( model );
  connect ( model,SIGNAL ( modelReset() ),this,SLOT ( slotUpdateView() ) );
}


void cs8ModbusWidget::slotUpdateView()
{
  cs8ModbusConfigFile* model= ( cs8ModbusConfigFile* ) tableView->model();
  leName->setText ( model->topicName() );
  spPort->setValue ( model->port() );
  spConnections->setValue ( model->connections() );
}



/*!
    \fn cs8ModbusWidget::slotInsertItem()
 */
void cs8ModbusWidget::slotInsertItem()
{

  int selectedRow;
  if ( tableView->selectionModel()->selectedRows().count() >0 ) {
    selectedRow=tableView->selectionModel()->selectedRows().at ( 0 ).row();
    ( ( cs8ModbusConfigFile* ) tableView->model() )->insertRow ( selectedRow );
    //QModelIndex index=tableView->model()->
    //tableView->selectionModel()->select(tableView->model()->index(selectedRow, 1, tableView->rootIndex()), QItemSelectionModel::ClearAndSelect);
    tableView->setCurrentIndex ( tableView->model()->index ( selectedRow, 0, tableView->rootIndex() ) );
  } else {
    QMessageBox::warning ( this,tr ( "" ),tr ( "Please select a row first!" ) );
  }
}


/*!
    \fn cs8ModbusWidget::slotAppendItem()
 */
void cs8ModbusWidget::slotAppendItem()
{
  int selectedRow=tableView->model()->rowCount();
  ( ( cs8ModbusConfigFile* ) tableView->model() )->insertRow ( selectedRow );
  //QModelIndex index=tableView->model()->
  //tableView->selectionModel()->select(tableView->model()->index(selectedRow, 1, tableView->rootIndex()), QItemSelectionModel::ClearAndSelect);
  tableView->setCurrentIndex ( tableView->model()->index ( selectedRow, 0, tableView->rootIndex() ) );
}

/*!
    \fn cs8ModbusWidget::slotDeleteItem()
 */
void cs8ModbusWidget::slotDeleteItem()
{

  int selectedRow;
  if ( tableView->selectionModel()->selectedRows().count() >0 ) {
    selectedRow=tableView->selectionModel()->selectedRows().at ( 0 ).row();
    ( ( cs8ModbusConfigFile* ) tableView->model() )->removeRow ( selectedRow );
  } else {
    QMessageBox::warning ( this,tr ( "" ),tr ( "Please select a row first!" ) );
  }
}

QToolBar* cs8ModbusWidget::toolBar()
{
  if ( !m_toolBar ) {
    m_toolBar=new QToolBar ( this );
    m_toolBar->addAction ( actionInsert_Item );
    m_toolBar->addAction ( actionAppend_Item );
    m_toolBar->addAction ( actionDelete_Item );
    
    connect ( actionInsert_Item,SIGNAL ( triggered() ),this,SLOT ( slotInsertItem() ) );
    connect ( actionDelete_Item,SIGNAL ( triggered() ),this,SLOT ( slotDeleteItem() ) );
    connect ( actionAppend_Item,SIGNAL ( triggered() ),this,SLOT ( slotAppendItem() ) );
  }
  return m_toolBar;
}



bool cs8ModbusWidget::readOnly()
{
	return m_readOnly;
}

void cs8ModbusWidget::setReadOnly(bool value)
{
	m_readOnly = value;
	leName->setReadOnly(value);
	spPort->setReadOnly(value);
	spConnections->setReadOnly(value);
	(( cs8ModbusConfigFile* ) tableView->model())->setReadOnly(value);
}
