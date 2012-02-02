//
// C++ Interface:
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CS8WAGONODEWIDGET_H
#define CS8WAGONODEWIDGET_H

#include <QWidget>
#include "ui_cs8WagoNodeWidget.h"

class cs8WagoNode;
class QSignalMapper;

class cs8WagoNodeWidget : public QWidget, private Ui::cs8WagoNodeWidgetBase
{
    Q_OBJECT
  private:
    QSignalMapper* m_mapper;
    
  public:
    void disableControls();
    void enableControls ( bool enable=true );
    cs8WagoNodeWidget ( QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~cs8WagoNodeWidget();
    void attach ( cs8WagoNode* node );
    
    /*$PUBLIC_FUNCTIONS$*/
    
  public slots:
    void update();
    /*$PUBLIC_SLOTS$*/
    
  protected:
    /*$PROTECTED_FUNCTIONS$*/
    
  protected slots:
    void slotRefreshComplete ( bool error );
    /*$PROTECTED_SLOTS$*/
  protected:
    cs8WagoNode* m_node;
    
  protected slots:
    void slotChanged ( const QString & source );
};

#endif

