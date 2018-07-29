#include "cs8abstractbrowser.h"

cs8AbstractBrowser::cs8AbstractBrowser(const QUrl &url, QObject *parent) : QObject(parent), m_url(url) {}
