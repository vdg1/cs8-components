//
// C++ Implementation: cs8modbusdelegate
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8modbusdelegate.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QModelIndex>
#include <QComboBox>
#include <QRegExpValidator>

cs8ModbusDelegate::cs8ModbusDelegate ( QObject *parent )
    :QItemDelegate ( parent )
{}

QWidget *cs8ModbusDelegate::createEditor ( QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
  QWidget* editor;
  switch ( index.column() ) {
    case 0: {
        editor= new QLineEdit ( parent );
        ( ( QLineEdit* ) editor )->setMaxLength ( 16 );
        
        QRegExpValidator *validator=new QRegExpValidator ( editor );
        validator->setRegExp ( QRegExp ( "[a-Z]+\\w*" ) );
        
        ( ( QLineEdit* ) editor )->setValidator ( validator );
      }
      break;
      
    case 1:
      editor=new QComboBox ( parent );
      ( ( QComboBox* ) editor )->addItems ( QStringList() << "B" << "W" << "D" << "F" );
      break;
      
    case 2:
      editor = new QSpinBox ( parent );
      ( ( QSpinBox* ) editor )->setMinimum ( 1 );
      //connect(( ( QSpinBox* ) editor ), SIGNAL(valueChanged(int)), 
      break;
      
    case 3:
      editor=new QComboBox ( parent );
      ( ( QComboBox* ) editor )->addItems ( QStringList() << "R" << "RW" );
      break;
      
    default:
      return 0;
      
  }
  //editor->setMaximum(100);
  
  return editor;
}

void cs8ModbusDelegate::setEditorData ( QWidget *editor,
                                        const QModelIndex &index ) const
{
  switch ( index.column() ) {
    case 0: {
        QString value = index.model()->data ( index, Qt::DisplayRole ).toString();
        QLineEdit *lineEdit = static_cast<QLineEdit*> ( editor );
        lineEdit->setText ( value );
      }
      break;
      
    case 1: {
        int value = index.model()->data ( index, Qt::EditRole ).toString().toUInt();
        QComboBox *comboBox = static_cast<QComboBox*> ( editor );
        comboBox->setCurrentIndex ( value );
      }
      break;
      
    case 2: {
        int value = index.model()->data ( index, Qt::DisplayRole ).toInt();
        QSpinBox *spinBox = static_cast<QSpinBox*> ( editor );
        spinBox->setValue ( value );
      }
      break;
      
    case 3: {
        int value = index.model()->data ( index, Qt::EditRole ).toUInt();
        QComboBox *comboBox = static_cast<QComboBox*> ( editor );
        comboBox->setCurrentIndex ( value );
      }
      break;
      
  }
  
}

void cs8ModbusDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index ) const
{

  switch ( index.column() ) {
    case 0: {
        QLineEdit *lineEdit = static_cast<QLineEdit*> ( editor );
        QString value = lineEdit->text();
        model->setData ( index, value, Qt::EditRole );
      }
      break;
      
    case 1: {
        QComboBox *comboBox = static_cast<QComboBox*> ( editor );
        QString value = comboBox->currentText();
        model->setData ( index, value, Qt::EditRole );
      }
      break;
      
    case 2: {
        QSpinBox *spinBox = static_cast<QSpinBox*> ( editor );
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData ( index, value, Qt::EditRole );
      }
      break;
      
    case 3: {
        QComboBox *comboBox = static_cast<QComboBox*> ( editor );
        QString value = comboBox->currentText();
        model->setData ( index, value, Qt::EditRole );
      }
      break;
      
  }
}

void cs8ModbusDelegate::updateEditorGeometry ( QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */ ) const
{
  editor->setGeometry ( option.rect );
}
