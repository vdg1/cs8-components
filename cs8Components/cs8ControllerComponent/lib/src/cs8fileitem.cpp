#include "cs8fileitem.h"
#include <QDebug>
#include <QDir>

// cs8FileItem::cs8FileItem() : m_checkState(Qt::Unchecked), m_parentItem(0) {}

cs8FileItem::cs8FileItem(cs8FileBrowserModel *model)
    : m_checkState(Qt::Unchecked), m_isDir(false), m_fileSize(0), m_childsEnumerated(false), m_parentItem(0) {
  m_model = model;
}

cs8FileItem::cs8FileItem(const QString &name, cs8FileItem *parent)
    : m_checkState(Qt::Unchecked), m_isDir(false), m_fileSize(0), m_childsEnumerated(false), m_model(0) {
  m_filePath = name;
  m_fileName = name;
  m_parentItem = parent;
  if (parent)
    m_model = parent->model();
}

cs8FileItem::cs8FileItem(const QFileInfo &info, cs8FileItem *parentItem)
    : m_parentItem(parentItem), m_checkState(Qt::Unchecked), m_childsEnumerated(false) {
  m_filePath = info.absoluteFilePath();
  m_fileName = QDir::fromNativeSeparators(m_filePath).split("/").last();
  m_isDir = info.isDir();
  m_fileSize = info.size();
  if (parentItem)
    m_model = parentItem->model();
}

cs8FileItem::cs8FileItem(const QUrlInfo &info, cs8FileItem *parentItem)
    : m_parentItem(parentItem), m_checkState(Qt::Unchecked), m_childsEnumerated(false) {
  m_filePath = info.name();
  m_fileName = QDir::fromNativeSeparators(m_filePath).split("/").last();
  m_isDir = info.isDir();
  m_fileSize = info.size();
  if (parentItem)
    m_model = parentItem->model();
}

cs8FileItem::cs8FileItem(const cs8FileItem &item, cs8FileItem *parentItem)
    : m_parentItem(parentItem), m_checkState(Qt::Unchecked), m_childsEnumerated(false) {
  m_filePath = item.filePath();
  m_fileName = item.fileName();
  m_isDir = item.isDir();
  m_fileSize = item.fileSize();
  if (parentItem)
    m_model = parentItem->model();
}

cs8FileItem::~cs8FileItem() { qDeleteAll(m_childItems); }

void cs8FileItem::appendChild(cs8FileItem *item) { m_childItems.append(item); }

void cs8FileItem::appendFileInfos(const QList<cs8FileItem> &list) {
  qDebug() << __FUNCTION__ << " Adding " << list.count() << " to " << m_filePath;
  for (auto &item : list) {
    m_childItems.append(new cs8FileItem(item, this));
  }
}

cs8FileItem *cs8FileItem::child(int row) { return m_childItems.value(row); }

int cs8FileItem::childCount() const { return m_childItems.count(); }

int cs8FileItem::row() const {
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<cs8FileItem *>(this));

  return 0;
}

int cs8FileItem::columnCount() const { return 3; }

QVariant cs8FileItem::data(int column) const {
  switch (column) {
  case 0:
    return m_filePath;
    break;
  case 1:
    return m_fileSize;
    break;
  case 2:
    return m_isDir;
    break;
  }
  return QVariant();
}

cs8FileItem *cs8FileItem::parentItem() { return m_parentItem; }

Qt::CheckState cs8FileItem::checkState() const {
  // qDebug() << __FUNCTION__ << ":" << m_itemData[0] << ":" << m_checked;
  return m_checkState;
}

void cs8FileItem::setChecked(Qt::CheckState checked) {
  // qDebug() << __FUNCTION__ << ":" << m_itemData[0] << ":" << checked;

  m_checkState = checked;
}

QList<cs8FileItem *> cs8FileItem::childItems() const { return m_childItems; }

QString cs8FileItem::filePath() const { return m_filePath; }

int cs8FileItem::fileSize() const { return m_fileSize; }

bool cs8FileItem::isDir() const { return m_isDir; }

QString cs8FileItem::fileName() const { return m_fileName; }

bool cs8FileItem::childsEnumerated() const { return m_childsEnumerated; }

cs8FileBrowserModel *cs8FileItem::model() const { return m_model; }

void cs8FileItem::setChildsEnumerated(bool childsEnumerated) { m_childsEnumerated = childsEnumerated; }

int cs8FileItem::childNumber() const {
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<cs8FileItem *>(this));

  return 0;
}

bool cs8FileItem::setData(int column, const QVariant &value) {
  if (column < 0 || column >= 1)
    return false;

  switch (column) {
  case 0:
    m_filePath = value.toString();
  }
  return true;
}

bool cs8FileItem::removeChildren(int position, int count) {
  if (position < 0 || position + count > m_childItems.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete m_childItems.takeAt(position);

  return true;
}

bool cs8FileItem::insertColumns(int position, int columns) {
  /*if (position < 0 || position > m_itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    m_itemData.insert(position, QVariant());

  foreach (cs8FileItem *child, m_childItems)
    child->insertColumns(position, columns);
*/
  return true;
}

bool cs8FileItem::insertChildren(int position, int count, int columns, const QString &title) {
  if (position < 0 || position > m_childItems.size())
    return false;

  for (int row = 0; row < count; ++row) {
    cs8FileItem *item = new cs8FileItem(title, this);
    m_childItems.insert(position, item);
  }
  return true;
}

bool cs8FileItem::removeColumns(int position, int columns) {
  /* if (position < 0 || position + columns > m_itemData.size())
     return false;

   for (int column = 0; column < columns; ++column)
     m_itemData.remove(position);

   foreach (cs8FileItem *child, m_childItems)
     child->removeColumns(position, columns);
 */
  return true;
}

bool cs8FileItem::hasChildren() const { return childCount() != 0; }

bool cs8FileItem::hasSiblings() const {
  if (m_parentItem != nullptr)
    return m_parentItem->childCount() > 1;
  else
    return false;
}

bool cs8FileItem::allChildsCheckedState(Qt::CheckState stateToCheck) const {
  foreach (auto item, m_childItems) {
    if (item->checkState() != stateToCheck)
      return false;
  }
  return true;
}
