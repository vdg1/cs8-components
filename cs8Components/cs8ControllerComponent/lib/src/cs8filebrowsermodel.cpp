#include "cs8filebrowsermodel.h"
#include "cs8localbrowser.h"
#include "cs8remotebrowser.h"

#include <QUrl>

void cs8FileBrowserModel::fillModel() {
  beginResetModel();
  if (rootItem != nullptr)
    delete rootItem;
  rootItem = new cs8FileItem(this /*m_backend->controllerName()*/);

  profilesNode = new cs8FileItem(tr("Profiles"), rootItem);
  // profilesNode->appendFileInfos(m_backend->getProfiles());
  rootItem->appendChild(profilesNode);

  logNode = new cs8FileItem(tr("Logs"), rootItem);
  // logNode->appendFileInfos(m_backend->getLogFiles());
  rootItem->appendChild(logNode);

  appNode = new cs8FileItem(tr("Applications"), rootItem);
  appNode->appendFileInfos(m_backend->getApplications());
  rootItem->appendChild(appNode);
  endResetModel();
}

void cs8FileBrowserModel::enumerateChildren(const QModelIndex &parentItem) {
  cs8FileItem *item = getItem(parentItem);
  if (item) {
    qDebug() << __FUNCTION__ << ":" << item->fileName();
    cs8FileItemList list = m_backend->getApplications(item);
    if (list.count() > 0) {
      beginInsertRows(parentItem, 0, list.count() - 1);
      item->appendFileInfos(list);
      endInsertRows();
    }
    item->setChildsEnumerated(true);
  }
}

cs8FileBrowserModel::cs8FileBrowserModel(const QUrl &url, QObject *parent)
    : QAbstractItemModel(parent), m_backend(0), rootItem(0) {
  if (url.scheme() == "ftp")
    m_backend = new cs8RemoteBrowser(url, this);
  else
    m_backend = new cs8LocalBrowser(url, this);

  rootItem = new cs8FileItem(this);
  QTimer::singleShot(0, this, &cs8FileBrowserModel::fillModel);
}

cs8FileBrowserModel::~cs8FileBrowserModel() { delete rootItem; }

int cs8FileBrowserModel::columnCount(const QModelIndex & /* parent */) const { return rootItem->columnCount(); }

bool cs8FileBrowserModel::canFetchMore(const QModelIndex &parent) const {
  cs8FileItem *item = getItem(parent);
  return m_backend->canFetchMore(*item);
}

QVariant cs8FileBrowserModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  cs8FileItem *item = getItem(index);

  switch (role) {
  case Qt::CheckStateRole:
    switch (index.column()) {
    case 0:
      return item->checkState();
      break;
    default:
      return QVariant();
      break;
    }
    break;

  case Qt::DisplayRole:
    switch (index.column()) {
    case 0:
      return item->fileName();
      break;
    case 1:
      return item->fileSize();
      break;
    case 2:
      return item->isDir();
      break;
    default:
      return QVariant();
      break;
    }
    break;
  }

  return QVariant();
}

Qt::ItemFlags cs8FileBrowserModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return 0;

  switch (index.column()) {
  case 0:
    return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
    break;
  }

  return Qt::ItemIsEnabled;
}

cs8FileItem *cs8FileBrowserModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    cs8FileItem *item = static_cast<cs8FileItem *>(index.internalPointer());
    if (item)
      return item;
    else
      return rootItem;
  }
  return rootItem;
}

/*
QString cs8FileBrowserModel::fileName(cs8FileItem *item) const {
  return item->filePath().remove(m_backend->getUrl().toString());
}
*/

cs8AbstractBrowser *cs8FileBrowserModel::backend() const { return m_backend; }

QUrl cs8FileBrowserModel::url() const { return m_backend->url(); }

QVariant cs8FileBrowserModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

QModelIndex cs8FileBrowserModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  cs8FileItem *parentItem = getItem(parent);
  cs8FileItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

bool cs8FileBrowserModel::insertColumns(int position, int columns, const QModelIndex &parent) {
  bool success;

  beginInsertColumns(parent, position, position + columns - 1);
  success = rootItem->insertColumns(position, columns);
  endInsertColumns();

  return success;
}

bool cs8FileBrowserModel::insertRows(int position, int rows, const QModelIndex &parent) {
  cs8FileItem *parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, rootItem->columnCount());
  endInsertRows();

  return success;
}

