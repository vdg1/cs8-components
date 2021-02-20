#ifndef CS8FILEITEM_H
#define CS8FILEITEM_H

#include "qurlinfo.h"
#include <QFileInfoList>
#include <QList>
#include <QVariant>
#include <QVector>
class cs8FileItem;
typedef QList<cs8FileItem> cs8FileItemList;
class cs8FileBrowserModel;

class cs8FileItem {
public:
  // explicit cs8FileItem();
  explicit cs8FileItem(cs8FileBrowserModel *model = 0);
  explicit cs8FileItem(const QString &name = "", cs8FileItem *parentItem = 0);
  explicit cs8FileItem(const QFileInfo &info, cs8FileItem *parentItem = 0);
  explicit cs8FileItem(const QUrlInfo &info, cs8FileItem *parentItem = 0);
  explicit cs8FileItem(const cs8FileItem &item, cs8FileItem *parentItem = 0);
  ~cs8FileItem();

  void appendChild(cs8FileItem *child);
  void appendFileInfos(const QList<cs8FileItem> &list);

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

  Qt::CheckState checkState() const;
  void setChecked(Qt::CheckState checkState);

  QList<cs8FileItem *> childItems() const;

  QString filePath() const;

  int fileSize() const;

  bool isDir() const;

  QString fileName() const;

  bool childsEnumerated() const;

  cs8FileBrowserModel *model() const;

  void setChildsEnumerated(bool childsEnumerated);

private:
  QList<cs8FileItem *> m_childItems;
  QString m_filePath;
  QString m_fileName;
  int m_fileSize;
  bool m_isDir;
  bool m_childsEnumerated;
  cs8FileItem *m_parentItem;
  cs8FileBrowserModel *m_model;
  Qt::CheckState m_checkState;
};

#endif // CS8FILEITEM_H
