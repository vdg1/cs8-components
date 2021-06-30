#include "cs8applicationmodel.h"

cs8ApplicationModel::cs8ApplicationModel(QObject *parent)
    : QAbstractListModel(parent) {}

QVariant cs8ApplicationModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const {
  return QString("Name");
}

int cs8ApplicationModel::rowCount(const QModelIndex &parent) const {
  // For list models only the root node (an invalid parent) should return the
  // list's size. For all other (valid) parents, rowCount() should return 0 so
  // that it does not become a tree model.
  if (parent.isValid())
    return 0;

  return m_applications.count();
}

QVariant cs8ApplicationModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  // FIXME: Implement me!
  return QVariant();
}

bool cs8ApplicationModel::insertRows(int row, int count,
                                     const QModelIndex &parent) {
  beginInsertRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endInsertRows();
  return true;
}

bool cs8ApplicationModel::removeRows(int row, int count,
                                     const QModelIndex &parent) {
  beginRemoveRows(parent, row, row + count - 1);
  // FIXME: Implement me!
  endRemoveRows();
  return true;
}

cs8Application *cs8ApplicationModel::openApplication(const QString &filePath) {
  cs8Application *a = new cs8Application(this);
  if (a->openFromPathName(filePath)) {
    m_applications[a->projectPath()] = a;
    for (const auto alias : a->libraryModel()->list())
      m_applications[alias->path()] = alias->application();
    return a;
  }
  return nullptr;
}
