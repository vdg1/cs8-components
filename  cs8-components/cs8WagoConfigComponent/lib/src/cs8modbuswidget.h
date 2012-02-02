#ifndef CS8MODBUSWIDGET_H
#define CS8MODBUSWIDGET_H
//
#include <QWidget>
#include "ui_cs8ModbusWidget.h"
//
class QToolBar;
class cs8ModbusWidget : public QWidget, public Ui::cs8ModbusWidgetBase
{
public:
	void setReadOnly(bool value);
	bool readOnly();
private:
	bool m_readOnly;
		Q_OBJECT
				
	protected slots:
		void slotUpdateView();
    void slotDeleteItem();
    void slotAppendItem();
		
	public:
		void setModel ( QAbstractItemModel* model );
		cs8ModbusWidget ( QWidget * parent = 0 );
		QToolBar* toolBar();
		
	private slots:
		void on_leName_editingFinished();
		void on_spPort_editingFinished();
		void on_spConnections_editingFinished();
		
	protected:
		QToolBar* m_toolBar;
public slots:
    void slotInsertItem();
};
#endif





