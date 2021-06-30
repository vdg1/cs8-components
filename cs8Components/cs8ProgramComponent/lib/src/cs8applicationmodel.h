#ifndef CS8APPLICATIONMODEL_H
#define CS8APPLICATIONMODEL_H

#include <QAbstractListModel>

#include "cs8application.h"

class cs8ApplicationModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit cs8ApplicationModel(QObject *parent = nullptr);

  // Header:
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  // Add data:
  bool insertRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  // Remove data:
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  cs8Application *openApplication(const QString &filePath);

private:
  QMap<QString, cs8Application *> m_applications;
};

#endif // CS8APPLICATIONMODEL_H
