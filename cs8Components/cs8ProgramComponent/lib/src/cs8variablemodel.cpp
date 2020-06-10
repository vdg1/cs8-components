#include "cs8variablemodel.h"
#include "cs8variable.h"
#include <QDebug>
#include <QRegularExpression>
#include <QStringList>
//
cs8VariableModel::cs8VariableModel(QObject *parent, modelType mode)
    : QAbstractTableModel(parent), m_mode(mode),
      m_withUndocumentedSymbols(false) {
  // m_variableList = new QList<cs8Variable*>;
}
//

bool cs8VariableModel::addVariable(QDomElement &element,
                                   const QString &description) {

  if (element.tagName() == "Local") {
    cs8Variable *variable = new cs8Variable(element, "", this);

    variable->setDescription(description);
    m_variableList.append(variable);
#if QT_VERSION >= 0x050000
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
  } else if (element.tagName() == "Parameter") {
    cs8Variable *variable = new cs8Variable(element, "", this);

    variable->setDescription(description);
    m_variableList.append(variable);
#if QT_VERSION >= 0x050000
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
  } else
    return false;
  return true;
}

void cs8VariableModel::addVariable(cs8Variable *variable) {
  m_variableList.append(variable);
  variable->setParent(this);
}

bool cs8VariableModel::addGlobalVariable(QDomElement &element,
                                         const QString &description) {
  auto *variable = new cs8Variable(element, description);
  connect(variable, SIGNAL(modified()), this, SLOT(slotModified()));
  variable->setScope(cs8Variable::Global);
  m_variableList.append(variable);
#if QT_VERSION >= 0x050000
  beginResetModel();
  endResetModel();
#else
  reset();
#endif
  return true;
}

QList<cs8Variable *> cs8VariableModel::publicVariables() {
  QList<cs8Variable *> out;
  foreach (cs8Variable *variable, m_variableList) {
    if (variable->isPublic())
      out << variable;
  }
  return out;
}

QList<cs8Variable *> cs8VariableModel::privateVariables() {
  QList<cs8Variable *> out;
  foreach (cs8Variable *variable, m_variableList) {
    if (!variable->isPublic())
      out << variable;
  }
  return out;
}

cs8Variable *cs8VariableModel::variable(QModelIndex index) {
  if (!index.isValid())
    return nullptr;
  return m_variableList.at(index.row());
}

QStringList cs8VariableModel::variableNameList() {
  QStringList list;
  foreach (cs8Variable *variable, m_variableList) { list << variable->name(); }
  return list;
}

QList<cs8Variable *> &cs8VariableModel::rvariableList() {
  return m_variableList;
}

QList<cs8Variable *> cs8VariableModel::variableList(const QString &type) {
  if (type.isEmpty()) {
    return m_variableList;
  } else {
    QList<cs8Variable *> list;
    foreach (cs8Variable *var, m_variableList) {
      if (var->type() == type)
        list.append(var);
    }
    return list;
  }
}

QList<cs8Variable *>
cs8VariableModel::variableList(const QRegularExpression &rx) {
  QList<cs8Variable *> list;
  foreach (cs8Variable *var, m_variableList) {
    QRegularExpressionMatch match = rx.match(var->type());
    if (match.hasMatch())
      list.append(var);
  }
  return list;
}

cs8Variable *cs8VariableModel::createVariable(const QString &name,
                                              const QString &type) {
  auto *variable = new cs8Variable(this);
  variable->setName(name);
  variable->setType(type);
  // variable->setParent(this);
  m_variableList.append(variable);
  return variable;
}

bool cs8VariableModel::hasDocumentation() {
  foreach (cs8Variable *var, m_variableList) {
    if (!var->documentation(true, false).isEmpty())
      return true;
  }
  return false;
}

QString cs8VariableModel::toDtxDocument() {
  QDomDocument doc;

  QDomProcessingInstruction process = doc.createProcessingInstruction(
      "xml", R"(version="1.0" encoding="utf-8")");
  doc.appendChild(process);
  QDomElement database = doc.createElement("Database");
  doc.appendChild(database);
  database.setAttribute("xmlns", "http://www.staubli.com/robotics/VAL3/Data/2");
  database.setAttribute("xmlns:xsi",
                        "http://www.w3.org/2001/XMLSchema-instance");

  QDomElement dataElement = doc.createElement("Datas");
  database.appendChild(dataElement);

  foreach (cs8Variable *variable, m_variableList) {

    QDomNode element = variable->element().cloneNode(true);
    qDebug() << "Variable " << element.toElement().attribute("name");
    if (dataElement.appendChild(element).isNull())
      qDebug() << "Failed to append node";
  }

  QString out;

  out = doc.toString();
  qDebug() << out;
  return out;
}

