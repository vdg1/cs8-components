#ifndef CS8ABSTRACTBROWSER_H
#define CS8ABSTRACTBROWSER_H

#include <QFileInfo>
#include <QObject>
#include <QUrl>

class cs8AbstractBrowser : public QObject {
  Q_OBJECT
public:
  explicit cs8AbstractBrowser(const QUrl &url, QObject *parent = nullptr);
  virtual bool getProfiles(QFileInfoList &profiles) = 0;
signals:
  void urlChanged();

public slots:

protected:
  QUrl m_url;
};

#endif // CS8ABSTRACTBROWSER_H
