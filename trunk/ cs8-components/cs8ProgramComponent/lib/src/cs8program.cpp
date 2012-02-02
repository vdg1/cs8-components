#include "cs8program.h"
#include "cs8variable.h"

#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QFileInfo>

#define MAX_LENGTH 40
//
cs8Program::cs8Program(QObject * parent) :
    QObject() {
        m_localVariableModel = new cs8VariableModel(this);
        m_parameterModel = new cs8ParameterModel(this);
    }
//
/*!
 \fn cs8Program::cs8Program(const QString & filePath)
 */
/*
 cs8Program::cs8Program(const QString & filePath) :
 QObject() {
 m_localVariableModel = new cs8VariableModel(this);
 m_parameterModel = new cs8ParameterModel(this);
 open(filePath);
 }
 */

bool cs8Program::open(const QString & filePath) {
        qDebug() << "cs8Program::open () " << filePath;
        QDomDocument doc("program");
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
            return false;
        if (!doc.setContent(&file)) {
            file.close();
            return false;
        }
        file.close();
        QFileInfo info(filePath);

        qDebug() << "cs8Program::open ():  ok";
        return parseProgramDoc(doc);
    }


void cs8Program::printChildNodes(const QDomElement & element)
    {
        qDebug() << "Child nodes of: " << element.tagName();
        for (int i=0;i<element.childNodes().count();i++)
            qDebug() << i << element.childNodes().at(i).nodeName();

    }

/*!
 \fn cs8Program::parseProgramDoc(const QDomDocument & doc)
 */
bool cs8Program::parseProgramDoc(const QDomDocument & doc) {
        m_root = doc.documentElement();


        printChildNodes(m_root);
        m_programSection = m_root.firstChild().toElement();


        printChildNodes(m_programSection);
        if (m_programSection.isNull())
            qDebug() << "Reading program section failed";



        m_paramSection = m_programSection.elementsByTagName("Parameters").at(0).toElement();
        m_localSection = m_programSection.elementsByTagName("Locals").at(0).toElement();

        qDebug() << m_programSection.elementsByTagName("Code").at(0).nodeType();
        m_codeSection =  m_programSection.elementsByTagName("Code").at(0).toElement();
        if (m_codeSection.isNull())
            qDebug() << "Reading code section failed";


        QDomNodeList childList;

        childList = m_localSection.elementsByTagName("Local");
        for (int i = 0; i < childList.count(); i++) {
            QDomElement item = childList.at(i).toElement();
            m_localVariableModel->addVariable(item);
        }

        childList = m_paramSection.elementsByTagName("Parameter");
        for (int i = 0; i < childList.count(); i++) {
            QDomElement item = childList.at(i).toElement();
            m_parameterModel->addVariable(item);
        }
        parseDocumentation(val3Code());
        return true;
    }


// returns the code without documentation header
QString cs8Program::val3Code(bool withDocumentation) {
        // qDebug() << m_codeSection.text();
        return withDocumentation? m_codeSection.text():extractCode(m_codeSection.text());
    }

QString cs8Program::toCSyntax() {
        QString val3_=val3Code();
        val3_=val3_.replace(QString("call "),QString(""),Qt::CaseSensitive);
        return val3_;
    }

void cs8Program::setVal3Code(const QString& theValue) {
        //m_codeSection.setData(theValue);

    }

QString cs8Program::extractCode(const QString & code_){
        QString code;
        // extract code
        code.replace("\r\n", "\n");
        QStringList list = code.split("\n");
        int start = list.indexOf(QRegExp("\\s*begin\\s*"));
        int stop = start + 1;
        while (list.at(stop).contains(QRegExp("^\\s*//.*")))
            stop++;
        for (int i = stop - 1; i > start; i--)
            list.removeAt(i);
        return list.join("\n");
    }

