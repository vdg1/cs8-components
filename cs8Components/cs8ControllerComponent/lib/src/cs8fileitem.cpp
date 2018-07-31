#include "cs8fileitem.h"
#include <QDebug>

cs8FileItem::cs8FileItem(const QString &title, cs8FileItem *parent) : m_checked(Qt::Unchecked) {
  m_itemData << title;
  m_parentItem = parent;
}

cs8FileItem::~cs8FileItem() { qDeleteAll(m_childItems); }

void cs8FileItem::appendChild(cs8FileItem *item) { m_childItems.append(item); }

void cs8FileItem::appendFileInfos(const QFileInfoList &list) {
  foreach (auto item, list) { m_childItems.append(new cs8FileItem(item.fileName(), this)); }
}

cs8FileItem *cs8FileItem::child(int row) { return m_childItems.value(row); }

int cs8FileItem::childCount() const { return m_childItems.count(); }

int cs8FileItem::row() const {
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<cs8FileItem *>(this));

  return 0;
}

int cs8FileItem::columnCount() const { return m_itemData.count(); }

QVariant cs8FileItem::data(int column) const { return m_itemData.value(column); }

cs8FileItem *cs8FileItem::parentItem() { return m_parentItem; }

Qt::CheckState cs8FileItem::checked() const {
  // qDebug() << __FUNCTION__ << ":" << m_itemData[0] << ":" << m_checked;
  return m_checked ? Qt::Checked : Qt::Unchecked;
}

void cs8FileItem::setChecked(Qt::CheckState checked) {
  // qDebug() << __FUNCTION__ << ":" << m_itemData[0] << ":" << checked;

  m_checked = checked;
}

QList<cs8FileItem *> cs8FileItem::childItems() const { return m_childItems; }

int cs8FileItem::childNumber() const {
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<cs8FileItem *>(this));

  return 0;
}

bool cs8FileItem::setData(int column, const QVariant &value) {
  if (column < 0 || column >= m_itemData.size())
    return false;

  m_itemData[column] = value;
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
  if (position < 0 || position > m_itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    m_itemData.insert(position, QVariant());

  foreach (cs8FileItem *child, m_childItems)
    child->insertColumns(position, columns);

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
  if (position < 0 || position + columns > m_itemData.size())
    return false;

  for (int column = 0; column < columns; ++column)
    m_itemData.remove(position);

  foreach (cs8FileItem *child, m_childItems)
    child->removeColumns(position, columns);

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
    if (item->checked() != stateToCheck)
      return false;
  }
  return true;
}
