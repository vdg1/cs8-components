#include "dialogimpl.h"
#include "cs8ControllerComponent.h"

#include "cs8filebrowsermodel.h"
#include "cs8localbrowser.h"
#include "cs8remotebrowser.h"
#include <QDebug>
#include <QDirIterator>
#include <QDirModel>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
//
DialogImpl::DialogImpl(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {
  setupUi(this);
  connect(btLoad, SIGNAL(clicked()), this, SLOT(slotLoad()));
  m_ctrl = new cs8Controller(this);
  // m_ctrl->enableOnlineCheck(true);
  m_ctrl->setAddress(leHost->text());
  scanner = new cs8NetworkScanner(this);

  // scanner->start(true);
  view->setModel(scanner);
  connect(scanner, SIGNAL(scannerActive(bool)), pushButton, SLOT(setDisabled(bool)));

  QObject::connect(m_ctrl, &cs8Controller::onlineChanged, radioButton, &QAbstractButton::setChecked);

  m_telnet = new cs8Telnet(this);
  QObject::connect(m_telnet, &cs8Telnet::telnetLine, [=](const QString &newValue) { textEdit->append(newValue); });

  m_browser = new cs8FileBrowserModel(
      QUrl(QDir::fromNativeSeparators(
          "D:\\data\\Staubli\\SRS\\_Development\\Dev_SaxeAutomation_SRS2014\\Dev_SaxeAutomation")),
      this);
  treeView->setModel(m_browser);
}
//

void DialogImpl::slotHostListChanged() {
  // listWidgetHosts->clear();
  // listWidgetHosts->addItems(scanner->hostList());
  scanner->stop();
}

/*!
    \fn DialogImpl::slotLoad()
 */
void DialogImpl::slotLoad() {
  textEdit->setText(QString(""));
  QFile file;
  QString text;

  m_ctrl->setAddress(leHost->text());
  text = m_ctrl->armSerialNumber();
  file.close();

  textEdit->setText(text);
}

void DialogImpl::on_leHost_textChanged(const QString &arg1) { m_ctrl->setAddress(arg1); }

void DialogImpl::on_pushButton_clicked() { scanner->start(); }

void DialogImpl::on_pushButton_2_clicked() { m_telnet->connectToHost(leHost->text(), 23); }