QString cs8Program::extractDocumentation(const QString & code_){
        //
        //extract documentation section
        QString documentationSection = code_;
        documentationSection.replace("\r\n", "\n");
        // revert automatic correction done by Val3 Studio
        documentationSection.replace("//\\endIf","//\\endif");
        documentationSection.remove(0, documentationSection.indexOf("begin\n") + 8);
        QStringList documentationList = documentationSection.split("\n");
        //
        // remove code section
        // search first non comment line
        int i = 0;
        while (documentationList.at(i).simplified().startsWith(("//")) || documentationList.at(i).simplified().isEmpty() )
            i++;
        // remove lines starting from found first non comment line
        while (documentationList.count() > i)
            documentationList.removeLast();

        return documentationList.join("\n");
    }

/*!
 \fn cs8Program::parseDocumentation( QString & code)
 */
/*
 void cs8Program::parseDocumentation(QString & code) {

 QString text = code;

 // remove header from code
 QString out;
 if (true) {
 code.replace("\r\n", "\n");
 QStringList list = code.split("\n");
 int start = list.indexOf(QRegExp("\\s*begin\\s*"));
 int stop = start + 1;
 while (list.at(stop).contains(QRegExp("^\\s*�//.*")))
 /*
 stop++;
 for (int i = stop - 1; i > start; i--)
 list.removeAt(i);
 code = list.join("\n");
 //out=code.replace(start,stop-start,QString());
 }
 text.replace("\r\n", "\n");
 text.remove(0, code.indexOf("begin\n") + 8);
 QStringList list = text.split("\n");

 int i = 0;
 for (i = 0; i < list.count(); i++) {
 if (!(list[i] = list[i].simplified()).startsWith("//"))
 break;
 }
 while (list.count() > i)
 list.removeLast();

 int state = 0;
 QString itemName = "";
 QString itemDescr = "";
 QStringList itemList;
 QString line;
 i = 0;
 while (i < list.count()) {
 line = list.at(i);
 QString item;
 if (line.startsWith("//!")) {
 item = line;
 i++;
 }

 while (i < list.count() && !((line = list.at(i)).startsWith("//!"))) {
 item += line.replace(QRegExp("^//"), "\n");
 i++;
 }
 itemList << item;
 }

 foreach ( QString line,itemList ) {
 if (line.startsWith("//!brief")) {
 line.remove("//!brief\n");
 m_description = line;
 }

 if (line.startsWith("//!param")) {
 line.remove("//!param");
 itemName = line.left(line.indexOf(":")).simplified();
 itemDescr
 = line.right(line.length() - line.indexOf(":") - 1).simplified();
 if (m_parameterModel->getVarByName(itemName) != 0)
 m_parameterModel->getVarByName(itemName)->setDescription(
 itemDescr);
 }
 if (line.startsWith("//!local")) {
 line.remove("//!local");
 itemName = line.left(line.indexOf(":")).simplified();
 itemDescr = line.simplified();
 if (m_localVariableModel->getVarByName(itemName) != 0)
 m_localVariableModel->getVarByName(itemName)->setDescription(
 itemDescr);
 }
 if (line.startsWith("//!global")) {
 line.remove("//!global");
 line = line.simplified();
 itemName = line.left(line.indexOf(" ")).simplified();
 itemDescr = line.simplified();
 qDebug() << "global var: " << itemName << ":" << itemDescr;
 emit globalVariableDocumentationFound(itemName, itemDescr);
 }
 }
 }
 */

