#ifndef CS8REMOTEBROWSER_H
#define CS8REMOTEBROWSER_H

#include "cs8abstractbrowser.h"
#include "cs8controller.h"

class cs8RemoteBrowser : public cs8AbstractBrowser {
public:
  explicit cs8RemoteBrowser(const QUrl &url, QObject *parent = nullptr);
  bool getProfiles(QFileInfoList &profiles) override;

protected:
  cs8Controller *m_controller;
};

#endif // CS8REMOTEBROWSER_H
