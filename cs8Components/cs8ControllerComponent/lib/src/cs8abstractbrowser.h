#ifndef CS8ABSTRACTBROWSER_H
#define CS8ABSTRACTBROWSER_H

#include <QFileInfo>
#include <QObject>
#include <QUrl>

class cs8AbstractBrowser : public QObject {
  Q_OBJECT
public:
  explicit cs8AbstractBrowser(const QUrl &url, QObject *parent = nullptr);
  virtual QFileInfoList getProfiles(bool *ok = 0) = 0;
  virtual QFileInfoList getLogFiles(bool *ok = 0) = 0;
  QString controllerName() const;
signals:
  void urlChanged();

public slots:

protected:
  QUrl m_url;
};

#endif // CS8ABSTRACTBROWSER_H
