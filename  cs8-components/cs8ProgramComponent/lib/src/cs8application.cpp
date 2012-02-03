#include "cs8application.h"

#include <QFile>
#include <QUrl>
#include <QFileInfo>
#include <QStringList>
#include <QDebug>
#include <QDir>

#include "cs8programmodel.h"
#include "cs8variablemodel.h"
#include "cs8libraryaliasmodel.h"
#include "cs8variable.h"
//
cs8Application::cs8Application(QObject *parent) :
    QObject(parent) {
    m_programModel = new cs8ProgramModel(this);
    m_variableModel = new cs8VariableModel(this);
    m_libraryAliasModel = new cs8LibraryAliasModel(this);
    m_projectPath="";
    m_projectName="";
    m_cellPath="";
    //	m_programModel->setProgramList(&m_programList);
    connect(m_programModel, SIGNAL(globalVariableDocumentationFound(const QString & , const QString & ))
            ,this, SLOT(slotGlobalVariableDocumentationFound(const QString & , const QString & )));
    connect(m_programModel, SIGNAL(moduleDocumentationFound( const QString & ))
            ,this, SLOT(slotModuleDocumentationFound(const QString & )));
}
//

bool cs8Application::open(const QString & filePath) {
    m_documentation = "";
    QDomDocument doc("Project");
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    file.close();

    m_projectPath = filePath;
    m_projectName = QFileInfo(m_projectPath).baseName();
    //m_projectPath.chop(m_projectPath.length() - 1 - m_projectPath.lastIndexOf(QDir::separator ()));
    m_projectPath=QFileInfo(m_projectPath).absolutePath()+QDir::separator();
    bool result=parseProject(doc);
    return result;
}

bool cs8Application::openFromPathName(const QString & filePath) {
    QString filePath_ = QDir::cleanPath(filePath);
    QString name = filePath_ + "/" + filePath_.right(filePath_.length()
                                                     - filePath_.lastIndexOf("/", -2)) + ".pjx";
    qDebug() << "openFromPathName: " << name;
    bool result=open(name);
    return result;
}

bool cs8Application::exportInterfacePrototype(const QString & path) {
    QString fileName;

    qDebug() << "export prototype to : " << path + "/" + m_projectName;
    qDebug() << "saving dtx";
    // create dest dir
    QDir dir(path + "/" + m_projectName);
    dir.mkpath(path + "/" + m_projectName);
    // delete existing files in dir
    QStringList files = dir.entryList(QDir::Files);
    foreach(QString applicationFile,files) {
        dir.remove(applicationFile);
    }
    // save data file
    fileName = path + "/" + m_projectName + "/" + m_projectName + ".dtx";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream stream(&file);

    //stream << m_dataDoc.toByteArray();
    stream << m_variableModel->toDtxDocument();
    //qDebug() << m_dataDoc.toString();
    file.close();
    qDebug() << "saving dtx done";
    qDebug() << "saving pjx";

    // save pjx file
    QDomDocument doc;
    QDomProcessingInstruction process = doc.createProcessingInstruction("xml",
                                                                        "version=\"1.0\" encoding=\"utf-8\"");
    doc.appendChild(process);
    QDomElement projectElement = doc.createElement("Project");
    projectElement.setAttribute("xmlns", "http://www.staubli.com/robotics/VAL3/Project/3");
    doc.appendChild(projectElement);

    QDomNode parameters =m_parameters.cloneNode();
    projectElement.appendChild(parameters);


    QDomElement programSection = doc.createElement("Programs");
    projectElement.appendChild(programSection);

    foreach(cs8Program* program,m_programModel->publicPrograms())
    {
        QDomElement programElement = doc.createElement("Program");
        programElement.setAttribute("file", program->fileName());
        programSection.appendChild(programElement);

        program->save(path + "/" + m_projectName, false);
    }

    QDomNode dataSection = m_dataSection.cloneNode();
    projectElement.appendChild(dataSection);

    /*
    QDomNode aliasSection = m_aliasSection.cloneNode();
    projectElement.appendChild(aliasSection);
    */
    // for interface prototype no libraries are needed
    QDomElement aliasSection=doc.createElement("Libraries");
    projectElement.appendChild(aliasSection);

    QDomNode types =m_typesSection.cloneNode();
    projectElement.appendChild(types);


    fileName = path + "/" + m_projectName + "/" + m_projectName + ".pjx";
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream out(&file);
    out << doc.toString();
    file.close();
    qDebug() << "saving pjx done";
    return true;
}

