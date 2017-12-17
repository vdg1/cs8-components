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

    uint size(int dimension=0)
    {
        QStringList list=m_element.attribute("size","").split(" ");
        if (list.count()>=dimension)
            return list.at(dimension).toUInt();
        else
            return 0;
    }

    QString dimension() const;
    uint dimensionCount() const;
    void setDimension(const QString & dim);
    void setType(QString value);
    QString type() const;
    QString prefix() const;
    bool isConst() const;

    void setName(QString value);
    QString name() const
    {
        return m_element.attribute("name");
    }
    cs8Variable(QDomElement & element, const QString & descripton=QString());
    cs8Variable(cs8Variable *var);
    cs8Variable();

    QString toString(bool withTypeDefinition=true);
    QString documentation(bool withPrefix, bool forCOutput);
    bool isPublic() const
    {
        return m_element.attribute("access","private")=="private"?false:true;
    }

    QStringList father();

    void setPublic(bool m_public);

    QDomNodeList values()
    {
        return m_element.childNodes();
    }
    QString definition();


    void setGlobal(bool global);
    bool isGlobal() const
    {
        //qDebug() << "var: " << m_name << " has " << m_values.count();
        return m_global;// m_values.count() > 0;
    }

    QString allSizes();
    void setAllSizes(const QString & sizes);

    QDomElement element() const
    {
        return m_element;
    }

    QVariant varValue(QString index="0");
    bool isBuildInType()const ;
    QStringList buildInTypes();
    bool hasConstPrefix(QString *prefix=0) const;


protected:
    QStringList m_buildInTypes;
    QDomElement m_element;
    QString m_description;
    QDomDocumentFragment m_docFragment;
    QDomDocument m_doc;
    bool m_global;

    void setBuildInTypes();
signals:
    void modified();

};
#endif
