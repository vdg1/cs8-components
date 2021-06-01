#include "cs8variable.h"
#include "cs8application.h"
#include "cs8variablemodel.h"
#include <QStringList>
//
cs8Variable::cs8Variable(QDomElement &element, const QString &description,
                         QObject *parent)
    : QObject(parent), cs8ReferencesAndLinter() {
  m_element = element;
  m_description = description;
  m_buildInTypes = setBuildInVariableTypes();
  m_name = name();
  // qDebug() << definition() << valuesToString();
}

cs8Variable::cs8Variable(QObject *parent)
    : QObject(parent), cs8ReferencesAndLinter() {
  m_docFragment = m_doc.createDocumentFragment();
  m_element = m_doc.createElement("Data");
  m_docFragment.appendChild(m_element);
  m_buildInTypes = setBuildInVariableTypes();
  setScope(Local);
}

cs8Variable::cs8Variable(cs8Variable *var, QObject *parent) : QObject(parent) {

  m_docFragment = m_doc.createDocumentFragment();
  m_element = var->element().cloneNode(true).toElement();
  m_docFragment.appendChild(m_element);
  m_buildInTypes = setBuildInVariableTypes();
  m_name = name();
}

QStringList cs8Variable::setBuildInVariableTypes() {
  return QStringList() << "aio"
                       << "bool"
                       << "configRs"
                       << "configRx"
                       << "dio"
                       << "frame"
                       << "jointRs"
                       << "jointRx"
                       << "mdesc"
                       << "num"
                       << "pointRs"
                       << "pointRx"
                       << "point"
                       << "sio"
                       << "string"
                       << "tool"
                       << "trsf";
}

QString cs8Variable::toString(bool withTypeDefinition) {
  if (withTypeDefinition)
    return QString("%1 %2%3").arg(type(), (use() == "reference" ? "& " : ""),
                                  name());
  else
    return QString("%1").arg(name());
}

QString cs8Variable::documentation(bool withPrefix, bool forCOutput) const {
  QString out;
  QString prefix = withPrefix ? "///" : "";
  if (m_description.isEmpty())
    return QString();

  QStringList list = m_description.split("\n");

  // qDebug() << "documentation(): " << m_name << ":" << m_description;
  bool inCodeSection = false;
  bool firstLine = true;
  foreach (QString str, list) {
    if (str.contains("<pre>")) {
      inCodeSection = true;
      out += prefix + "<br>\n";
    }
    if (str.contains("</pre>")) {
      inCodeSection = false;
      out += prefix + "<br>\n";
    }
    out +=
        (firstLine ? "" : prefix) + str + (inCodeSection ? "<br>" : "") + "\n";
    firstLine = false;
  }
  // qDebug() << "processed: " << out;
  if (scope() == Parameter)
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
      father.remove(QRegExp(R"(\[(\d*)\])"));
      list << father;
    }
    element = element.nextSiblingElement("Value");
  }
  list.removeDuplicates();
  return list;
}

void cs8Variable::setValues(const QDomNodeList &values) {
  while (m_element.hasChildNodes())
    m_element.removeChild(m_element.firstChild());

  for (int i = 0; i < values.count(); i++)
    m_element.appendChild(values.item(i));
}

void cs8Variable::setPublic(bool m_public) {
  emit modified();
  m_element.setAttribute("access", m_public ? "public" : "private");
}

QString cs8Variable::publicStr() const {
  return m_element.attribute("access", "private");
}

QDomNodeList cs8Variable::values() const { return m_element.childNodes(); }

QString cs8Variable::valuesToString() const {
  QString s;
  for (int i = 0; i < values().count(); i++) {
    s += values().item(i).toElement().attribute("key") + ":>" +
         values().item(i).toElement().attribute("value") + "<\n";
  }
  return s;
}

