#ifndef CS8VARIABLEMODEL_H
#define CS8VARIABLEMODEL_H
//
#include <QAbstractTableModel>
#include <QDomElement>

class cs8Variable;
//
class cs8VariableModel: public QAbstractTableModel {
    Q_OBJECT

private:
    bool m_mode;
protected:
    QList<cs8Variable*> m_variableList;


public:
    QString toDocumentedCode();
    QDomNode document(QDomDocument & doc);
    QVariant headerData(int section, Qt::Orientation orientation, int role =
            Qt::DisplayRole) const;
    int rowCount(const QModelIndex& index) const;
    QVariant data(const QModelIndex & index, int role) const;
    int columnCount(const QModelIndex & index) const;
    bool addVariable(QDomElement & element, const QString & description =
            QString());
    bool addGlobalVariable(QDomElement & element, const QString & description =
            QString());
    cs8VariableModel(QObject * parent = 0, bool mode = false);
    bool setData(const QModelIndex & index, const QVariant & value, int role =
            Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const;
    cs8Variable* getVarByName(const QString & name);

    QString toDtxDocument();
    QList<cs8Variable*> findVariablesByType(const QString & type_, bool public_=true);
    void clear();
    QList<cs8Variable*> publicVariables();
    QList<cs8Variable*> privateVariables();
    cs8Variable* variable(QModelIndex index);
    QStringList variableNameList();
    QList<cs8Variable*> variableList()
    {
        return m_variableList;
    }

signals:
    void modified(bool);

protected slots:
    void slotModified();
};
#endif
