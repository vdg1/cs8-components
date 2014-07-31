#ifndef CS8VARIABLE_H
#define CS8VARIABLE_H
//
#include <QObject>
#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QDomElement>
#include <QStringList>
//
class cs8Variable : public QObject
{

    Q_OBJECT

public:
    void setUse(QString value);

    QString use() const;

    void setDescription(QString value);
    QString description() const;

    uint size(int dimension=0) {
        QStringList list=m_element.attribute("size","").split(" ");
        if (list.count()>=dimension)
            return list.at(dimension).toUInt();
        else
            return 0;
    }

    QString dimension() const;
    void setDimension(const QString & dim);
    void setType(QString value);
    QString type() const;


    void setName(QString value);
    QString name() {
        return m_element.attribute("name");
    }
    cs8Variable(QDomElement & element, const QString & descripton=QString());
    cs8Variable();

    QString toString(bool withTypeDefinition=true);
    QString documentation(bool withPrefix=true);
    bool isPublic() const {
        return m_element.attribute("access","private")=="private"?false:true;
    }

    void setPublic(bool m_public);

    QDomNodeList values() {
        return m_element.childNodes();
    }
    QString definition();


    void setGlobal(bool global);
    bool isGlobal() const {
        //qDebug() << "var: " << m_name << " has " << m_values.count();
        return m_global;// m_values.count() > 0;
    }

    QString allSizes();
    void setAllSizes(const QString & sizes);

    QDomElement element() const {
        return m_element;}

    QVariant varValue(QString index=0);

protected:
    QDomElement m_element;
    QString m_description;
    QDomDocumentFragment m_docFragment;
     QDomDocument m_doc;
    bool m_global;


signals:
    void modified();

};
#endif
