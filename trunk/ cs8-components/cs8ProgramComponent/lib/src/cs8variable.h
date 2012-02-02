#ifndef CS8VARIABLE_H
#define CS8VARIABLE_H
//
#include <QString>
#include <QHash>
#include <QVariant>
#include <QDebug>
#include <QDomElement>
#include <QStringList>
//
class cs8Variable {

public:
    void setUse(QString value) {
        m_element.setAttribute("use",value);
    }
    QString use() {
        return m_element.attribute("use","reference");
    }
    void setDescription(QString value) {
        m_description = value;
    }
    QString description() {
        return m_description;
    }
    uint size(int dimension=0) {
        QStringList list=m_element.attribute("size","").split(" ");
        if (list.count()>=dimension)
            return list.at(dimension).toUInt();
        else
            return 0;
    }
    void setType(QString value) {
        m_element.setAttribute("type",value);
    }
    QString type() {
        return m_element.attribute("type");
    }

    /*
        enum VariableTypes {
  Point, Joint, Flange, Bool, String, Num, Config
 };
        */

    void setName(QString value) {
        m_element.setAttribute("name",value);
    }
    QString name() {
        return m_element.attribute("name");
    }
    cs8Variable(QDomElement & element, const QString & descripton=QString());
    QString toString();
    QString documentation(bool withPrefix=true);
    bool isPublic() const {
        return m_element.attribute("access","private")=="private"?false:true;
    }

    void setPublic(bool m_public) {
        m_element.setAttribute("access",m_public? "public":"private");
    }

    QDomNodeList values() {
        return m_element.childNodes();
    }
    QString definition();


    void setGlobal(bool m_global){
        this->m_global=m_global;
    }
    bool isGlobal() const {
        //qDebug() << "var: " << m_name << " has " << m_values.count();
        return m_global;// m_values.count() > 0;
    }

    QString allSizes(){
        return m_element.attribute("size").replace(" ",", ");
    }

    QDomElement element() const { return m_element;}

protected:
    QDomElement m_element;
    QString m_description;
    bool m_global;
};
#endif
