#ifndef DIALOGIMPL_H
#define DIALOGIMPL_H
//
#include "ui_dialog.h"

#include "../../lib/src/cs8LoggerComponent.h"

//

class QMdiArea;
class cs8LoggerWidget;
class cs8PlotStatisticWidget;
class cs8LogStatisticsWidget;
class QMdiSubWindow;

class DialogImpl : public QMainWindow, public Ui::MainWindow
  {
    Q_OBJECT
protected:
	void closeEvent(QCloseEvent *event);
public slots:
	void restoreSettings();
	void saveSettings();
  private:
	QString m_fileName;
    QMdiArea* m_mdiArea;
    cs8LoggerWidget* m_loggerWidget;
    cs8PlotStatisticWidget* m_plotWidget;
    cs8LogStatisticsWidget* m_statisticWidget;
  protected slots:
	void slotPlotWindow();
	void slotMessagesWindow();
	void slotStatisticWindow();
    void slotAttributeChanged ( cs8LoggerWidget::Attribute attribute,
                                const QString & value );
    void slotDisconnected();
    void slotConnected();
    void slotClear();
    void slotOpen();
    void slotConnect();
    void slotLoad();
  public:
    void setupConnections();
    DialogImpl ( QWidget * parent = 0, Qt::WFlags f = 0 );
  };
#endif




