#ifndef CS8LOCALVARIABLEMODEL_H
#define CS8LOCALVARIABLEMODEL_H

#include "cs8variablemodel.h"

class cs8Application;
class cs8LocalVariableModel : public cs8VariableModel {
  Q_OBJECT
public:
  explicit cs8LocalVariableModel(cs8Application *application,
                                 QObject *parent = 0);

signals:

public slots:
};

#endif // CS8LOCALVARIABLEMODEL_H
