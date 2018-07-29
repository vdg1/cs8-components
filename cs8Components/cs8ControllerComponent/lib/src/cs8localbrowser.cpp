#include "cs8localbrowser.h"

#include <QDir>

cs8LocalBrowser::cs8LocalBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {}

bool cs8LocalBrowser::getProfiles(QFileInfoList &profiles) {
  QDir dir(m_url.toString() + "/usr/config/profiles");
  profiles = dir.entryInfoList(QStringList() << "cfx", QDir::Files);
  return true;
}
