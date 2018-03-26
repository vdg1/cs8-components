#ifndef CS8VARIABLEMODEL_H
#define CS8VARIABLEMODEL_H
//
#include <QAbstractTableModel>
#include <QDomElement>

class cs8Variable;
//
class cs8VariableModel : public QAbstractTableModel {
  Q_OBJECT

protected:
  QList<cs8Variable *> m_variableList;

public:
  enum modelType { Global, Local, Parameter, ReferencedGlobal };

  QString toDocumentedCode();
  QDomNode document(QDomDocument &doc);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role) const;
  int columnCount(const QModelIndex &index) const;
  bool addVariable(QDomElement &element, const QString &description = QString());
  void addVariable(cs8Variable *variable);
  bool addGlobalVariable(QDomElement &element, const QString &description = QString());
  cs8VariableModel(QObject *parent = 0, modelType mode = Local);
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  cs8Variable *getVarByName(const QString &name);

  QString toDtxDocument();
  QList<cs8Variable *> findVariablesByType(const QString &type_, bool public_ = true);
  void clear();
  QList<cs8Variable *> publicVariables();
  QList<cs8Variable *> privateVariables();
  cs8Variable *variable(QModelIndex index);
  QStringList variableNameList();
  QList<cs8Variable *> variableList(const QString &type = QString());

  cs8Variable *createVariable(const QString &name);
  bool hasDocumentation();

  bool withUndocumentedSymbols() const;
  void setWithUndocumentedSymbols(bool withUndocumentedSymbols);

signals:
  void modified(bool);
  void documentationChanged(const QString &documentation);

protected slots:
  void slotModified();

private:
  modelType m_mode;
  bool m_withUndocumentedSymbols;
};
#endif
