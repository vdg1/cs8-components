#include "cs8remotebrowser.h"

cs8RemoteBrowser::cs8RemoteBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {
  m_controller = new cs8Controller(this);
  m_controller->setUrl(m_url);
}

bool cs8RemoteBrowser::getProfiles(QFileInfoList &profiles) {
  QList<QUrlInfo> list;
  if (m_controller->getFolderContents("/usr/config/profiles", list)) {
    QFileInfoList outList;
    foreach (auto info, list) {
      QFileInfo item;
      item.setFile(info.name());
      outList << item;
    }
    profiles = outList;
    return true;
  } else {
    profiles = QFileInfoList();
    return false;
  }
}
