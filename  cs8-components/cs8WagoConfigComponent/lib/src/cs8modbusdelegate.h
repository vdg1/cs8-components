//
// C++ Interface: cs8modbusdelegate
//
// Description: 
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8MODBUSDELEGATE_H
#define CS8MODBUSDELEGATE_H

#include <QItemDelegate>

/**
	@author Volker Drewer-Gutland <volker.drewer@gmx.de>
*/
class cs8ModbusDelegate : public QItemDelegate
{
	Q_OBJECT

	public:
		cs8ModbusDelegate(QObject *parent = 0);

		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
							  const QModelIndex &index) const;

		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model,
						  const QModelIndex &index) const;

		void updateEditorGeometry(QWidget *editor,
								  const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif
