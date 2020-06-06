#include "cs8variable.h"
#include <QStringList>
//
cs8Variable::cs8Variable(QDomElement &element, const QString &description,
                         QObject *parent)
    : QObject(parent) {
  m_element = element;
  m_description = description;
  m_buildInTypes = setBuildInVariableTypes();
}

cs8Variable::cs8Variable(QObject *parent) : QObject(parent) {
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
}

QStringList cs8Variable::setBuildInVariableTypes(bool val3S6Format) {
  return QStringList() << "aio"
                       << "bool"
                       << "configRs" << (val3S6Format ? "config" : "configRx")
                       << "dio"
                       << "frame"
                       << "jointRs" << (val3S6Format ? "joint" : "jointRx")
                       << "mdesc"
                       << "num"
                       << "pointRs" << (val3S6Format ? "point" : "pointRx")
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
  if (!scope())
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

void cs8Variable::setPublic(bool m_public) {
  emit modified();
  m_element.setAttribute("access", m_public ? "public" : "private");
}

QDomNodeList cs8Variable::values() { return m_element.childNodes(); }

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
    if (!m_element.hasAttribute("xsi:type"))
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
  else if (tagName == "Global")
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

QDomElement cs8Variable::element(QDomDocument *doc, bool val3S6Format) const {
  if (!val3S6Format)
    return m_element;
  else {
    QDomElement element = doc->createElement("___");
    if (m_element.tagName() == "Local") {
      element.setTagName("local");
      element.setAttribute("name", m_element.attribute("name"));
      element.setAttribute("type", m_element.attribute("type"));
      element.setAttribute("size", m_element.attribute("size"));
    } else if (m_element.tagName() == "Parameter") {
      element.setTagName("param");
      element.setAttribute("name", m_element.attribute("name"));
      element.setAttribute("type", m_element.attribute("type"));
      element.setAttribute("byVal", m_element.attribute("use") == "reference"
                                        ? "false"
                                        : "true");
    } else {
      QString typeName = m_element.attribute("type");
      // replace jointRX and pointRX with point and joint
      typeName.replace("Rx", "");
      element.setTagName(typeName);
      element.setAttribute("name", m_element.attribute("name"));
      element.setAttribute("public", m_element.attribute("access") == "public"
                                         ? "true"
                                         : "false");
      // for io type do not write value elements
      if (typeName == "dio" || typeName == "aio" || typeName == "sio") {
        element.setAttribute("size", m_element.attribute("size"));
      } else {
        // identify array size
        QStringList arrayDefinition =
            m_element.attribute("size", "1").split(",");
        while (arrayDefinition.count() < 3)
          arrayDefinition.insert(0, "1");
        int totalValues = arrayDefinition.at(0).toInt() *
                          arrayDefinition.at(1).toInt() *
                          arrayDefinition.at(2).toInt();
        // element.setAttribute("size", m_element.attribute("size"));
        // write values
        for (int i = 0; i < totalValues; i++) {

          // QDomElement sourceValue = m_element.childNodes().at(i).toElement();

          QDomElement sourceValue;
          for (int id = 0; id < m_element.childNodes().count(); id++) {
            if (m_element.childNodes()
                    .at(id)
                    .toElement()
                    .attribute("key", "0")
                    .toInt() == i) {
              sourceValue = m_element.childNodes().at(id).toElement();
              break;
            }
          }

          QString str = element.tagName();
          str = str.at(0).toUpper() + str.mid(1);
          QDomElement valueElement = doc->createElement("value" + str);
          valueElement.setAttribute(
              "index", sourceValue.attribute("key", QString("%1").arg(i)));

          if (typeName == "bool") {
            valueElement.setAttribute("value",
                                      sourceValue.attribute("value", "false"));
          } else if (typeName == "joint") {
            QDomElement val = doc->createElement("jointValue");
            val.setAttribute("j1", sourceValue.attribute("j1", "0"));
            val.setAttribute("j2", sourceValue.attribute("j2", "0"));
            val.setAttribute("j3", sourceValue.attribute("j3", "0"));
            val.setAttribute("j4", sourceValue.attribute("j4", "0"));
            val.setAttribute("j5", sourceValue.attribute("j5", "0"));
            val.setAttribute("j6", sourceValue.attribute("j6", "0"));
            valueElement.appendChild(val);
          } else if (typeName == "mdesc") {
            QDomElement val = doc->createElement("mdescValue");
            val.setAttribute("accel", sourceValue.attribute("accel", "100"));
            val.setAttribute("decel", sourceValue.attribute("decel", "100"));
            val.setAttribute("vel", sourceValue.attribute("vel", "100"));
            val.setAttribute("tmax", sourceValue.attribute("tmax", "999999"));
            val.setAttribute("rmax", sourceValue.attribute("rmax", "999999"));
            val.setAttribute("blend", sourceValue.attribute("blend", "off"));
            val.setAttribute("leave", sourceValue.attribute("leave", "0"));
            val.setAttribute("reach", sourceValue.attribute("reach", "0"));
            valueElement.appendChild(val);
          } else if (typeName == "trsf") {
            QDomElement val = doc->createElement("trsfValue");
            val.setAttribute("x", sourceValue.attribute("x", "0"));
            val.setAttribute("y", sourceValue.attribute("y", "0"));
            val.setAttribute("z", sourceValue.attribute("z", "0"));
            val.setAttribute("rx", sourceValue.attribute("rx", "0"));
            val.setAttribute("ry", sourceValue.attribute("ry", "0"));
            val.setAttribute("rz", sourceValue.attribute("rz", "0"));
            valueElement.appendChild(val);
          } else if (typeName == "config") {
            QDomElement val = doc->createElement("configValue");
            val.setAttribute("shoulder",
                             sourceValue.attribute("shoulder", "ssame"));
            val.setAttribute("elbow", sourceValue.attribute("elbow", "esame"));
            val.setAttribute("wrist", sourceValue.attribute("wrist", "wsame"));
            valueElement.appendChild(val);
          } else if (typeName == "configRs") {
            QDomElement val = doc->createElement("configRsValue");
            val.setAttribute("shoulder",
                             sourceValue.attribute("shoulder", "ssame"));
            valueElement.appendChild(val);
          } else if (typeName == "num") {
            valueElement.setAttribute("value",
                                      sourceValue.attribute("value", "0"));
          } else if (typeName == "string") {
            valueElement.setAttribute("value",
                                      sourceValue.attribute("value", ""));
          } else if (typeName == "tool") {
            QDomElement fatherElement = doc->createElement("tFather");
            QString sourceFather = sourceValue.attribute("fatherId");
            QString arrayName, arrayIndex;
            extractArrayIndex(sourceFather, arrayName, arrayIndex);
            fatherElement.setAttribute("name", arrayName);
            fatherElement.setAttribute("fatherIndex", arrayIndex);
            //
            element.appendChild(fatherElement);
            QDomElement val = doc->createElement("ttValue");
            val.setAttribute("x", sourceValue.attribute("x", "0"));
            val.setAttribute("y", sourceValue.attribute("y", "0"));
            val.setAttribute("z", sourceValue.attribute("z", "0"));
            val.setAttribute("rx", sourceValue.attribute("rx", "0"));
            val.setAttribute("ry", sourceValue.attribute("ry", "0"));
            val.setAttribute("rz", sourceValue.attribute("rz", "0"));
            valueElement.appendChild(val);
            val = doc->createElement("io");
            val.setAttribute("alias", "io");
            val.setAttribute("ioIndex", "0");
            val.setAttribute("name", sourceValue.attribute("ioLink", "valve1"));
            val.setAttribute("open", sourceValue.attribute("open", "0"));
            val.setAttribute("close", sourceValue.attribute("close", "0"));
            valueElement.appendChild(val);
          } else if (typeName == "point") {
            QDomElement fatherElement = doc->createElement("pFather");
            QString sourceFather = sourceValue.attribute("fatherId");
            QString arrayName, arrayIndex;
            extractArrayIndex(sourceFather, arrayName, arrayIndex);
            fatherElement.setAttribute("name", arrayName);
            fatherElement.setAttribute("fatherIndex", arrayIndex);
            //
            element.appendChild(fatherElement);
            QDomElement val = doc->createElement("tpValue");
            val.setAttribute("x", sourceValue.attribute("x", "0"));
            val.setAttribute("y", sourceValue.attribute("y", "0"));
            val.setAttribute("z", sourceValue.attribute("z", "0"));
            val.setAttribute("rx", sourceValue.attribute("rx", "0"));
            val.setAttribute("ry", sourceValue.attribute("ry", "0"));
            val.setAttribute("rz", sourceValue.attribute("rz", "0"));
            valueElement.appendChild(val);
            val = doc->createElement("cpValue");
            val.setAttribute("shoulder",
                             sourceValue.attribute("shoulder", "ssame"));
            val.setAttribute("elbow", sourceValue.attribute("elbow", "esame"));
            val.setAttribute("wrist", sourceValue.attribute("wrist", "wsame"));
            valueElement.appendChild(val);
          } else if (typeName == "frame") {
            QDomElement fatherElement = doc->createElement("fFather");
            QString sourceFather = sourceValue.attribute("fatherId");
            QString arrayName, arrayIndex;
            extractArrayIndex(sourceFather, arrayName, arrayIndex);
            fatherElement.setAttribute("name", arrayName);
            fatherElement.setAttribute("fatherIndex", arrayIndex);
            //
            element.appendChild(fatherElement);
            QDomElement val = doc->createElement("tfValue");
            val.setAttribute("x", sourceValue.attribute("x", "0"));
            val.setAttribute("y", sourceValue.attribute("y", "0"));
            val.setAttribute("z", sourceValue.attribute("z", "0"));
            val.setAttribute("rx", sourceValue.attribute("rx", "0"));
            val.setAttribute("ry", sourceValue.attribute("ry", "0"));
            val.setAttribute("rz", sourceValue.attribute("rz", "0"));
            valueElement.appendChild(val);
          }
          element.appendChild(valueElement);
        }
      }
    }
    return element;
  }
}

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

QStringList cs8Variable::buildInTypes(bool val3S6Format) {
  return setBuildInVariableTypes(val3S6Format);
}

bool cs8Variable::hasConstPrefix(QString *prefix) const {
  QRegExp rx;
  rx.setPattern("([A-Z]+)(_[A-Z0-9]*)");
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

void cs8Variable::setName(QString value) {
  emit modified();
  m_element.setAttribute("name", value);
}

QString cs8Variable::name() const { return m_element.attribute("name"); }

//
