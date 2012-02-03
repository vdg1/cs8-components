#include "cs8variablemodel.h"
#include "cs8variable.h"
#include <QDebug>
#include <QStringList>
//
cs8VariableModel::cs8VariableModel(QObject * parent, bool mode) :
    QAbstractTableModel(parent), m_mode(mode) {
    //m_variableList = new QList<cs8Variable*>;
}
//

bool cs8VariableModel::addVariable(QDomElement & element,
                                   const QString & description) {

    if (element.tagName() == "Local") {
        cs8Variable * variable = new cs8Variable(element);

        variable->setDescription(description);
        m_variableList.append(variable);
        reset();
    } else if (element.tagName() == "Parameter") {
        cs8Variable * variable = new cs8Variable(element);

        variable->setDescription(description);
        m_variableList.append(variable);
        reset();
    } else
        return false;
    return true;
}

bool cs8VariableModel::addGlobalVariable(QDomElement & element,
                                         const QString & description) {
    QString name = element.attribute("name");
    QString type = element.attribute("type");
    QDomNodeList valueList = element.childNodes();
    int size = valueList.count();
    cs8Variable * variable = new cs8Variable(element);

    variable->setGlobal(true);
    // append values of global variable
    qDebug() << variable->name() << ": adding " << valueList.count() << " elements to var";

    m_variableList.append(variable);
    reset();
    return true;
}

QList<cs8Variable*> cs8VariableModel::publicVariables() {
    QList<cs8Variable*> out;
    foreach(cs8Variable* variable,m_variableList)
    {
        if (variable->isPublic())
            out << variable;
    }
    return out;
}

QList<cs8Variable*> cs8VariableModel::privateVariables() {
    QList<cs8Variable*> out;
    foreach(cs8Variable* variable,m_variableList)
    {
        if (!variable->isPublic())
            out << variable;
    }
    return out;
}

cs8Variable* cs8VariableModel::variable(QModelIndex index){
    if (!index.isValid())
        return 0;
    return m_variableList.at(index.row());
}

QStringList cs8VariableModel::variableNameList()
{
    QStringList list;
    foreach(cs8Variable *variable, m_variableList)
    {
        list << variable->name ();
    }
    return list;
}

QString cs8VariableModel::toDtxDocument() {
    QDomDocument doc;

    QDomProcessingInstruction process = doc.createProcessingInstruction("xml",
                                                                        "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(process);
    QDomElement database=doc.createElement("Database");
    doc.appendChild(database);
    database.setAttribute("xmlns","http://www.staubli.com/robotics/VAL3/Data/2");
    database.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");

    QDomElement dataElement = doc.createElement("Datas");
    database.appendChild(dataElement);

    foreach(cs8Variable *variable, m_variableList){

        QDomNode element=variable->element().cloneNode(true);
        qDebug() << "Variable " << element.toElement().attribute("name");
        if (dataElement.appendChild(element).isNull())
            qDebug() << "Failed to append node";
    }

    QString out;

    out = doc.toString();
    qDebug() << out;
    return out;
}

QList<cs8Variable*> cs8VariableModel::findVariablesByType(
    const QString & type_, bool public_) {
    QList<cs8Variable*> list;
    foreach(cs8Variable* variable,m_variableList) {
        if (variable->type() == type_ && variable->isPublic() == public_)
            list.append(variable);
    }
    return list;
}

int cs8VariableModel::rowCount(const QModelIndex& index) const {
    return m_variableList.count();
}

int cs8VariableModel::columnCount(const QModelIndex & index) const {
    return 4;
}

void cs8VariableModel::clear() {
    m_variableList.clear();
}

QVariant cs8VariableModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();
    if (role == Qt::DisplayRole || role == Qt::EditRole) {


        cs8Variable* variable = m_variableList.at(row);
        switch (column) {
        case 0:
            return variable->name();
            break;

        case 1:
            return variable->type();
            break;

        case 2:
            return variable->size();
            break;

        case 3:
            return variable->description();
            break;
        }
    }

    return QVariant();
}

QVariant cs8VariableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        switch (section) {
        case 0:
            return tr("Name");
            break;

        case 1:
            return tr("Type");
            break;

        case 2:
            return tr("Size");
            break;

        case 3:
            return tr("Description");
            break;
        }

    return QVariant();
}

/*!
 \fn cs8VariableModel::flags ( const QModelIndex & index ) const
 */
Qt::ItemFlags cs8VariableModel::flags(const QModelIndex & index) const {
    if (!index.isValid())
        return 0;

    if (index.column() == 3)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

}

/*!
 \fn cs8VariableModel::setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
 */
bool cs8VariableModel::setData(const QModelIndex & index,
                               const QVariant & value, int role) {
    if (role == Qt::EditRole) {
        if (index.column() == 3) {
            cs8Variable* variable = m_variableList.at(index.row());
            variable->setDescription(value.toString());
            emit (dataChanged(index, index));
        }
    }
    return true;
}

/*!
 \fn cs8VariableModel::getVarByName(const QString & name)
 */
cs8Variable* cs8VariableModel::getVarByName(const QString & name) {
    for (int i = 0; i < m_variableList.count(); i++) {
        if (m_variableList.at(i)->name().compare(name) == 0)
            return m_variableList.at(i);
    }
    return 0;
}

QDomNode cs8VariableModel::document(QDomDocument & doc) {
    QDomDocumentFragment fragment = doc.createDocumentFragment();
    QDomElement section = doc.createElement(m_mode ? "paramSection"
                                                   : "localSection");
    fragment.appendChild(section);
    foreach ( cs8Variable* var, m_variableList ) {
        QDomElement varElement = doc.createElement(m_mode ? "param"
                                                          : "local");
        if (m_mode)
            varElement.setAttribute("byVal", var->use());
        else
            varElement.setAttribute("size", var->size());

        varElement.setAttribute("type", var->type());
        varElement.setAttribute("name", var->name());
        section.appendChild(varElement);
    }
    return fragment;
}

QString cs8VariableModel::toDocumentedCode() {
    QString header;
    QString prefix = m_mode ? "param" : "var";
    foreach ( cs8Variable* var, m_variableList ) {
        QString descr = var->description();
        header+= QString("\n\\%1 %3 %2") .arg(prefix) .arg(descr) .arg(
                    var->name());
    }
    qDebug() << "header: " << header;
    return header;
}
