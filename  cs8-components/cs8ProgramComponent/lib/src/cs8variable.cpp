#include "cs8variable.h"
#include <QStringList>
//
cs8Variable::cs8Variable(QDomElement & element, const QString & description) {
        m_element=element;
        m_description=description;
}

cs8Variable::cs8Variable()
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

QString cs8Variable::definition() {

    return (QString("%1 %2").arg(type()).arg(name())) + (allSizes() !=QString() ? QString(
                                                                                    "[%1]").arg(allSizes()) : "");
}

//
