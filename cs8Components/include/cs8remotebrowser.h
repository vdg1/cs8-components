#ifndef CS8REMOTEBROWSER_H
#define CS8REMOTEBROWSER_H

#include "cs8abstractbrowser.h"
#include "cs8controller.h"

class cs8RemoteBrowser : public cs8AbstractBrowser {
public:
  explicit cs8RemoteBrowser(const QUrl &url, QObject *parent = nullptr);

  cs8FileItemList getProfiles(bool *ok = Q_NULLPTR) override;
  cs8FileItemList getLogFiles(bool *ok = 0) override;
  cs8FileItemList getApplications(cs8FileItem *parent = 0, bool *ok = 0) override;
  bool canFetchMore(const cs8FileItem &parent) const override;

protected:
  cs8Controller *m_controller;
};

#endif // CS8REMOTEBROWSER_H
