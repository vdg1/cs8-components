#ifndef CS8FILEBROWSERMODEL_H
#define CS8FILEBROWSERMODEL_H

#include "cs8abstractbrowser.h"
#include "cs8fileitem.h"
#include <QAbstractItemModel>

class cs8FileBrowserModel : public QAbstractItemModel {
  Q_OBJECT
public:
  cs8FileBrowserModel(const QUrl &url, QObject *parent = 0);

  ~cs8FileBrowserModel();

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  bool canFetchMore(const QModelIndex &parent) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
  bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

  bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
  bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
  bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

  cs8AbstractBrowser *backend() const;
  QUrl url() const;

public slots:
  void fillModel();

protected slots:
  void enumerateChildren(const QModelIndex &parentItem);

private:
  void setupModelData(const QStringList &lines, cs8FileItem *parent);
  cs8FileItem *getItem(const QModelIndex &index) const;
  // QString fileName(cs8FileItem *item) const;

  cs8FileItem *rootItem;
  cs8FileItem *profilesNode;
  cs8FileItem *logNode;
  cs8FileItem *appNode;
  cs8AbstractBrowser *m_backend;
};

#endif // CS8FILEBROWSERMODEL_H
