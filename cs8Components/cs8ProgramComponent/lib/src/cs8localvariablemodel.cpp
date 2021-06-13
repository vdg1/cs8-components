#include "cs8localvariablemodel.h"
#include "cs8application.h"

cs8LocalVariableModel::cs8LocalVariableModel(cs8Application *application,
                                             QObject *parent)
    : cs8VariableModel(application, parent, cs8VariableModel::Local) {}
