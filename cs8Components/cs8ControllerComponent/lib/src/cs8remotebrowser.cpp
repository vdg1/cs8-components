#include "cs8remotebrowser.h"

cs8RemoteBrowser::cs8RemoteBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {
  m_controller = new cs8Controller(this);
  m_controller->setUrl(m_url);
}

cs8FileItemList cs8RemoteBrowser::getProfiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/usr/configs/profiles";
  QList<QUrlInfo> list;
  if (m_controller->getFolderContents("/usr/configs/profiles", "*", QDir::Files, list)) {
    cs8FileItemList outList;
    foreach (auto info, list) {
      if (info.name().contains(".cfx"))
        outList << cs8FileItem(info);
    }
    if (ok)
      *ok = true;
    return outList;

  } else {
    if (ok)
      *ok = false;
    return cs8FileItemList();
  }
}

cs8FileItemList cs8RemoteBrowser::getLogFiles(bool *ok) {
  qDebug() << __FUNCTION__ << m_url.toString() + "/log";
  QList<QUrlInfo> list;
  if (m_controller->getFolderContents("/log", "*", QDir::Files, list)) {
    cs8FileItemList outList;
    foreach (auto info, list) {
      if (info.name().contains("errors."))
        outList << cs8FileItem(info);
    }
    if (ok)
      *ok = true;
    return outList;

  } else {
    if (ok)
      *ok = false;
    return cs8FileItemList();
  }
}

cs8FileItemList cs8RemoteBrowser::getApplications(cs8FileItem *parent, bool *ok) { return cs8FileItemList(); }

bool cs8RemoteBrowser::canFetchMore(const cs8FileItem &parent) const { return false; }
