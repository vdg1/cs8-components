#ifndef CS8FILEITEM_H
#define CS8FILEITEM_H

#include <QFileInfoList>
#include <QList>
#include <QVariant>
#include <QVector>

class cs8FileItem {
public:
  explicit cs8FileItem(const QString &title, cs8FileItem *parentItem = 0);
  ~cs8FileItem();

  void appendChild(cs8FileItem *child);
  void appendFileInfos(const QFileInfoList &list);

  cs8FileItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column) const;
  bool insertChildren(int position, int count, int columns, const QString &title = QString());
  bool insertColumns(int position, int columns);
  bool removeChildren(int position, int count);
  bool removeColumns(int position, int columns);
  bool hasChildren() const;
  bool hasSiblings() const;
  bool allChildsCheckedState(Qt::CheckState stateToCheck) const;
  int childNumber() const;
  bool setData(int column, const QVariant &value);
  int row() const;
  cs8FileItem *parentItem();

  Qt::CheckState checked() const;
  void setChecked(Qt::CheckState checked);

  QList<cs8FileItem *> childItems() const;

private:
  QList<cs8FileItem *> m_childItems;
  QVector<QVariant> m_itemData;
  cs8FileItem *m_parentItem;
  Qt::CheckState m_checked;
};

#endif // CS8FILEITEM_H