void cs8Program::parseDocumentation(const QString & code_) {

        QString documentation=extractDocumentation(code_);
        QString tagType;
        QString tagName;
        QString tagText;

        //qDebug() << "parseDocumentation: " << m_name << ":" << documentationList;
        foreach(QString line,documentation.split("\n")) {
            line = line.simplified();
            // process a complete tag before starting the next tag
            if ((line.startsWith("//!") || line.startsWith("//\\")) && !tagType.isEmpty()) {
                // remove trailing array indexes from tag name
                tagName=tagName.remove(QRegExp("\\[\\d*\\]"));
                //qDebug() << "process tag:" << tagType << ":" << tagName << ":" << tagText;
                if (tagType == "param") {
                    if (m_parameterModel->getVarByName(tagName) != 0)
                        m_parameterModel->getVarByName(tagName)->setDescription(
                                    tagText);
                }
                else if (tagType == "local" or tagType=="var") {
                    if (m_localVariableModel->getVarByName(tagName) != 0)
                        m_localVariableModel->getVarByName(tagName)->setDescription(
                                    tagText);
                }
                else if (tagType == "global") {
                    //qDebug() << "global var: " << tagName << ":" << tagText;
                    emit globalVariableDocumentationFound(tagName, tagText);
                }
                else if (tagType == "brief") {
                    m_description = tagText;
                }
                else if (tagType == "doc") {
                    m_description = tagText;
                }
                else if (tagType == "module") {
                    qDebug() << "module doc: " << tagName << ":" << tagText;
                    emit moduleDocumentationFound(tagText);
                }
                else {
                    m_description += QString("\n\\%1 %2\n%3").arg(tagType).arg(tagName).arg(tagText);
                }
            }
            // a new tag
            if (line.startsWith("//!") || line.startsWith("//\\")) {

                tagText = "";
                tagName = "";
                // set tag type
                tagType = line.remove("//!").remove("//\\").simplified();
                tagType = tagType.left(tagType.indexOf(" ")).simplified();
                // set tag name
                QRegExp rx;
                rx.setPattern("^\\s*" + tagType);
                tagText = line.remove(rx).simplified();
                rx.setPattern("^\\w*(\\s|$)");
                int pos = rx.indexIn(tagText);
                if (pos != -1)
                    tagName = rx.cap().simplified();
                else
                    tagName = "";
                // set tag text
                tagText.remove(rx);
                rx.setPattern("^:\\s*");
                tagText.remove(rx);
                qDebug() << "new tag:" << tagType << ":" << tagName << ":"
                         << tagText;
            } else {
                // Read tag text from code lines and prepend a \n at end of each read line.
                // However, do not prepend a \n if it is the first line.
                tagText += (tagText.isEmpty()?"":"\n")+line.remove(0,2);//"\n" + line.remove(0, 2);
            }

        }
        // process a complete tag if there is one available at the end
        if (!tagType.isEmpty()) {
            qDebug() << "process remaining tag:" << tagType << ":" << tagName << ":" << tagText;
            if (tagType == "param") {
                if (m_parameterModel->getVarByName(tagName) != 0)
                    m_parameterModel->getVarByName(tagName)->setDescription(tagText);
            }
            else if (tagType == "local" or tagType=="var") {
                if (m_localVariableModel->getVarByName(tagName) != 0)
                    m_localVariableModel->getVarByName(tagName)->setDescription(
                                tagText);
            }
            else if (tagType == "global") {
                qDebug() << "global var: " << tagName << ":" << tagText;
                emit globalVariableDocumentationFound(tagName, tagText);
            }
            else if (tagType == "brief") {
                m_description = tagText;
            }
            else if (tagType == "doc") {
                m_description = tagText;
            }
            else if (tagType == "module") {
                qDebug() << "module doc: " << tagName << ":" << tagText;
                emit moduleDocumentationFound(tagText);
            }
            else {
                m_description += QString("\n\\%1 %2\n%3").arg(tagType).arg(tagName).arg(tagText);
            }
        }
        //qDebug() << documentationList;

    }

QString cs8Program::description() const {
        return m_description;
    }

QString cs8Program::documentation(bool withPrefix) {
        qDebug() << "documentation: " << name() << ":" << m_description;
        QString out;
        QString prefix=withPrefix?"///":"";
        QStringList list = m_description.split("\n");
        //list.removeLast();
        bool inCodeSection = false;
        int indentation=0;
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
            if (inCodeSection)
            {
                qDebug() << str << "indent: " << indentation;
                if (str.simplified().indexOf(QRegExp("begin |if |for | while |do |switch| case"))==0){
                    str=QString().fill(QChar(' '),indentation*4)+str.simplified();
                    indentation++;
                }
                if (str.simplified().indexOf(QRegExp("end|until |break"))==0){
                    indentation--;
                    indentation=qMax(0,indentation);
                    str=QString().fill(QChar(' '),indentation*4)+str.simplified();
                }
            }
            out += prefix + str + (inCodeSection ? "<br>" : "") + "\n";
        }
        //out += "\n";
        //out =list.join("\n");
        foreach(cs8Variable* parameter,m_parameterModel->variableList()) {
            out += parameter->documentation();
        }
        return out;
    }