QModelIndex cs8FileBrowserModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  cs8FileItem *childItem = getItem(index);
  cs8FileItem *parentItem = childItem->parentItem();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool cs8FileBrowserModel::removeColumns(int position, int columns, const QModelIndex &parent) {
  bool success;

  beginRemoveColumns(parent, position, position + columns - 1);
  success = rootItem->removeColumns(position, columns);
  endRemoveColumns();

  if (rootItem->columnCount() == 0)
    removeRows(0, rowCount());

  return success;
}

bool cs8FileBrowserModel::removeRows(int position, int rows, const QModelIndex &parent) {
  cs8FileItem *parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

int cs8FileBrowserModel::rowCount(const QModelIndex &parent) const {
  cs8FileItem *parentItem = getItem(parent);
  // qDebug() << __FUNCTION__ << "Child count of " << parentItem->data(0) << ":" << parentItem->childCount();
  return parentItem->childCount();
}

bool cs8FileBrowserModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (!index.isValid())
    return false;
  // qDebug() << __FUNCTION__ << index << ":" << value << ":" << role;
  cs8FileItem *item = getItem(index);

  if (item->isDir() && !item->childsEnumerated())
    enumerateChildren(index);

  bool result = true;

  if (role == Qt::CheckStateRole) {
    // qDebug() << __FUNCTION__ << index << ":" << value.value<Qt::CheckState>();
    item->setChecked(value.value<Qt::CheckState>());
    //
    // check/uncheck child items if this item is being checked/unchecked
    if (item->hasChildren() && (value.value<Qt::CheckState>() != Qt::PartiallyChecked)) {
      foreach (auto item, item->childItems()) { item->setChecked(value.value<Qt::CheckState>()); }
      emit dataChanged(this->index(0, 0, index), this->index(item->childCount() - 1, 0, index));
    }
    //
    // update parent item to tristate state if required
    if (item->hasSiblings()) {
      // all childs of parent are checked ==> check parent, too
      if (item->parentItem()->allChildsCheckedState(Qt::Checked))
        setData(index.parent(), Qt::Checked, Qt::CheckStateRole);
      // all childs of parent are unchecked ==> uncheck parent, too
      else if (item->parentItem()->allChildsCheckedState(Qt::Unchecked))
        setData(index.parent(), Qt::Unchecked, Qt::CheckStateRole);
      else
        // childs of parent are checked and unchecked ==> set partially checked for parent
        setData(index.parent(), Qt::PartiallyChecked, Qt::CheckStateRole);
    }
  } else
    result = item->setData(index.column(), value);

  emit dataChanged(index, index);

  return result;
}

bool cs8FileBrowserModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) {
  if (role != Qt::EditRole || orientation != Qt::Horizontal)
    return false;

  bool result = rootItem->setData(section, value);

  if (result)
    emit headerDataChanged(orientation, section, section);

  return result;
}

bool cs8FileBrowserModel::hasChildren(const QModelIndex &parent) const {
  cs8FileItem *item = getItem(parent);
  if (item != nullptr) {
    if (item->isDir() && !item->childsEnumerated()) {
      return true;
    }
  }
  return rowCount(parent) > 0;
}

void cs8FileBrowserModel::setupModelData(const QStringList &lines, cs8FileItem *parent) {
  QList<cs8FileItem *> parents;
  QList<int> indentations;
  parents << parent;
  indentations << 0;

  int number = 0;

  while (number < lines.count()) {
    int position = 0;
    while (position < lines[number].length()) {
      if (lines[number].at(position) != ' ')
        break;
      ++position;
    }

    QString lineData = lines[number].mid(position).trimmed();

    if (!lineData.isEmpty()) {
      // Read the column data from the rest of the line.
      QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
      QVector<QVariant> columnData;
      for (int column = 0; column < columnStrings.count(); ++column)
        columnData << columnStrings[column];

      if (position > indentations.last()) {
        // The last child of the current parent is now the new parent
        // unless the current parent has no children.

        if (parents.last()->childCount() > 0) {
          parents << parents.last()->child(parents.last()->childCount() - 1);
          indentations << position;
        }
      } else {
        while (position < indentations.last() && parents.count() > 0) {
          parents.pop_back();
          indentations.pop_back();
        }
      }

      // Append a new item to the current parent's list of children.
      cs8FileItem *parent = parents.last();
      parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
      for (int column = 0; column < columnData.size(); ++column)
        parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
    }

    ++number;
  }
}