QString cs8Variable::definition() {

  return (QString("%1 %2").arg(type(), name())) +
         (allSizes() != QString() ? QString("[%1]").arg(allSizes()) : "");
}

void cs8Variable::setScope(DeclarationScope scope) {
  emit modified();
  switch (scope) {
  case Local:
    m_element.setTagName("Local");
    m_element.removeAttribute("use");
    if (!m_element.hasAttribute("xsi:type"))
      m_element.setAttribute("xsi:type", "array");
    if (!m_element.hasAttribute("type"))
      m_element.setAttribute("type", "num");
    if (!m_element.hasAttribute("size"))
      m_element.setAttribute("size", "1");
    break;

  case Global:
    m_element.setTagName("Data");
    m_element.removeAttribute("use");
    if (!m_element.hasAttribute("xsi:type") ||
        (m_element.attribute("xsi:type") == "element"))
      m_element.setAttribute("xsi:type", "array");
    if (!m_element.hasAttribute("type"))
      m_element.setAttribute("type", "num");
    if (!m_element.hasAttribute("size"))
      m_element.setAttribute("size", "1");
    if (!m_element.hasAttribute("access"))
      m_element.setAttribute("access", "private");
    break;

  case Parameter:
    m_element.setTagName("Parameter");
    if (!m_element.hasAttribute("xsi:type"))
      m_element.setAttribute("xsi:type", "element");
    if (!m_element.hasAttribute("type"))
      m_element.setAttribute("type", "num");
    if (!m_element.hasAttribute("use"))
      m_element.setAttribute("use", "reference");
    break;
  }
}