QString cs8Application::exportToCSyntax() {
    QStringList out;

    out << documentation();
    out << QString("class %1{").arg(m_projectName);
    out << " public:";
    foreach(cs8Variable* variable,m_variableModel->publicVariables())
    {
        out << variable->documentation();
        out << QString("   %1;").arg(variable->definition());
    }
    foreach(cs8LibraryAlias* alias,m_libraryAliasModel->aliasList())
    {
        out << alias->documentation();
        out << QString("   %1;").arg(alias->definition());
    }
    foreach(cs8Program* program,m_programModel->publicPrograms())
    {
        out << program->documentation();
        out << QString("   void %1{\n%2\n}").arg(program->definition()).arg(program->toCSyntax());
    }

    out << " private:";
    foreach(cs8Variable* variable,m_variableModel->privateVariables())
    {
        out << variable->documentation();
        out << QString("   %1;").arg(variable->definition());
    }
    foreach(cs8Program* program,m_programModel->privatePrograms())
    {
        out << program->documentation();
        out << QString("   void %1{\n%2\n}").arg(program->definition()).arg(program->val3Code());
    }

    out << "}";

    return   out.join(("\n"));
}

/*!
 *
 */
void cs8Application::exportToCFile(const QString & path) {
    QString fileName;

    qDebug() << "export C interface to : " << path + "/" + m_projectName;
    qDebug() << "saving C file";
    // create dest dir
    //QDir dir(path + "/" + m_projectName);
    //dir.mkpath(path + "/" + m_projectName);

    fileName = path + "/" + m_projectName + ".c";
    QFile file;
    file.setFileName(fileName);
    if (!file.open(QFile::WriteOnly))
        return;
    QTextStream stream(&file);
    stream << exportToCSyntax();//  out.join(("\n"));
    file.close();
}

/*!
 \fn cs8Application::parseProject(const QDomDocument & doc)
 */
bool cs8Application::parseProject(const QDomDocument & doc) {
    m_programModel->clear();
    m_programModel->setCellPath(m_cellPath);
    m_variableModel->clear();
    m_libraryAliasModel->clear();

    QDomElement rootElement = doc.documentElement();

    m_parameters = rootElement.elementsByTagName("Parameters").at(0).toElement();
    m_programSection = rootElement.elementsByTagName("Programs").at(0).toElement();
    m_dataSection = rootElement.elementsByTagName("Database").at(0).toElement();
    m_aliasSection = rootElement.elementsByTagName("Libraries").at(0).toElement();
    m_typesSection = rootElement.elementsByTagName("Types").at(0).toElement();


    // load data file
    QDomElement data = m_dataSection.elementsByTagName("Data").at(0).toElement();
    if (loadDataFile(m_projectPath+data.attribute("file"))) {

        // load programs
        QDomNodeList list = m_programSection.elementsByTagName("Program");
        for (int i = 0; i < list.count(); i++) {
            QString fileName = list.at(i).toElement().attribute("file");
            m_programModel->addProgram(m_projectPath + fileName);
        }
        // load alias
        list = m_aliasSection.elementsByTagName("Library");
        for (int i = 0; i < list.count(); i++) {
            m_libraryAliasModel->addAlias(list.at(i).toElement());
        }
        return true;
    } else{
        qDebug() << "failed";
        return false;
    }

}

void cs8Application::slotGlobalVariableDocumentationFound(const QString & name,
                                                          const QString & document) {

    qDebug() << "find: " << name;
    cs8Variable*variable = m_variableModel->getVarByName(name);
    if (variable != 0) {
        qDebug() << "setting doc for " << name << " to " << document;
        variable->setDescription(document);
    }
}

void cs8Application::slotModuleDocumentationFound(const QString & document) {
    m_documentation = document;
}
bool cs8Application::loadDocumentationFile(const QString & fileName) {
    if (m_programModel->getProgramByName("_globals")) {

    }
    return false;
}

cs8ProgramModel* cs8Application::programModel() const {
    return m_programModel;
}

