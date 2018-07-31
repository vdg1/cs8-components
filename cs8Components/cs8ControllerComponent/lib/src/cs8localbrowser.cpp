#include "cs8localbrowser.h"
#include <QDebug>
#include <QDir>

cs8LocalBrowser::cs8LocalBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {}

QFileInfoList cs8LocalBrowser::getProfiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/usr/configs/profiles";
  QDir dir(m_url.toString() + "/usr/configs/profiles");
  if (ok)
    *ok = dir.exists();
  return dir.entryInfoList(QStringList() << "*.cfx", QDir::Files);
}

QFileInfoList cs8LocalBrowser::getLogFiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/log";
  QDir dir(m_url.toString() + "/log");
  if (ok)
    *ok = dir.exists();
  return dir.entryInfoList(QStringList() << "errors.*", QDir::Files);
}
