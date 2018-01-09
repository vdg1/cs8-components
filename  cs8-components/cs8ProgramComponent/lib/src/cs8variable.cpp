#include "cs8variable.h"
#include <QStringList>
//
cs8Variable::cs8Variable(QDomElement &element, const QString &description)
    : QObject() {
  m_element = element;
  m_description = description;
  setBuildInTypes();
  setGlobal(false);
}

cs8Variable::cs8Variable(cs8Variable *var) {
  m_docFragment = m_doc.createDocumentFragment();
  m_element = var->element().cloneNode(true).toElement();
  m_docFragment.appendChild(m_element);
  setBuildInTypes();
}

void cs8Variable::setBuildInTypes() {
  m_buildInTypes << "pointRx"
                 << "pointRs"
                 << "mdesc"
                 << "bool"
                 << "string"
                 << "tool"
                 << "jointRx"
                 << "jointRs"
                 << "frame"
                 << "dio"
                 << "sio"
                 << "aio"
                 << "num"
                 << "config"
                 << "trsf";
}

cs8Variable::cs8Variable() : QObject() {
  m_docFragment = m_doc.createDocumentFragment();
  m_element = m_doc.createElement("Data");
  m_docFragment.appendChild(m_element);
  setBuildInTypes();
  setGlobal(false);
}

QString cs8Variable::toString(bool withTypeDefinition) {
  if (withTypeDefinition)
    return QString("%1 %2%3")
        .arg(type())
        .arg(use() == "reference" ? "& " : "")
        .arg(name());
  else
    return QString("%1").arg(name());
}

QString cs8Variable::documentation(bool withPrefix, bool forCOutput) {
  QString out;
  QString prefix = withPrefix ? "///" : "";
  if (m_description.isEmpty())
    return QString();

  QStringList list = m_description.split("\n");

  // qDebug() << "documentation(): " << m_name << ":" << m_description;
  bool inCodeSection = false;
  bool firstLine = true;
  foreach (QString str, list) {
    if (str.contains("<code>")) {
      inCodeSection = true;
      out += prefix + "<br>\n";
    }
    if (str.contains("</code>")) {
      inCodeSection = false;
      out += prefix + "<br>\n";
    }
    out +=
        (firstLine ? "" : prefix) + str + (inCodeSection ? "<br>" : "") + "\n";
    firstLine = false;
  }
  // qDebug() << "processed: " << out;
  if (!isGlobal())
    return prefix + (forCOutput ? "@param " : "\\param ") + name() + " " + out +
           "\n";
  else
    return prefix + out;
}

bool cs8Variable::isPublic() const {
  return m_element.attribute("access", "private") == "private" ? false : true;
}

QStringList cs8Variable::father() {
  QStringList list;
  QDomNode element = m_element.firstChildElement("Value");
  while (!element.isNull()) {
    QString father = element.toElement().attribute("fatherId");
    if (!father.isEmpty()) {
      father.remove(QRegExp("\\[(\\d*)\\]"));
      list << father;
    }
    element = element.nextSiblingElement("Value");
  }
  list.removeDuplicates();
  return list;
}

void cs8Variable::setPublic(bool m_public) {
  emit modified();
  m_element.setAttribute("access", m_public ? "public" : "private");
}

QDomNodeList cs8Variable::values() { return m_element.childNodes(); }

QString cs8Variable::definition() {

  return (QString("%1 %2").arg(type()).arg(name())) +
         (allSizes() != QString() ? QString("[%1]").arg(allSizes()) : "");
}

void cs8Variable::setGlobal(bool global) {
  emit modified();
  m_global = global;
  if (global) {
    m_element.setTagName("Data");
    if (!m_element.hasAttribute("xsi:type")) {
      m_element.setAttribute("xsi:type", "array");
    } else {
      if (m_element.attribute("xsi:type") == "element")
        m_element.setAttribute("xsi:type", "array");
    }
    if (!m_element.hasAttribute("size")) {
      m_element.setAttribute("size", "1");
    }
    m_element.removeAttribute("use");
    if (m_element.attribute("xsi:type") == "collection" &&
        m_element.hasAttribute("size")) {
      m_element.removeAttribute("size");
    }
    if (!m_element.hasAttribute("access")) {
      m_element.setAttribute("access", "private");
    }
  }
}

bool cs8Variable::isGlobal() const {
  // qDebug() << "var: " << m_name << " has " << m_values.count();
  return m_global; // m_values.count() > 0;
}

