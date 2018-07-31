#include "cs8abstractbrowser.h"

cs8AbstractBrowser::cs8AbstractBrowser(const QUrl &url, QObject *parent) : QObject(parent), m_url(url) {}

QString cs8AbstractBrowser::controllerName() const {
  if (m_url.scheme() == "ftp")
    return m_url.host();
  else
    return m_url.path().split("/").last();
}
