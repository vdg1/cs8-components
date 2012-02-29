#include "cs8variable.h"
#include <QStringList>
//
cs8Variable::cs8Variable(QDomElement & element, const QString & description)
    :QObject()
{
    m_element=element;
    m_description=description;
}

cs8Variable::cs8Variable()
    :QObject()
{
}

QString cs8Variable::toString(bool withTypeDefinition) {
    if (withTypeDefinition)
        return QString("%1 %2%3")
                .arg(type())
                .arg(use() == "reference" ? "" : "& ")
                .arg(name());
    else
        return QString("%1")
                .arg(name());
}


QString cs8Variable::documentation(bool withPrefix) {
    QString out;
    QString prefix=withPrefix?"///":"";
    if (m_description.isEmpty())
        return QString();

    QStringList list = m_description.split("\n");

    //qDebug() << "documentation(): " << m_name << ":" << m_description;
    bool inCodeSection = false;
    bool firstLine = true;
    foreach (QString str,list)
    {
        if (str.contains("<code>")) {
            inCodeSection = true;
            out += prefix+"<br>\n";
        }
        if (str.contains("</code>")) {
            inCodeSection = false;
            out += prefix+"<br>\n";
        }
        out += (firstLine ? "" : prefix) + str + (inCodeSection ? "<br>"
                                                                : "") + "\n";
        firstLine = false;
    }
    //qDebug() << "processed: " << out;
    if (!isGlobal())
        return prefix+"\\param " + name() + " " + out + "\n";
    else
        return out;
}

void cs8Variable::setPublic(bool m_public)
{
    emit modified ();
    m_element.setAttribute("access",m_public? "public":"private");

}

QString cs8Variable::definition() {

    return (QString("%1 %2").arg(type()).arg(name())) + (allSizes() !=QString() ? QString(
                                                                                      "[%1]").arg(allSizes()) : "");
}

void cs8Variable::setGlobal(bool global)
{
    emit modified ();
    m_global=global;
    if (global)
    {
        m_element.setTagName ("Data");
        m_element.setAttribute ("xsi:type","array");
        m_element.setAttribute ("size","1");
    }
}

void cs8Variable::setUse(QString value)
{
    emit modified ();
    m_element.setAttribute("use",value);
}

QString cs8Variable::use() const
{
    return m_element.attribute("use","reference");
}

void cs8Variable::setDescription(QString value)
{
    emit modified ();
    m_description = value;
}

QString cs8Variable::description() const
{
    return m_description;
}

void cs8Variable::setType(QString value)
{
    emit modified ();
    m_element.setAttribute("type",value);
}

QString cs8Variable::type() const
{
    return m_element.attribute("type");

}

void cs8Variable::setName(QString value)
{
    emit modified ();
    m_element.setAttribute("name",value);
}

//
