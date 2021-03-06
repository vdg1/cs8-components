#ifndef CS8GLOBALVARIABLEMODEL_H
#define CS8GLOBALVARIABLEMODEL_H

#include "cs8variable.h"
#include "cs8variablemodel.h"

class cs8GlobalVariableModel : public cs8VariableModel {
  Q_OBJECT
public:
  explicit cs8GlobalVariableModel(QObject *parent = 0);
  void writeXMLStream(QXmlStreamWriter &stream);
  using cs8VariableModel::addVariable;
  bool addVariable(QDomElement &element,
                   const QString &description = QString());

signals:

public slots:
};

#endif // CS8GLOBALVARIABLEMODEL_H