cs8Variable::DeclarationScope cs8Variable::scope() const {
  QString tagName = m_element.tagName();
  if (tagName == "Local")
    return Local;
  else if (tagName == "Parameter")
    return Parameter;
  else if (tagName == "Data")
    return Global;
  return Local;
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

QStringList cs8Variable::buildInTypes() { return setBuildInVariableTypes(); }

bool cs8Variable::hasConstPrefix(QString *prefix) const {
  QRegExp rx;
  rx.setPattern("([A-Z]+)(_[A-Z0-9_]*)");
  bool result = rx.indexIn(name()) == 0;
  // qDebug() << rx.cap(1);
  if (prefix != nullptr)
    *prefix = rx.cap(1);
  return result;
}

void cs8Variable::extractArrayIndex(const QString &value, QString &name,
                                    QString &index) {
  if (value.indexOf("[") > -1) {
    name = value.mid(0, value.indexOf("["));
    index =
        value.mid(value.indexOf("[") + 1, value.length() - value.indexOf("]"));
  } else {
    name = value;
    index = "0";
  }
}

void cs8Variable::writeValueElements(QXmlStreamWriter &stream) {

  writeNodes(stream, m_element.childNodes());
}

void cs8Variable::writeNodes(QXmlStreamWriter &stream, QDomNodeList nodes) {
  qDebug() << __FUNCTION__ << "Write values of " << name();
  for (int i = 0; i < nodes.count(); i++) {
    QDomElement valueElement = nodes.at(i).toElement();
    QStringList attributeList;
    QDomNamedNodeMap valueAttributes = valueElement.attributes();
    for (int i = 0; i < valueAttributes.count(); i++)
      attributeList << valueAttributes.item(i).nodeName();

    QStringList attributeOrder = QStringList() << "key"
                                               << "name"
                                               << "value"
                                               << "x"
                                               << "y"
                                               << "z"
                                               << "rx"
                                               << "ry"
                                               << "rz"
                                               << "j1"
                                               << "j2"
                                               << "j3"
                                               << "j4"
                                               << "j5"
                                               << "j6"
                                               << "accel"
                                               << "vel"
                                               << "decel"
                                               << "leave"
                                               << "reach"
                                               << "xsi:type"
                                               << "type"
                                               << "size"
                                               << "shoulder"
                                               << "elbow"
                                               << "wrist"
                                               << "fatherId";

    stream.writeStartElement(valueElement.tagName());
    // qDebug() << "attributes before: " << valueElement.attributes().count()
    //         << attributeList;
    // write attributes in pre defined order
    for (const auto &attr : qAsConst(attributeOrder)) {
      if (attributeList.contains(attr)) {
        QDomNode attribute = valueAttributes.namedItem(attr);
        // qDebug() << "write ordered attribute" << attribute.nodeName();
        stream.writeAttribute(attribute.nodeName(), attribute.nodeValue());
        attributeList.removeAll(attr);
      }
    }
    // write remaining attributes
    while (attributeList.length() > 0) {
      QDomNode attribute = valueAttributes.namedItem(attributeList.first());
      Q_ASSERT(attribute.isAttr());
      // qDebug() << "write remaining attribute" << attribute.nodeName();
      stream.writeAttribute(attribute.nodeName(), attribute.nodeValue());
      attributeList.removeFirst();
    }
    // write fields
    if (valueElement.childNodes().count() > 0) {
      writeNodes(stream, valueElement.childNodes());
    }
    stream.writeEndElement();
    // qDebug() << "attributes after : " << valueElement.attributes().count();
  }
}

void cs8Variable::writeXMLStream(QXmlStreamWriter &stream) {
  switch (scope()) {
  case Parameter:
    stream.writeEmptyElement("Parameter");
    stream.writeAttribute("name", name());
    stream.writeAttribute("type", type());
    stream.writeAttribute("xsi:type", xsiType());
    if (use() == "reference")
      stream.writeAttribute("use", use());
    if (dimensionCount() > 1)
      stream.writeAttribute("dimensions", QString("%1").arg(dimensionCount()));
    break;

  case Local:
    stream.writeEmptyElement("Local");
    stream.writeAttribute("name", name());
    stream.writeAttribute("type", type());
    stream.writeAttribute("xsi:type", xsiType());
    if (xsiType() != "collection")
      stream.writeAttribute("size", dimension());
    break;

  case Global:
    stream.writeStartElement("Data");
    stream.writeAttribute("name", name());
    stream.writeAttribute("access", publicStr());
    stream.writeAttribute("xsi:type", xsiType());
    stream.writeAttribute("type", type());
    if (xsiType() != "collection")
      stream.writeAttribute("size", dimension());
    writeValueElements(stream);
    stream.writeEndElement();
    break;
  }
}

void cs8Variable::setUse(QString value) {
  emit modified();
  m_element.setAttribute("use", value);
}

QString cs8Variable::use() const { return m_element.attribute("use", "value"); }

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

int cs8Variable::dimensionCount() const {
  switch (scope()) {
  case Local:
  case Global:
    return m_element.attribute("size", "").count(",") + 1;
    break;
  case Parameter:
    return m_element.attribute("dimensions", "1").toInt();
    break;
  }

  return 1;
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
    return {};
}

bool cs8Variable::isConst() const {
  QRegExp rx;
  rx.setPattern("([A-Z]+)(_[A-Z0-9]*)");
  return rx.indexIn(name()) == 0;
}

void cs8Variable::setXsiType(const QString &type) {
  m_element.setAttribute("xsi:type", type);
}

QString cs8Variable::xsiType() const {
  return m_element.attribute("xsi:type", "array");
}

bool cs8Variable::setName(QString value, cs8Application *application) {
  cs8VariableModel *m = qobject_cast<cs8VariableModel *>(parent());
  if (m_name == value)
    return true;
  else if (m->getVarByName(value) != nullptr)
    return false;
  else {
    emit modified();
    QString oldName = m_name;
    m_name = value;
    m_element.setAttribute("name", value);
    if (application) {
      updateReference(application, oldName, value);
      application->programModel()->updateCodeModel();
    }
  }
  return true;
}

QString cs8Variable::name() const { return m_element.attribute("name"); }

//