QString cs8Variable::allSizes() {
  return m_element.attribute("size").replace(" ", ", ");
}

void cs8Variable::setAllSizes(const QString &sizes) {
  emit modified();
  m_element.setAttribute("size", sizes);
}

QDomElement cs8Variable::element() const { return m_element; }

QVariant cs8Variable::varValue(QString index) {
  QDomElement e;
  for (int i = 0; i < m_element.childNodes().count(); i++) {
    if (m_element.childNodes().at(i).toElement().attribute("key") == index) {
      return m_element.childNodes().at(i).toElement().attribute("value");
    }
  }

  // return empty variable
  if (type() == "num")
    return QVariant(0);
  else if (type() == "string")
    return QVariant("");
  else
    return QVariant();
}

void cs8Variable::setValue(const QString &index,
                           const QMap<QString, QString> &valueMap) {
  QDomElement valueElement;
  // check if key already exist
  for (int i = 0; i < m_element.childNodes().count(); i++) {
    if (m_element.childNodes().at(i).toElement().attribute("key") == index) {
      valueElement = m_element.childNodes().at(i).toElement();
    }
  }
  // key does not exist yet
  if (!valueElement.hasAttribute("key")) {
    valueElement = m_doc.createElement("Value");
    m_element.appendChild(valueElement);
    valueElement.setAttribute("key", index);
  }
  // set value elements
  QMapIterator<QString, QString> i(valueMap);
  while (i.hasNext()) {
    i.next();
    valueElement.setAttribute(i.key(), i.value());
  }
  // adjust array size
  QStringList currIndexes = m_element.attribute("size").split(",");
  QStringList valueIndex = index.split(",");
  for (int i = 0; i < currIndexes.size(); i++) {
    if (valueIndex.at(i).toInt() > currIndexes.at(i).toInt())
      currIndexes[i] = valueIndex.at(i);
  }
  m_element.setAttribute("size", currIndexes.join(","));
}

bool cs8Variable::isBuildInType() const {
  // check if variable type is a build in type
  QStringList types = m_buildInTypes;
  QString type_ = type();
  return types.contains(type_);
}

QStringList cs8Variable::buildInTypes() { return m_buildInTypes; }

bool cs8Variable::hasConstPrefix(QString *prefix) const {
  QRegExp rx;
  rx.setPattern("([A-Z]+)(_[A-Z0-9]*)");
  bool result = rx.indexIn(name()) == 0;
  qDebug() << rx.cap(1);
  if (prefix != 0)
    *prefix = rx.cap(1);
  return result;
}

void cs8Variable::setUse(QString value) {
  emit modified();
  m_element.setAttribute("use", value);
}

QString cs8Variable::use() const {
  return m_element.attribute("use", "reference");
}

void cs8Variable::setDescription(QString value) {
  emit modified();
  m_description = value;
}

QString cs8Variable::description() const { return m_description; }

uint cs8Variable::size(int dimension) {
  QStringList list = m_element.attribute("size", "").split(" ");
  if (list.count() >= dimension)
    return list.at(dimension).toUInt();
  else
    return 0;
}

QString cs8Variable::dimension() const {
  return m_element.attribute("size", "");
}

uint cs8Variable::dimensionCount() const {
  return m_element.attribute("size", "").count(",") + 1;
}

void cs8Variable::setDimension(const QString &dim) {
  if (m_element.hasAttribute("size"))
    m_element.setAttribute("size", dim);
}

void cs8Variable::setType(QString value) {
  emit modified();
  m_element.setAttribute("type", value);
}

QString cs8Variable::type() const { return m_element.attribute("type"); }

QChar cs8Variable::prefix() const {
  QString typeName = type();
  if (typeName == "tool")
    return 't';
  else if (typeName.startsWith("point"))
    return 'p';
  else if (typeName.startsWith("frame"))
    return 'f';
  else if (typeName.startsWith("joint"))
    return 'j';
  else if (typeName.startsWith("bool"))
    return 'b';
  else if (typeName.startsWith("mdesc"))
    return 'm';
  else if (typeName.startsWith("num"))
    return 'n';
  else if (typeName.startsWith("string"))
    return 's';
  else
    return QChar();
}

bool cs8Variable::isConst() const {
  QRegExp rx;
  rx.setPattern("([A-Z]+)(_[A-Z0-9]*)");
  return rx.indexIn(name()) == 0;
}

void cs8Variable::setName(QString value) {
  emit modified();
  m_element.setAttribute("name", value);
}

QString cs8Variable::name() const { return m_element.attribute("name"); }

//
