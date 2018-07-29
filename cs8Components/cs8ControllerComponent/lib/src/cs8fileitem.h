#ifndef CS8FILEITEM_H
#define CS8FILEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class cs8FileItem {
public:
  explicit cs8FileItem(const QVector<QVariant> &data, cs8FileItem *parentItem = 0);
  ~cs8FileItem();

  void appendChild(cs8FileItem *child);

  cs8FileItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  bool insertChildren(int position, int count, int columns);
  bool insertColumns(int position, int columns);
  bool removeChildren(int position, int count);
  bool removeColumns(int position, int columns);
  int childNumber() const;
  bool setData(int column, const QVariant &value);
  int row() const;
  cs8FileItem *parentItem();

private:
  QList<cs8FileItem *> m_childItems;
  QVector<QVariant> m_itemData;
  cs8FileItem *m_parentItem;
};

#endif // CS8FILEITEM_H
