#include "cs8parametermodel.h"
#include "cs8variable.h"
//
cs8ParameterModel::cs8ParameterModel ( QObject * parent )
    : cs8VariableModel ( parent, true )
{
    // TODO
}
//
QVariant cs8ParameterModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        switch ( section )
            {
        case 0:
                return tr ( "Name" );
                break;

            case 1:
                return tr ( "Type" );
                break;

            case 2:
                return tr ( "By Value" );
                break;

            case 3:
                return tr ( "Description" );
                break;

            }

    return QVariant();
}

QVariant cs8ParameterModel::data ( const QModelIndex & index, int role ) const
{
    cs8Variable* variable=m_variableList.at ( index.row() );
    if ( role == Qt::DisplayRole )
        {
            if ( index.column() ==2 && index.isValid() )
                return variable->use();
        }
    return cs8VariableModel::data ( index,role );
}

QString cs8ParameterModel::toString()
{
    QString line;
    for (int idx=0;idx<rowCount(QModelIndex());idx++)
        {
            line+=m_variableList.at(idx)->toString()+", ";
        }
    line.chop(2);
    return line;
}

