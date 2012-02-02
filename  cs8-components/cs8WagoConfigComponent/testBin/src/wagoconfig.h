#ifndef WAGOCONFIG_H
#define WAGOCONFIG_H

#include <QtGui/QWidget>
#include <QByteArray>
#include <QHttpResponseHeader>
#include <QTcpSocket>


#include "ui_cs8WagoConfigMainWindow.h"

#include "progressdialogbaseimpl.h"

class cs8WagoNode;
class cs8ModbusDelegate;
class cs8ModbusConfigFile;

class WagoConfig : public QMainWindow
{
    Q_OBJECT
  protected slots:
	void slotRefresh();
	void slotAboutQt();
	void slotAbout();
    void slotCommit();
    void slotSystemRefreshComplete ( bool error );
    void slotUserRefreshComplete ( bool error );
    void slotUserProposal();
    void slotSystemProposal();
    void slotRemoteOpen();
    bool slotRecommission();
    void slotSystemCommitComplete ( bool error );
    void slotUserCommitComplete ( bool error );
    void getConfig();
    void slotOpen();
    void slotRestoreConfig();
    void slotBackupConfig();
    
  public:
    WagoConfig ( QWidget *parent = 0 );
    ~WagoConfig();
    
  private:
    progressDialogBaseImpl* m_progress;
    bool m_modbusConfigValid;
    cs8ModbusConfigFile* m_modbusFile;
    cs8WagoNode* m_userNode;
    cs8WagoNode* m_systemNode;
    Ui::cs8WagoConfigMainWindow ui;
};

#endif // WAGOCONFIG_H
