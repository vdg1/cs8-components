#include "cs8localbrowser.h"
#include "cs8filebrowsermodel.h"
#include <QDebug>
#include <QDir>

cs8LocalBrowser::cs8LocalBrowser(const QUrl &url, QObject *parent) : cs8AbstractBrowser(url, parent) {}

cs8FileItemList cs8LocalBrowser::getProfiles(bool *ok) {
  return getDirContents("/usr/configs/profiles", "*.cfx", QDir::Files, ok);
}

cs8FileItemList cs8LocalBrowser::getDirContents(const QString &path, const QString &nameFilter,
                                                const QFlags<QDir::Filter> &filter, bool *ok) {

  qDebug() << __FUNCTION__ << m_url.toString() + path;
  QDir dir(m_url.toString() + path);
  if (ok)
    *ok = dir.exists();
  cs8FileItemList list;
  foreach (auto item, dir.entryInfoList(QStringList() << nameFilter, filter)) { list << cs8FileItem(item); }
  return list;
}

cs8FileItemList cs8LocalBrowser::getLogFiles(bool *ok) { return getDirContents("/log", "errors.*", QDir::Files, ok); }

cs8FileItemList cs8LocalBrowser::getApplications(cs8FileItem *parent, bool *ok) {
  if (parent == nullptr) {
    return getDirContents("/usr/usrapp", "", QDir::AllDirs | QDir::NoDotAndDotDot, ok);
  } else {
    return getDirContents(parent->filePath().remove(0, parent->model()->url().toString().length()), "",
                          QDir::AllDirs | QDir::NoDotAndDotDot, ok);
  }
}

bool cs8LocalBrowser::canFetchMore(const cs8FileItem &parent) const { return false; }
