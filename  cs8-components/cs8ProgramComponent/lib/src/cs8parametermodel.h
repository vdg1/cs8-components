#ifndef CS8PARAMETERMODEL_H
#define CS8PARAMETERMODEL_H
//
#include "cs8variablemodel.h"
//
class cs8ParameterModel : public cs8VariableModel
{

public:
	QVariant data(const QModelIndex & index, int role=Qt::DisplayRole) const;
	cs8ParameterModel(QObject * parent=0);
	QVariant headerData ( int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const;
	QString toString();

};
#endif
