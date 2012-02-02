#ifndef CS8WAGONEWADDRESSWIDGET_H
#define CS8WAGONEWADDRESSWIDGET_H
//
#include <QDialog>
#include "ui_cs8WagoNewAddress.h"

class cs8WagoNode;
//
class cs8WagoNewAddressWidget : public QDialog, public Ui::cs8WagoNewAddressWidget
{
Q_OBJECT
public:
	void attach ( cs8WagoNode* node );
	cs8WagoNewAddressWidget( QWidget * parent = 0, Qt::WFlags f = 0 );

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();
	
	protected:
    cs8WagoNode* m_node;

protected slots:
	void slotChanged();
    void slotCommited(bool error);
};
#endif





