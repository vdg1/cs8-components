#include "cs8globalvariablemodel.h"
#include "cs8application.h"

cs8GlobalVariableModel::cs8GlobalVariableModel(cs8Application *application,
                                               QObject *parent)
    : cs8VariableModel(application, parent, cs8VariableModel::Global) {}

void cs8GlobalVariableModel::writeXMLStream(QXmlStreamWriter &stream) {
  stream.writeStartElement("Datas");

  for (const auto item : variableListByType()) {
    item->writeXMLStream(stream);
  }
  stream.writeEndElement();
}

bool cs8GlobalVariableModel::addVariable(QDomElement &element,
                                         const QString &description) {
  auto *variable = new cs8Variable(element, description, this);
  // variable->setScope(cs8Variable::Global);
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
