#ifndef CS8VARIABLEMODEL_H
#define CS8VARIABLEMODEL_H
//
#include <QAbstractTableModel>
#include <QDomElement>
#include <QXmlStreamWriter>

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
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role) const;
  int columnCount(const QModelIndex &index) const;
  bool addVariable(QDomElement &element,
                   const QString &description = QString());
  void addVariable(cs8Variable *variable);
  cs8VariableModel(QObject *parent = 0, modelType mode = Local);
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;
  cs8Variable *getVarByName(const QString &name);

  QString toDtxDocument();
  QList<cs8Variable *> findVariablesByType(const QString &type_,
                                           bool public_ = true);
  QList<cs8Variable *>
  findVariablesReferencedByProgram(const QString &program) const;
  cs8Variable *findVariableByName(const QString &name_);
  void clear();
  QList<cs8Variable *> publicVariables();
  QList<cs8Variable *> privateVariables();
  cs8Variable *variable(QModelIndex index);
  QStringList variableNameList();
  QList<cs8Variable *> &rvariableList();
  QList<cs8Variable *>
  variableListByType(const QString &type = QString()) const;
  QList<cs8Variable *> variableListByType(const QRegularExpression &rx) const;
  QList<cs8Variable *> variableListByName(const QRegExp &rx) const;

  cs8Variable *createVariable(const QString &name, const QString &type);
  bool hasDocumentation();

  bool withUndocumentedSymbols() const;
  void setWithUndocumentedSymbols(bool withUndocumentedSymbols);

  void writeXMLStream(QXmlStreamWriter &stream);

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
