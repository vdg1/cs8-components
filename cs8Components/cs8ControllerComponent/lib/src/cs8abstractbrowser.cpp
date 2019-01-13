#include "cs8abstractbrowser.h"
#include <QDebug>

cs8AbstractBrowser::cs8AbstractBrowser(const QUrl &url, QObject *parent) : QObject(parent), m_url(url) {
  qDebug() << __FUNCTION__ << ":" << url.toString();
}

QString cs8AbstractBrowser::controllerName() const {
  if (m_url.scheme() == "ftp")
    return m_url.host();
  else
    return m_url.path().split("/").last();
}

QUrl cs8AbstractBrowser::url() const { return m_url; }
