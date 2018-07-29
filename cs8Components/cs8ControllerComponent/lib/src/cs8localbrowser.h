#ifndef CS8LOCALBROWSER_H
#define CS8LOCALBROWSER_H

#include "cs8abstractbrowser.h"

class cs8LocalBrowser : public cs8AbstractBrowser {
public:
  cs8LocalBrowser(const QUrl &url, QObject *parent = nullptr);
  bool getProfiles(QFileInfoList &profiles);
};

#endif // CS8LOCALBROWSER_H
