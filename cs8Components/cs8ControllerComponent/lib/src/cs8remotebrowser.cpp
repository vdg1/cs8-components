#include "cs8remotebrowser.h"

cs8RemoteBrowser::cs8RemoteBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {
  m_controller = new cs8Controller(this);
  m_controller->setUrl(m_url);
}

QFileInfoList cs8RemoteBrowser::getProfiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/usr/configs/profiles";
  QList<QUrlInfo> list;
  if (m_controller->getFolderContents("/usr/configs/profiles", list)) {
    QFileInfoList outList;
    foreach (auto info, list) {
      QFileInfo item;
      item.setFile(info.name());
      outList << item;
    }
    *ok = true;
    return outList;

  } else {
    *ok = false;
    return QFileInfoList();
  }
}

QFileInfoList cs8RemoteBrowser::getLogFiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/log";
  QList<QUrlInfo> list;
  if (m_controller->getFolderContents("/log", list)) {
    QFileInfoList outList;
    foreach (auto info, list) {
      QFileInfo item;
      item.setFile(info.name());
      outList << item;
    }
    *ok = true;
    return outList;

  } else {
    *ok = false;
    return QFileInfoList();
  }
}