void cs8Program::setDescription(const QString& theValue) {
        m_description = theValue;
    }

/*
bool cs8Program::save(const QString & projectPath, bool withCode) {
    qDebug() << "Saving progam " << m_name;
    QDomDocument doc;
    QDomElement programList = doc.createElement("programList");
    programList.setAttribute("xmlns", "ProgramNameSpace");
    doc.appendChild(programList);

    QDomElement program = doc.createElement("program");
    program.setAttribute("name", name());
    program.setAttribute("public", isPublic() ? "true" : "false");
    programList.appendChild(program);


    QDomElement descriptionElement=doc.createElement("description");
    //descriptionElement.setAttribute("xmlns","ProgramNameSpace");
    program.appendChild(descriptionElement);
    program.appendChild(m_parameterModel->document(doc));
    program.appendChild(m_localVariableModel->document(doc));

    QDomElement source = doc.createElement("source");
    program.appendChild(source);

    QDomElement code = doc.createElement("code");
    source.appendChild(code);

    QDomText codeText;
    if (withCode)
    {
        codeText = doc.createTextNode(toDocumentedCode());
    }
    else
    {
        if (name()!="stop")
        {
            codeText = doc.createTextNode("begin\n"
                                          "logMsg(libPath())\n"
                                          "put(sqrt(-1))\n"
                                          "end");
        }
        else
        {
            codeText = doc.createTextNode("begin\n"
                                          "end");
        }
    }
    code.appendChild(codeText);

    QFile file(projectPath + "/" + fileName());
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream stream(&file);
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";
    stream << doc.toString();
    file.close();

    return true;
}
*/


bool cs8Program::save(const QString & projectPath, bool withCode) {
        qDebug() << "Saving progam " << name();
        QDomDocument doc;
        QDomProcessingInstruction process = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
        doc.appendChild(process);
        QDomNode root = m_root.cloneNode();
        doc.appendChild(root);


        QString codeText;
        if (withCode)
        {
            codeText = toDocumentedCode();
        }
        else
        {
            if (name()!="stop")
            {
                codeText = ("begin\n"
                            "logMsg(libPath())\n"
                            "put(sqrt(-1))\n"
                            "end");
            }
            else
            {
                codeText = ("begin\n"
                            "end");
            }
        }

        QDomCDATASection data=doc.createCDATASection(codeText);
        QDomElement code=root.toElement().firstChildElement("Program").firstChildElement("Code");
        code.removeChild(code.firstChild());
        code.appendChild(data);

        QFile file(projectPath + "/" + fileName());
        if (!file.open(QIODevice::WriteOnly))
            return false;

        QTextStream stream(&file);
        stream << doc.toString();
        file.close();

        return true;
    }

QString cs8Program::toDocumentedCode() {
        QString documentation;
        if (!m_description.isEmpty()) {
            documentation = "\\brief\n";
            documentation += m_description;
        }
        documentation += m_localVariableModel->toDocumentedCode();
        documentation += m_parameterModel->toDocumentedCode();
        if (!documentation.isEmpty())
            documentation+="\n";
        // format documentation section
        QStringList list;
        QRegExp rx;
        rx.setPattern("\\b");

        QString subString;
        /*
     while (documentation.length()>MAX_LENGTH){
         pos=documentation.lastIndexOf(rx,MAX_LENGTH);
         if (pos==-1){
             pos=documentation.indexOf(rx,MAX_LENGTH-1);
             if (pos==-1)
                 pos=documentation.length();
         }
        list.append(documentation.left(pos));
        documentation.remove(0,pos);
     }
     list.append(documentation);
     */
        list=documentation.split("\n");
        list.replaceInStrings(QRegExp("^"),"  //");
        documentation=list.join("\n")+"\n";
        QString code = val3Code();
        rx.setPattern("\\s*begin\\s*");
        int start = rx.indexIn(code, 0);
        start += rx.matchedLength();

        code.insert(start++, documentation);

        return code;
    }

QString cs8Program::definition() {

        return name() + "(" + m_parameterModel->toString() + ")";
    }

