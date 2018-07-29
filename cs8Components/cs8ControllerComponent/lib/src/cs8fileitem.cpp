#include "cs8fileitem.h"

cs8FileItem::cs8FileItem(const QVector<QVariant> &data, cs8FileItem *parent) {
  m_parentItem = parent;
  m_itemData = data;
}

cs8FileItem::~cs8FileItem() { qDeleteAll(m_childItems); }

void cs8FileItem::appendChild(cs8FileItem *item) { m_childItems.append(item); }

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