bool cs8Application::loadDataFile(const QString & fileName) {
    qDebug() << "Loading data file: " << fileName;

    QDomDocument doc("data");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    ///TODO: quick fix to load data
    //m_dataDoc = doc;
    m_variableModel->clear();
    QDomElement rootElement = doc.documentElement();
    QDomNodeList sectionList = rootElement.childNodes();
    for (int i = 0; i < sectionList.count(); i++) {
        QDomElement sectionElement = sectionList.at(i).toElement();
        qDebug() << "Reading section: " << sectionElement.tagName();
        QDomNodeList variableList = sectionElement.childNodes();
        for (int j = 0; j < variableList.count(); j++) {
            QDomElement variableElement = variableList.at(j).toElement();
            m_variableModel->addGlobalVariable(variableElement);
        }
    }
    file.close();
    return true;
}

void cs8Application::save() {
    foreach(cs8Program* program, m_programModel->programList())
    {
        program->save(m_projectPath);
    }
}

QString cs8Application::projectPath() {
    return m_projectPath;
}

QString cs8Application::documentation() {

    QString out;
    QStringList list = m_documentation.split("\n");
    bool inCodeSection = false;
    int indentation=0;
    QString indentText;
    foreach (QString str,list)
    {
        if (str.contains("<code>")) {
            inCodeSection = true;
            out += "///<br>\n";
        }
        if (str.contains("</code>")) {
            inCodeSection = false;
            out += "///<br>\n";
        }
        if (inCodeSection)
        {
            qDebug() << str << "indent: " << indentation;
            if (str.simplified().indexOf(QRegExp("begin |if |for | while |do |switch| case"))==0){
                str=indentText+str.trimmed();
                indentation++;
                indentText="";
                for (int i=0;i<indentation;i++)
                    indentText+="&nbsp;";
                qDebug() << "increase indent";


            }else
                if (str.simplified().indexOf(QRegExp("end(If|While|For|Switch)|until |break"))==0){
                    indentation=qMax(0,--indentation);
                    qDebug() << "decrease indent to "<<indentation;
                    indentText="";
                    for (int i=0;i<indentation;i++)
                        indentText+="&nbsp;";
                    str=indentText+str.trimmed();
                }else
                    str=indentText+str.trimmed();
        }
        out += "///" + str + (inCodeSection ? "<br>" : "") + "\n";
    }
    out += "\n";
    return out;
}

QString cs8Application::checkVariables()
{
    QStringList output;
    // map containing information if a global variable has been referenced
    QMap<QString, bool> referencedMap;
    // build map
    foreach(cs8Variable *var, m_variableModel->variableList ())
    {
        qDebug() << "Checking global variable: " << var->name ();
        referencedMap.insert (var->name (),false);
        // check if a local variable hides the global variable
        foreach(cs8Program* program, m_programModel->programList ())
        {
            qDebug() << "  Checking program: " << program->name ();
            // check if global variable name is also declared as local variable
            if (program->localVariableModel ()->variableNameList ().contains (var->name ()))
            {
                qDebug() << "Warning: Global variable '" << var->name () << "'' is hidden in program " << program->name();
                //output.append (QString("Warning: Global variable '" + var->name () + "' is hidden in program " + program->name()+" by a local variable of the same name"));
                output.append (QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>1<msg>%2<file>%3")
                               .arg (program->name ())
                               .arg ("Warning: Global variable '" + var->name () + "' is hidden in program by a local variable of the same name")
                               .arg(program->cellFilePath ()));
            }
            else
            {
                // if global variable is not hidden, check if it is used
                if (program->variableTokens ().contains (var->name ()))
                {
                    referencedMap[var->name ()]=true;
                }

            }
            foreach(cs8Variable* lvar,program->localVariableModel ()->variableList ())
            {
                if (!program->variableTokens ().contains (lvar->name ()))
                {
                    output.append (QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>1<msg>%2<file>%3")
                                   .arg (program->name ())
                                   .arg ("Warning: Local variable '" + lvar->name () + "' is not used")
                                   .arg(program->cellFilePath ()));
                }
            }
        }
        if (referencedMap[var->name ()]==false)
        {
            //output.append (QString("Warning: Global variable '" + var->name () + "' is not used"));
            output.append (QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>1<msg>%2<file>%3")
                           .arg ("")
                           .arg ("Warning: Global variable '" + var->name () + "' is not used")
                           .arg(""));

        }
    }
    return output.join ("\n");
}

void cs8Application::setCellPath(const QString &path)
{
    m_cellPath=path;
}
