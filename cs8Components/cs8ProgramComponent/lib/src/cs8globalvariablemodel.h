#ifndef CS8GLOBALVARIABLEMODEL_H
#define CS8GLOBALVARIABLEMODEL_H

#include "cs8variablemodel.h"

class cs8GlobalVariableModel : public cs8VariableModel {
  Q_OBJECT
public:
  explicit cs8GlobalVariableModel(QObject *parent = 0);
  void writeXMLStream(QXmlStreamWriter &stream);
signals:

public slots:
};

#endif // CS8GLOBALVARIABLEMODEL_H
