#ifndef CS8LOCALBROWSER_H
#define CS8LOCALBROWSER_H

#include "cs8abstractbrowser.h"

class cs8LocalBrowser : public cs8AbstractBrowser {
public:
  cs8LocalBrowser(const QUrl &url, QObject *parent = nullptr);
  QFileInfoList getProfiles(bool *ok = 0) override;
  QFileInfoList getLogFiles(bool *ok = 0) override;
};

#endif // CS8LOCALBROWSER_H
