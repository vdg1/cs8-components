#ifndef CS8ABSTRACTBROWSER_H
#define CS8ABSTRACTBROWSER_H

#include "cs8fileitem.h"
#include <QFileInfo>
#include <QObject>
#include <QUrl>

class cs8AbstractBrowser : public QObject {
  Q_OBJECT
public:
  explicit cs8AbstractBrowser(const QUrl &url, QObject *parent = nullptr);
  virtual cs8FileItemList getProfiles(bool *ok = 0) = 0;
  virtual cs8FileItemList getLogFiles(bool *ok = 0) = 0;
  virtual cs8FileItemList getApplications(cs8FileItem *parent = 0, bool *ok = 0) = 0;
  virtual bool canFetchMore(const cs8FileItem &parent) const = 0;
  QString controllerName() const;
  QUrl url() const;

signals:
  void urlChanged();

public slots:

protected:
  QUrl m_url;
};

#endif // CS8ABSTRACTBROWSER_H
