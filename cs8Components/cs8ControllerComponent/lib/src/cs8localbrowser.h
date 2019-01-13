#ifndef CS8LOCALBROWSER_H
#define CS8LOCALBROWSER_H

#include "cs8abstractbrowser.h"
#include <QDir>

class cs8LocalBrowser : public cs8AbstractBrowser {
public:
  cs8LocalBrowser(const QUrl &url, QObject *parent = nullptr);
  cs8FileItemList getProfiles(bool *ok = 0) override;
  cs8FileItemList getLogFiles(bool *ok = 0) override;
  cs8FileItemList getApplications(cs8FileItem *parent = 0, bool *ok = 0) override;
  bool canFetchMore(const cs8FileItem &parent) const override;

protected:
  cs8FileItemList getDirContents(const QString &path, const QString &nameFilter = "*.*",
                                 const QFlags<QDir::Filter> &filter = QDir::Files, bool *ok = 0);
};

#endif // CS8LOCALBROWSER_H