QList<cs8Variable *> cs8VariableModel::findVariablesByType(const QString &type_,
                                                           bool public_) {
  QList<cs8Variable *> list;
  foreach (cs8Variable *variable, m_variableList) {
    if (variable->type() == type_ && variable->isPublic() == public_)
      list.append(variable);
  }
  return list;
}

cs8Variable *cs8VariableModel::findVariableByName(const QString &name_) {
  for (auto var : m_variableList) {
    if (var->name() == name_)
      return var;
  }
  return Q_NULLPTR;
}

int cs8VariableModel::rowCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return m_variableList.count();
}

int cs8VariableModel::columnCount(const QModelIndex &index) const {
  Q_UNUSED(index);
  return 4;
}

void cs8VariableModel::clear() { m_variableList.clear(); }

QVariant cs8VariableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int row = index.row();
  int column = index.column();
  QVariant ret;
  if (role == Qt::DisplayRole || role == Qt::EditRole) {

    cs8Variable *variable = m_variableList.at(row);
    switch (column) {
    case 0:
      ret = variable->name();
      break;

    case 1:
      ret = variable->type();
      break;

    case 2:
      ret = variable->size();
      break;

    case 3:
      ret = variable->description();
      break;
    }
  }

  return ret;
}

QVariant cs8VariableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    switch (section) {
    case 0:
      return tr("Name");
      break;

    case 1:
      return tr("Type");
      break;

    case 2:
      return tr("Size");
      break;

    case 3:
      return tr("Description");
      break;
    }

  return QVariant();
}

/*!
   \fn cs8VariableModel::flags ( const QModelIndex & index ) const
 */
Qt::ItemFlags cs8VariableModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return nullptr;

  if (index.column() == 3)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/*!
   \fn cs8VariableModel::setData ( const QModelIndex & index, const QVariant &
   value, int role = Qt::EditRole )
 */
bool cs8VariableModel::setData(const QModelIndex &index, const QVariant &value,
                               int role) {
  if (role == Qt::EditRole) {
    if (index.column() == 3) {
      cs8Variable *variable = m_variableList.at(index.row());
      if (variable->description() != value.toString())
        emit documentationChanged(value.toString());
      variable->setDescription(value.toString());
      emit(dataChanged(index, index));
    }
  }
  return true;
}

/*!
   \fn cs8VariableModel::getVarByName(const QString & name)
 */
cs8Variable *cs8VariableModel::getVarByName(const QString &name) {
  foreach (cs8Variable *var, m_variableList) {
    QString n = name;
    n.remove(QRegExp("\\[.*\\]"));
    if (var->name().compare(n) == 0)
      return var;
  }
  return nullptr;
}

QDomNode cs8VariableModel::document(QDomDocument &doc) {
  QDomDocumentFragment fragment = doc.createDocumentFragment();
  QDomElement section =
      doc.createElement(m_mode == Parameter ? "paramSection" : "localSection");
  fragment.appendChild(section);
  foreach (cs8Variable *var, m_variableList) {
    QDomElement varElement = doc.createElement(m_mode ? "param" : "local");
    if (m_mode == Parameter)
      varElement.setAttribute("byVal", var->use());
    else
      varElement.setAttribute("size", var->size());

    varElement.setAttribute("type", var->type());
    varElement.setAttribute("name", var->name());
    section.appendChild(varElement);
  }
  return fragment;
}

QString cs8VariableModel::toDocumentedCode() {
  QString header;

  QString prefix;
  switch (m_mode) {
  case Parameter:
    prefix = "param";
    break;

  case Local:
    prefix = "var";
    break;

  case Global:
    prefix = "global";
    break;

  case ReferencedGlobal:
    prefix = "refGlobal";
    break;
  }

  foreach (cs8Variable *var, m_variableList) {
    QString descr = var->description();
    if (!descr.isEmpty() || m_withUndocumentedSymbols)
      header += QString("\n\\%1 %3 %2\n").arg(prefix).arg(descr, var->name());
  }
  qDebug() << "header: " << header;
  return header;
}

void cs8VariableModel::slotModified() { emit modified(true); }
bool cs8VariableModel::withUndocumentedSymbols() const {
  return m_withUndocumentedSymbols;
}

void cs8VariableModel::setWithUndocumentedSymbols(
    bool withUndocumentedSymbols) {
  m_withUndocumentedSymbols = withUndocumentedSymbols;
}

void cs8VariableModel::writeXMLStream(QXmlStreamWriter &stream) {
  stream.writeStartElement("Locals");
  for (auto item : variableList()) {
    item->writeXMLStream(stream);
  }
  stream.writeEndElement();
}
