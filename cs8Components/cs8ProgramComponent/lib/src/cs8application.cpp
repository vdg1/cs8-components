#include "cs8application.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStringList>
#include <QUrl>

#include "cs8libraryaliasmodel.h"
#include "cs8programmodel.h"
#include "cs8variable.h"
#include "cs8variablemodel.h"
#include <QBuffer>
#include <QRegularExpression>
#include <QXmlStreamWriter>
#include <Qt3DCore/QTransform>
//
void cs8Application::initPrecompilerSettings() {
  QSettings settings;

  reportUnusedPublicGlobalVariables =
      settings.value("reportUnusedPublicGlobalVariables", false).toBool();
  reportHiddenGlobalVariables =
      settings.value("reportHiddenGlobalVariables", true).toBool();
  reportParametersPostfix =
      settings.value("reportParametersPostfix", true).toBool();
  reportToDos = settings.value("reportToDos", true).toBool();
}
QString cs8Application::mainPageDocumentation() const {
  return m_mainPageDocumentation;
}

void cs8Application::setMainPageDocumentation(
    const QString &mainPageDocumentation) {
  m_mainPageDocumentation = mainPageDocumentation;
}
QString cs8Application::version() const { return m_version; }
bool cs8Application::includeLibraryDocuments() const {
  return m_includeLibraryDocuments;
}

void cs8Application::setIncludeLibraryDocuments(bool includeLibraryDocuments) {
  if (m_includeLibraryDocuments != includeLibraryDocuments)
    emit modified(true);
  m_includeLibraryDocuments = includeLibraryDocuments;
}

cs8Application::cs8Application(QObject *parent)
    : QObject(parent), m_modified(false), m_withUndocumentedSymbols(false),
      m_includeLibraryDocuments(false), m_projectVersion("7.0"),
      m_projectStackSize("5000"), m_projectMillimeterUnit("true"),
      m_compactFileMode(true) {
  m_programModel = new cs8ProgramModel(this);
  m_globalVariableModel = new cs8GlobalVariableModel(this);
  m_libraryAliasModel = new cs8LibraryAliasModel(this);
  m_typeModel = new cs8TypeModel(this);
  m_projectPath = "";
  m_projectName = "";
  m_cellPath = "";
  setModified(false);

  initPrecompilerSettings();

  //	m_programModel->setProgramList(&m_programList);
  connect(m_programModel, &cs8ProgramModel::globalVariableDocumentationFound,
          this, &cs8Application::slotGlobalVariableDocumentationFound);
  connect(m_programModel, &cs8ProgramModel::moduleDocumentationFound, this,
          &cs8Application::slotModuleDocumentationFound);
  connect(m_programModel, &cs8ProgramModel::briefModuleDocumentationFound, this,
          &cs8Application::slotBriefModuleDocumentationFound);
  connect(m_programModel, &cs8ProgramModel::mainPageDocumentationFound, this,
          &cs8Application::slotMainPageDocumentationFound);
  connect(m_programModel, &cs8ProgramModel::exportDirectiveFound, this,
          &cs8Application::slotExportDirectiveFound);
  connect(m_programModel, &cs8ProgramModel::unknownTagFound, this,
          &cs8Application::slotUnknownTagFound);

  connect(m_programModel, &cs8ProgramModel::modifiedChanged, this,
          &cs8Application::setModified);
  connect(m_globalVariableModel, &cs8VariableModel::modified, this,
          &cs8Application::setModified);
  connect(m_libraryAliasModel, &cs8LibraryAliasModel::modified, this,
          &cs8Application::setModified);
  connect(m_typeModel, &cs8LibraryAliasModel::modified, this,
          &cs8Application::setModified);
}
//

void cs8Application::setProjectPath(const QString &pth) {
  QFileInfo fileInfo(pth);
  QDir dirInfo(pth);
  // qDebug() << fileInfo.absolutePath() << fileInfo.isFile() <<
  // fileInfo.isDir(); qDebug() << dirInfo.absolutePath();

  m_projectPath =
      (fileInfo.isFile() ? fileInfo.absolutePath() : dirInfo.absolutePath()) +
      "/";
  if (m_projectName.isEmpty()) {
    qDebug() << fileInfo.absolutePath() << ":"
             << fileInfo.absolutePath().split("/");
    m_projectName = dirInfo.dirName();
  }
  QString p = m_projectPath;
  p.chop(m_projectName.length() + 1);
  setCellPath(p);
}
/*
bool cs8Application::importVPlusFile(const QString &fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;

  QString line;
  enum SectionType { None, Locations, Strings };
  SectionType currSection;
  QStringList components;
  QString varName, index;
  QRegularExpression rx("\\[(.*)\\]");
  cs8Variable *var;
  while (!file.atEnd()) {
    line = file.readLine().trimmed();
    if (line.startsWith(".LOCATIONS")) {
      currSection = Locations;
      line = file.readLine();
    } else if (line.startsWith(".STRINGS")) {
      currSection = Strings;
      line = file.readLine();
    } else if (line.startsWith(".END")) {
      currSection = None;
    }

    //
    switch (currSection) {
    case Locations: {
      components = line.split(" ", QString::SkipEmptyParts);
      varName = components.at(0).trimmed();
      bool isJoint = varName.startsWith("#");
      // extract array index
      index = rx.match(varName).captured(1);
      index = index.isEmpty() ? QString("0") : index;
      varName.remove(rx);
      // sanitize file name
      varName = sanitizeSymbolName(isJoint ? varName.remove(0, 1) : varName);
      // remove name from list of components
      components.removeAt(0);

      if (isJoint) {
        // location is a joint variable
        varName.prepend("j");
        if ((var = globalVariableModel()->getVarByName(varName)) == nullptr)
          var = new cs8Variable(this);
        var->setScope(cs8Variable::Global);
        var->setType("jointRx");
        var->setName(varName);
        QMap<QString, QString> map;
        map.insert("j1", components.at(0));
        map.insert("j2", QString("%1").arg(components.at(1).toDouble() + 90.0));
        map.insert("j3", QString("%1").arg(components.at(2).toDouble() - 90.0));
        map.insert("j4", components.at(3));
        map.insert("j5", components.at(4));
        map.insert("j6", components.at(5));

        var->setValue(index, map);
        globalVariableModel()->addVariable(var);
      } else {
        // location is a point variable
        varName.prepend("p");
        if ((var = globalVariableModel()->getVarByName(varName)) == nullptr) {
          var = new cs8Variable(this);
          var->setScope(cs8Variable::Global);
        }

        // read next line in location file containing matrix components
        line = file.readLine().trimmed();
        components << line.split(" ", QString::SkipEmptyParts);

        QList<float> matrixRot, matrixTrans;
        bool ok;
        foreach (QString s, components)
          matrixRot.append(s.toFloat(&ok));

        matrixTrans = matrixRot.mid(matrixRot.size() - 3, 3);

        QMatrix4x4 matrix(matrixRot[0], matrixRot[3], matrixRot[6],
                          matrixRot[9], matrixRot[1], matrixRot[4],
                          matrixRot[7], matrixRot[10], matrixRot[2],
                          matrixRot[5], matrixRot[8], matrixRot[11], 0, 0, 0,
                          1);
        Qt3DCore::QTransform trans;
        trans.setMatrix(matrix);
        QMap<QString, QString> map;

        float rx, ry, rz;
        rx = trans.rotationX();
        ry = trans.rotationY();
        rz = trans.rotationZ();
        // rx = atan2(matrixRot[5], matrixRot[8]);
        // ry = atan2(-matrixRot[2], sqrtf(matrixRot[5] * matrixRot[5] +
        // matrixRot[8] * matrixRot[8])); rz = atan2(matrixRot[1],
        // matrixRot[0]);

        // if name contains "grip" assume it is a TCP
        if (varName.contains("grip", Qt::CaseInsensitive)) {
          var->setType("tool");
          map.insert("fatherId", "flange[0]");
          map.insert("ioLink", "valve1");
        } else {
          var->setType("pointRx");
          map.insert("fatherId", "world[0]");
        }
        var->setName(varName);

        map.insert("x", QString("%1").arg(trans.translation().x()));
        map.insert("y", QString("%1").arg(trans.translation().y()));
        map.insert("z", QString("%1").arg(trans.translation().z()));
        map.insert("rx", QString("%1").arg(rx));
        map.insert("ry", QString("%1").arg(ry));
        map.insert("rz", QString("%1").arg(rz));

        var->setValue(index, map);
        globalVariableModel()->addVariable(var);
      }
    } break;

    default:
      break;
    }
  }
  qDebug() << globalVariableModel()->toDtxDocument();
  return true;
}
*/

QString cs8Application::briefModuleDocumentation() const {
  return m_briefModuleDocumentation;
}

void cs8Application::setBriefModuleDocumentation(
    const QString &briefModuleDocumentation) {
  m_briefModuleDocumentation = briefModuleDocumentation;
}

QString cs8Application::getProjectVersion() const { return m_projectVersion; }

void cs8Application::setProjectVersion(const QString &projectVersion) {
  m_projectVersion = projectVersion;
}

QString cs8Application::getProjectStackSize() const {
  return m_projectStackSize;
}

void cs8Application::setProjectStackSize(const QString &projectStackSize) {
  m_projectStackSize = projectStackSize;
}

QString cs8Application::getProjectMillimeterUnit() const {
  return m_projectMillimeterUnit;
}

void cs8Application::setProjectMillimeterUnit(
    const QString &projectMillimeterUnit) {
  m_projectMillimeterUnit = projectMillimeterUnit;
}

bool cs8Application::open(const QString &pfxFilePath) {
  // qDebug() << Q_FUNC_INFO << "path:" << pfxFilePath;
  QString pth = QDir::fromNativeSeparators(pfxFilePath);
  if (pth.startsWith("Disk://")) {
    pth.replace(QString("Disk://"), m_cellPath + "/usr/usrapp/");
    pth = QDir::cleanPath(pth);
  }
  m_moduleDocumentation = "";
  m_briefModuleDocumentation = "";
  m_exportDirectives.clear();
  m_pragmaList.clear();

  QDomDocument doc("Project");
  QFile file(pth);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Open project file: " << pth << ":" << file.errorString();
    return false;
  }
  if (!doc.setContent(&file)) {
    file.close();
    return false;
  }
  file.close();

  m_projectName = QFileInfo(pth).baseName();

  setProjectPath(pth);
  loadProjectData();
  bool result = parseProject(doc);
  setModified(false);
  return result;
}

bool cs8Application::openFromPathName(const QString &filePath) {
  QString filePath_ = filePath;
  if (filePath.startsWith("Disk://"))
    filePath_.replace("Disk://", m_cellPath + "usr/usrapp/");
  filePath_ = QDir::cleanPath(filePath_);
  QString name = filePath_;
  if (!filePath_.endsWith(".pjx"))
    name =
        filePath_ + "/" +
        filePath_.right(filePath_.length() - filePath_.lastIndexOf("/", -2)) +
        ".pjx";
  // qDebug() << Q_FUNC_INFO << ":" << name;
  bool result = open(name);
  return result;
}

bool cs8Application::exportInterfacePrototype(const QString &path) const {
  QString fileName;

  qDebug() << "export prototype to : " << path + "/" + m_projectName;
  qDebug() << "saving dtx";
  // create dest dir
  QDir dir(path + "/" + m_projectName);
  dir.mkpath(path + "/" + m_projectName);
  // delete existing files in dir
  QStringList files = dir.entryList(QDir::Files);
  foreach (QString applicationFile, files) { dir.remove(applicationFile); }
  // save data file
  fileName = path + "/" + m_projectName + "/" + m_projectName + ".dtx";
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly))
    return false;

  QTextStream stream(&file);

  // stream << m_dataDoc.toByteArray();
  stream << m_globalVariableModel->toDtxDocument();
  // qDebug() << m_dataDoc.toString();
  file.close();
  qDebug() << "saving dtx done";
  qDebug() << "saving pjx";

  // save pjx file
  QDomDocument doc;
  QDomProcessingInstruction process = doc.createProcessingInstruction(
      "xml", R"(version="1.0" encoding="utf-8")");
  doc.appendChild(process);
  QDomElement projectElement = doc.createElement("Project");
  projectElement.setAttribute("xmlns",
                              "http://www.staubli.com/robotics/VAL3/Project/3");
  doc.appendChild(projectElement);

  QDomNode parameters = m_parameters.cloneNode();
  projectElement.appendChild(parameters);

  QDomElement programSection = doc.createElement("Programs");
  projectElement.appendChild(programSection);

  foreach (cs8Program *program, m_programModel->publicPrograms()) {
    QDomElement programElement = doc.createElement("Program");
    programElement.setAttribute("file", program->fileName());
    programSection.appendChild(programElement);

    program->save(path + "/" + m_projectName + "/", false);
  }

  QDomNode dataSection = m_dataSection.cloneNode();
  projectElement.appendChild(dataSection);

  /*
     QDomNode aliasSection = m_aliasSection.cloneNode();
     projectElement.appendChild(aliasSection);
   */
  // for interface prototype no libraries are needed
  QDomElement aliasSection = doc.createElement("Libraries");
  projectElement.appendChild(aliasSection);

  QDomNode types = m_typesSection.cloneNode();
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

bool cs8Application::integrateInterface(cs8Application *sourceApplication) {
  // integrate global symbols
  foreach (cs8Variable *globalSourceVariable,
           sourceApplication->globalVariableModel()->publicVariables()) {
    // the global variable already exist in aplication
    if (globalVariableModel()->getVarByName(globalSourceVariable->name())) {
      cs8Variable *var =
          globalVariableModel()->getVarByName(globalSourceVariable->name());
      // check if there's a variable type conflict
      if (var->type() != globalSourceVariable->type()) {
        // the variable types do not match
        qDebug() << "The variable types do not match of variable "
                 << var->name() << ":" << var->type() << " versus "
                 << globalSourceVariable->type();
        return false;
      }
      // check if variable dimension
      if (var->dimensionCount() != globalSourceVariable->dimensionCount()) {
        // the variable dimensions do not match
        qDebug() << "The variable dimensions do not match of variable "
                 << var->name() << ":" << var->dimensionCount() << " versus "
                 << globalSourceVariable->dimensionCount();
        return false;
      }
      // set variable to public
      if (!var->isPublic())
        var->setPublic(true);
    } else {
      auto *var = new cs8Variable(globalSourceVariable);
      globalVariableModel()->addVariable(var);
    }
  }
  // integrate user types
  foreach (cs8LibraryAlias *sourceAlias,
           sourceApplication->typeModel()->list()) {
    // the type definition already exists
    if (typeModel()->getAliasByName(sourceAlias->name())) {
      cs8LibraryAlias *a = typeModel()->getAliasByName(sourceAlias->name());
      // check if there is a path conflict
      if (a->path() != sourceAlias->path()) {
        // the paths to the type do not match
        qDebug() << "The paths to the type do not match " << a->name() << ":"
                 << a->path() << " versus " << sourceAlias->name();
        return false;
      }
    } else {
      // add the user type
      typeModel()->add(sourceAlias->name(), sourceAlias->path());
    }
  }

  // integrate public programs
  foreach (cs8Program *sourceProgram,
           sourceApplication->programModel()->publicPrograms()) {
    if (programModel()->getProgramByName(sourceProgram->name())) {

    } else {
      // create program
      cs8Program *program =
          programModel()->createProgram(sourceProgram->name());
      // copy parameter model
      program->copyFromParameterModel(sourceProgram->parameterModel());
    }
  }

  return true;
}

QString cs8Application::name() const { return m_projectName; }

void cs8Application::setName(const QString &name) {
  if (!m_projectName.isEmpty()) {
    m_projectPath.chop(m_projectName.length() + 1);
    m_projectPath += name + "/";
  }
  m_projectName = name;
}

QString cs8Application::exportToCImplementation() const {
  QStringList out;

  out << QString("#include <%1.h>\n").arg(m_projectName);

  out << "\n";

  foreach (cs8Program *program, m_programModel->programList()) {
    out << QString("   void %3::%1{\n%2\n}")
               .arg(program->definition())
               .arg(program->toCSyntax())
               .arg(m_projectName);
  }

  return out.join("\n");
}

QString cs8Application::exportToCDefinition() const {
  QStringList out;

  out << "\n";

  if (!m_mainPageDocumentation.isEmpty()) {
    out << "///\\mainpage\n";
    out << mainPageDocumentationFormatted();
  }

  if (!m_briefModuleDocumentation.isEmpty()) {
    out << "///\\brief\n";
    out << briefModuleDocumentationFormatted();
  }

  if (!m_moduleDocumentation.isEmpty()) {
    out << "///\\details\n";
    out << moduleDocumentationFormatted();
  }

  out << QString("class %1{").arg(m_projectName);
  out << " public:";

  foreach (cs8Program *program, m_programModel->publicPrograms()) {
    out << program->documentation();
    out << QString("   void %1;").arg(program->definition());
  }

  QMap<QString, QMap<QString, QString> *> constSets = getEnumerations();
  QMap<QString, QString> *constSet;

  QMapIterator<QString, QMap<QString, QString> *> j(constSets);
  while (j.hasNext()) {
    j.next();
    constSet = j.value();
    out << QString("/*!\n * \\enum %1 Enumeration \n*/").arg(j.key());
    out << QString("    enum %1 {").arg(j.key());
    QMapIterator<QString, QString> i(*constSet);
    while (i.hasNext()) {
      i.next();

      QString doc;
      cs8Variable *var = m_globalVariableModel->getVarByName(i.value());
      if (var != nullptr) {
        doc = var->description();
        doc = doc.replace("\n", "\\n");
      } else {
        doc = "";
      }
      out << QString("     %1 = %4%3 /*!< %4: %2 */\n")
                 .arg(i.value())                   // 1: name of variable
                 .arg(doc)                         // 2: documentation
                 .arg(i.hasNext() ? "," : " ")     // 3: , for next element
                 .arg(var->varValue().toString()); // 4: value
    }
    out << QString("   };\n\n");
  }

  foreach (cs8Variable *variable, m_globalVariableModel->publicVariables()) {
    if (!variable->hasConstPrefix()) {
      out << variable->documentation(true, true);
      out << QString("   %1;").arg(variable->definition());
    }
  }

  out << " private:";
  foreach (cs8LibraryAlias *alias, m_libraryAliasModel->list()) {
    out << alias->documentation();
    out << QString("   %1;").arg(alias->definition());
    out.insert(0, QString("#include <%1.h>").arg(alias->name()));
  }
  foreach (cs8Variable *variable, m_globalVariableModel->privateVariables()) {
    if (!variable->hasConstPrefix()) {
      out << variable->documentation(true, true);
      out << QString("   %1;").arg(variable->definition());
    }
  }

  foreach (cs8Program *program, m_programModel->privatePrograms()) {
    out << program->documentation(true, true);
    out << QString("   void %1;").arg(program->definition());
  }

  out << "}";

  return out.join("\n");
}

QString cs8Application::exportToIOList() const {
  QString data;

  foreach (cs8Variable *variable, m_globalVariableModel->variableListByType(
                                      QRegularExpression("[asd]io"))) {
    data += QString("%1;%2;%3;%4\n")
                .arg(variable->name())
                .arg(variable->type())
                .arg(variable->size())
                .arg(variable->documentation(false, false).remove("\n"));
  }
  return data;
}

/*!
 *
 */
void cs8Application::exportToCppFile(const QString &path) const {
  QString fileName;

  // qDebug() << "export Cpp interface to : " << path + " : " + m_projectName;
  // qDebug() << "saving Cpp file";
  fileName = path + "/" + m_projectName + ".cpp";
  QFile file;
  file.setFileName(fileName);
  if (!file.open(QFile::WriteOnly)) {
    qDebug() << "Couldn't write Cpp file to file " << path
             << file.errorString();
    return;
  }
  QTextStream stream(&file);
  stream << exportToCImplementation();
  file.close();
}

void cs8Application::exportToHFile(const QString &path) const {
  QString fileName;

  // qDebug() << "export  interface to : " << path + " : " + m_projectName;
  // qDebug() << "saving h file";
  fileName = path + "/" + m_projectName + ".h";
  QFile file;
  file.setFileName(fileName);
  if (!file.open(QFile::WriteOnly)) {
    qDebug() << "Couldn't write Cpp file to file " << path
             << file.errorString();
    return;
  }
  QTextStream stream(&file);
  stream << exportToCDefinition();
  file.close();
}

//!
//! \brief sanitizeSymbolName
//! \param varName
//! \return sanitized symbol name
//!
QString cs8Application::sanitizeSymbolName(const QString &varName) {
  QStringList c = varName.split(".", QString::SkipEmptyParts);
  for (int i = 0; i < c.count(); i++) {
    c[i][0] = c[i].at(0).toTitleCase();
  }
  return c.join("");
}

/*!
   \fn cs8Application::parseProject(const QDomDocument & doc)
 */
bool cs8Application::parseProject(const QDomDocument &doc) {
  m_programModel->clear();
  m_programModel->setCellPath(m_cellPath);
  m_globalVariableModel->clear();
  m_libraryAliasModel->clear();
  m_typeModel->clear();

  QDomElement rootElement = doc.documentElement();

  m_parameters = rootElement.elementsByTagName("Parameters").at(0).toElement();
  m_projectVersion = m_parameters.attribute("version", "7.0");
  m_projectStackSize = m_parameters.attribute("stackSize", "5000");
  m_projectMillimeterUnit = m_parameters.attribute("millimeterUnit", "true");
  m_programSection =
      rootElement.elementsByTagName("Programs").at(0).toElement();
  m_dataSection = rootElement.elementsByTagName("Database").at(0).toElement();
  m_aliasSection = rootElement.elementsByTagName("Libraries").at(0).toElement();
  m_typesSection = rootElement.elementsByTagName("Types").at(0).toElement();

  // load data file
  QDomElement data = m_dataSection.elementsByTagName("Data").at(0).toElement();
  if (loadDataFile(m_projectPath + QDir::separator() +
                   data.attribute("file"))) {

    // load programs
    QDomNodeList list = m_programSection.elementsByTagName("Program");
    for (int i = 0; i < list.count(); i++) {
      QString fileName = list.at(i).toElement().attribute("file");
      m_programModel->addProgramFile(m_projectPath + fileName);
    }
    // load alias
    list = m_aliasSection.elementsByTagName("Library");
    for (int i = 0; i < list.count(); i++) {
      m_libraryAliasModel->add(this, list.at(i).toElement());
    }
    // load types
    list = m_typesSection.elementsByTagName("Type");
    for (int i = 0; i < list.count(); i++) {
      m_typeModel->add(list.at(i).toElement());
    }

    foreach (cs8Program *prog, m_programModel->programList()) {
      prog->setWithUndocumentedSymbols(m_withUndocumentedSymbols);
    }

    // try to read version information
    // check if global variable sVersion exists
    m_version = "";
    cs8Variable *var = globalVariableModel()->getVarByName("sVersion");
    if (var != nullptr) {
      m_version = var->varValue("0").toString();
    } else {
      // check if version.dat exists
      QFile file(m_projectPath + "version.dat");
      if (file.open(QFile::ReadOnly)) {
        m_version = file.readAll();
        m_version = m_version.trimmed();
      }
    }
    // if version info found check if build.dat exists
    if (!m_version.isEmpty()) {
      QFile file(m_projectPath + "build.dat");
      if (file.open(QFile::ReadOnly)) {
        QString build = file.readAll();
        m_version = m_version + "r" + build.trimmed();
      }
    }
    //
    foreach (auto testVar, m_globalVariableModel->variableListByType()) {
      foreach (auto var, m_globalVariableModel->variableListByType()) {
        if (var->type() == "pointRx" || var->type() == "pointRs" ||
            var->type() == "frame")
          if (var->father().contains(testVar->name()))
            testVar->addSymbolReference(0, 0, var->type() + " " + var->name());

        if (var->type() == "tool")
          if (var->father().contains(testVar->name()))
            testVar->addSymbolReference(0, 0, var->type() + " " + var->name());
      }
    }
    m_callList = buildCallList();
    m_programModel->updateCodeModel();
    return true;
  } else {
    qDebug() << "failed to parse project file of application" << name();
    return false;
  }
}

void cs8Application::slotGlobalVariableDocumentationFound(
    const QString &name, const QString &document) {

  // qDebug() << "find: " << name;
  cs8Variable *variable = m_globalVariableModel->getVarByName(name);
  if (variable != nullptr) {
    // qDebug() << "setting doc for " << name << " to " << document;
    variable->setDescription(document);
  }
}

void cs8Application::slotModuleDocumentationFound(const QString &document) {
  m_moduleDocumentation = document;
}

void cs8Application::slotBriefModuleDocumentationFound(
    const QString &document) {
  m_briefModuleDocumentation = document;
}

void cs8Application::slotMainPageDocumentationFound(const QString &document) {
  m_mainPageDocumentation = document;
}

void cs8Application::slotExportDirectiveFound(const QString &module,
                                              const QString &function) {
  qDebug() << "export " << module << ":" << function;
  m_exportDirectives.insert(function, module);
}

void cs8Application::slotUnknownTagFound(const QString &tagType,
                                         const QString &tagName,
                                         const QString &tagText) {
  if (tagType == "pragma")
    m_pragmaList.insert(tagName, tagText);
}

void cs8Application::setModified(bool modified_) {
  m_modified = modified_;
  for (auto prog : m_programModel->programList()) {
    prog->setIsModified(false);
  }
  emit modified(modified_);
}

bool cs8Application::getCompactFileMode() const { return m_compactFileMode; }

void cs8Application::setCompactFileMode(bool singleProgramFile) {
  m_compactFileMode = singleProgramFile;
}

bool cs8Application::loadDocumentationFile(const QString & /*fileName*/) {
  if (m_programModel->getProgramByName("_globals")) {
  }
  return false;
}

cs8ProgramModel *cs8Application::programModel() const { return m_programModel; }

cs8LibraryAliasModel *cs8Application::libraryModel() const {
  return m_libraryAliasModel;
}

bool cs8Application::loadDataFile(const QString &fileName) {
  // qDebug() << "Loading data file: " << fileName;

  QDomDocument doc("data");
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  if (!doc.setContent(&file)) {
    file.close();
    return false;
  }
  /// TODO: quick fix to load data
  // m_dataDoc = doc;
  m_globalVariableModel->clear();
  QDomElement rootElement = doc.documentElement();
  QDomNodeList sectionList = rootElement.childNodes();
  for (int i = 0; i < sectionList.count(); i++) {
    QDomElement sectionElement = sectionList.at(i).toElement();
    // qDebug() << "Reading section: " << sectionElement.tagName();
    QDomNodeList variableList = sectionElement.childNodes();
    for (int j = 0; j < variableList.count(); j++) {
      QDomElement variableElement = variableList.at(j).toElement();
      m_globalVariableModel->addVariable(variableElement);
    }
  }
  file.close();
  return true;
}

bool cs8Application::saveDataFile(const QString &fileName) {
  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);
  QXmlStreamWriter stream(&buffer);
  stream.setAutoFormatting(true);
  stream.setAutoFormattingIndent(2);
  stream.setCodec("utf-8");
  stream.writeStartDocument();

  stream.writeStartElement("Database");
  stream.writeAttribute("xmlns:xsi",
                        "http://www.w3.org/2001/XMLSchema-instance");
  stream.writeAttribute("xmlns", "http://www.staubli.com/robotics/VAL3/Data/2");

  m_globalVariableModel->writeXMLStream(stream);

  stream.writeEndDocument();

  // match our XML output to XML output of SRS
  buffer.buffer().replace("encoding=\"UTF-8", "encoding=\"utf-8");
  if (buffer.buffer().right(1) == "\n")
    buffer.buffer().chop(1);
  //
  // insert BOM
  buffer.buffer().insert(0, static_cast<char>(0xBF));
  buffer.buffer().insert(0, static_cast<char>(0xBB));
  buffer.buffer().insert(0, static_cast<char>(0xEF));
  //
  QString fileName_ = fileName;
  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly))
    return false;
  file.write(buffer.buffer());
  return true;
}

bool cs8Application::save(const QString &path, const QString &name,
                          bool compactMode) {
  // if (!path.isEmpty())
  m_projectPath = path;
  //  if (!name.isEmpty())
  m_projectName = name;
  m_compactFileMode = compactMode;

  // check if a global data has documented code or a application documentation
  // exists.
  // If it does, we select program zzDocumentation() - if it does not exist yet
  // we append it here
  if (m_globalVariableModel->hasDocumentation() ||
      !m_moduleDocumentation.isEmpty() ||
      !m_briefModuleDocumentation.isEmpty() ||
      !m_mainPageDocumentation.isEmpty()) {
    if (!m_programModel->getProgramByName("zzDocumentation"))
      m_programModel->createProgram("zzDocumentation");

    cs8Program *prog = m_programModel->getProgramByName("zzDocumentation");
    prog->setGlobalDocContainer(true);
    if (prog->briefDescription(true).isEmpty())
      prog->setBriefDescription("This program contains documentation tags");
    prog->clearDocumentationTags();

    if (!m_briefModuleDocumentation.isEmpty() || true) {
      prog->setBriefModuleDocumentation(m_briefModuleDocumentation);
    }

    if (!m_moduleDocumentation.isEmpty() || true) {
      prog->setApplicationDocumentation(moduleDocumentationFormatted(""));
    }

    if (!m_mainPageDocumentation.isEmpty() || true) {
      prog->setMainPageDocumentation(m_mainPageDocumentation);
    }

    foreach (const cs8Variable *globalVar,
             m_globalVariableModel->variableListByType()) {
      QString doc = globalVar->documentation(false, false);
      // qDebug() << "global var: " << globalVar->name() << ":" << doc;
      if (!doc.isEmpty() || m_withUndocumentedSymbols) {
        prog->addTag("global", globalVar->name(), doc);
      }
      if (!globalVar->linterDirective().isEmpty())
        prog->addTag("linter", globalVar->linterDirective(), globalVar->name());
    }
  }
  QDir dir;
  dir.mkpath(m_projectPath);
  if (!writeProjectFile())
    return false;
  if (!saveDataFile(m_projectPath + m_projectName + ".dtx"))
    return false;
  if (m_compactFileMode) {
    m_programModel->setHasByteOrderMark(true);
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    QXmlStreamWriter stream(&buffer);
    stream.setAutoFormatting(true);
    stream.setAutoFormattingIndent(2);
    stream.setCodec("utf-8");
    stream.writeStartDocument();

    //
    stream.writeStartElement("Programs");
    stream.writeAttribute("xmlns:xsi",
                          "http://www.w3.org/2001/XMLSchema-instance");
    stream.writeAttribute("xmlns",
                          "http://www.staubli.com/robotics/VAL3/Program/2");
    for (const auto program : m_programModel->programList()) {
      program->writeXMLStream(stream, true);
      program->deleteSourceFile();
    }
    stream.writeEndDocument();

    // match our XML output to XML output of SRS
    buffer.buffer().replace("encoding=\"UTF-8", "encoding=\"utf-8");
    if (buffer.buffer().right(1) == "\n")
      buffer.buffer().chop(1);
    buffer.buffer().replace("\n", "\r\n");
    buffer.buffer().replace("\"/>", "\" />");
    //

    QFile file(m_projectPath + "/" + m_projectName + ".pgx");
    if (!file.open(QIODevice::WriteOnly))
      return false;

    if (m_programModel->getHasByteOrderMark()) {
      buffer.buffer().insert(0, static_cast<char>(0xBF));
      buffer.buffer().insert(0, static_cast<char>(0xBB));
      buffer.buffer().insert(0, static_cast<char>(0xEF));
    }
    file.write(buffer.buffer());
  } else {
    foreach (cs8Program *program, m_programModel->programList()) {
      // qDebug() << __FUNCTION__ << " Save program: " << program->name();
      if (!program->save(m_projectPath, true))
        return false;
    }
  }
  if (!saveProjectData())
    return false;

  setModified(false);
  return true;
}

QString cs8Application::projectPath(bool cs8Format) const {
  if (cs8Format) {
    QString pth = QDir::fromNativeSeparators(m_projectPath);
    pth = pth.replace(QDir::fromNativeSeparators(m_cellPath) + "usr/usrapp/",
                      "Disk://");
    return pth;
  } else {
    return m_projectPath;
  }
}

bool cs8Application::save(bool compactMode) {
  return save(m_projectPath, m_projectName, compactMode);
}

bool cs8Application::save() {
  return save(m_projectPath, m_projectName, m_compactFileMode);
}

QString
cs8Application::moduleDocumentationFormatted(const QString &withSlashes) const {
  return formattedDocument(m_moduleDocumentation, withSlashes);
}

QString cs8Application::mainPageDocumentationFormatted(
    const QString &withSlashes) const {
  return formattedDocument(m_mainPageDocumentation, withSlashes);
}

QString cs8Application::briefModuleDocumentationFormatted(
    const QString &withSlashes) const {
  return formattedDocument(m_briefModuleDocumentation, withSlashes);
}

QString cs8Application::formattedDocument(const QString &doc,
                                          const QString &withSlashes) const {
  QString out;
  QStringList list;

  list << doc.split("\n");
  bool inCodeSection = false;
  int indentation = 0;
  QString indentText;
  foreach (QString str, list) {
    if (str.contains("<pre>")) {
      inCodeSection = true;
      // out += withSlashes + " \n";
    }
    if (str.contains("</pre>")) {
      inCodeSection = false;
      // out += withSlashes + " \n";
    }
    if (inCodeSection) {
      // qDebug() << str << "indent: " << indentation;
      if (str.simplified().indexOf(QRegExp(
              R"(^\s*(begin|if\s|for\s|while\s|(do\s*$)|switch\s|case\s|default))")) ==
          0) {
        str = indentText + str.trimmed();
        indentation++;
        indentText = "";
        for (int i = 0; i < indentation; i++)
          indentText += " ";
        // qDebug() << "increase indent";

      } else if (str.simplified().indexOf(QRegExp(
                     "^\\s*(end(If|While|For|Switch)|until|break|else)")) ==
                 0) {
        indentation = qMax(0, --indentation);
        // qDebug() << "decrease indent to " << indentation;
        indentText = "";
        for (int i = 0; i < indentation; i++)
          indentText += " ";
        str = indentText + str.trimmed();
        // we need to increase indentation after "else" again
        if (str.simplified().indexOf(QRegExp("^\\s*(else)")) == 0) {
          indentation++;
          indentText = "";
          for (int i = 0; i < indentation; i++)
            indentText += " ";
        }
      } else
        str = indentText + str.trimmed();
    }

    // out += str.isEmpty()
    //           ? "\n"
    //           : (withSlashes + str + (inCodeSection ? "" : "") + "\n");
    out += withSlashes + str + "\n";
  }
  out += out.isEmpty() ? "" : "\n";
  return out;
}

QMap<QString, QMap<QString, QString> *>
cs8Application::getEnumerations() const {

  QString prefix;
  QMap<QString, QString> *constSet;
  QMap<QString, QMap<QString, QString> *> constSets;
  foreach (cs8Variable *var, m_globalVariableModel->variableListByType()) {
    if (var->hasConstPrefix(&prefix) &&
        (var->type() == "num" || var->type() == "string")) {

      // qDebug() << "Found prefix " << prefix << ": " << var->name();
      if (!constSets.contains(prefix)) {
        constSet = new QMap<QString, QString>();
        constSets.insert(prefix, constSet);
      }
      if (var->size(0) > 1) {
        for (uint i = 0; i < var->size(0); i++) {
          QString value = var->varValue(QString("%1").arg(i)).toString();
          constSets.value(prefix)->insertMulti(
              value, var->name() + QString("[%1]").arg(i));
        }
      } else {
        QString value = var->varValue("0").toString();
        constSets.value(prefix)->insertMulti(value, var->name());
      }
    }
  }
  // remove enumerations with only 1 value
  QMapIterator<QString, QMap<QString, QString> *> i(constSets);
  while (i.hasNext()) {
    i.next();

    constSet = i.value();
    if (constSet->values().size() < 2)
      constSets.remove(i.key());
  }
  return constSets;
}

void cs8Application::checkPrograms(QStringList &output) {
  foreach (cs8Program *program, m_programModel->programList()) {
    foreach (cs8Variable *lvar,
             program->localVariableModel()->variableListByType()) {
      if (!program->referencedVariables().contains(lvar->name())) {
        output.append(
            QString(
                "<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>1<msg>%2<file>%3")
                .arg(program->name())
                .arg("Warning: Local variable '" + lvar->name() +
                     "' is not used")
                .arg(program->cellFilePath()));
      }
    }

    if (reportParametersPostfix) {
      foreach (cs8Variable *lvar,
               program->parameterModel()->variableListByType()) {
        if (!lvar->name().endsWith('_')) {
          output.append(QString("<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>1<"
                                "msg>%2<file>%3")
                            .arg(program->name())
                            .arg("Warning: Parameter '" + lvar->name() +
                                 "' does not end with '_'")
                            .arg(program->cellFilePath()));
        }
      }
    }

    if (reportToDos) {
      QMapIterator<int, QString> i(program->todos());
      while (i.hasNext()) {
        i.next();
        output.append(
            QString(
                "<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>%4<msg>%2<file>%3")
                .arg(program->name())
                .arg("Warning: " + i.value())
                .arg(program->cellFilePath())
                .arg(i.key()));
      }
    }
  }
}

QStringList cs8Application::checkEnumerations() const {
  QStringList output;
  QMap<QString, QMap<QString, QString> *> constSets = getEnumerations();

  QMap<QString, QString> *constSet;
  foreach (constSet, constSets) {
    QMapIterator<QString, QString> i(*constSet);
    while (i.hasNext()) {
      i.next();
      if (constSet->values(i.key()).size() > 1) {
        QString msg;
        QString firstName;
        for (const auto &key : constSet->values(i.key())) {
          if (m_globalVariableModel->findVariableByName(key)
                  ->linterDirective() != "ignore") {
            if (msg.isEmpty())
              firstName = key;
            msg += key + ", ";
          }
        }
        msg.chop(2);
        if (!msg.isEmpty())
          output.append(QString("<level>Error<CLASS>DATA<P1>%1<P2>%5<line>%4<"
                                "msg>%2<file>%3")
                            .arg("")
                            .arg("Global variables '" + msg +
                                 "' have the same value '" + i.key() + "'")
                            .arg(cellDataFilePath(true))
                            .arg(0)
                            .arg(firstName));
        break;
      }
    }
  }
  return output;
}

QMap<QString, QString> cs8Application::ambigousEnumerationItems() const {
  QMap<QString, QString> output;
  QMap<QString, QMap<QString, QString> *> constSets = getEnumerations();

  QMap<QString, QString> *constSet;
  foreach (constSet, constSets) {
    QMapIterator<QString, QString> i(*constSet);
    while (i.hasNext()) {
      i.next();
      if (constSet->values(i.key()).size() > 1) {
        QString msg;
        for (const auto &key : constSet->values(i.key())) {
          msg += key + ", ";
        }
        msg.chop(2);
        output.insert(msg, i.key());
        break;
      }
    }
  }
  return output;
}

QMap<QString, QList<cs8Program *>> cs8Application::buildCallList() {
  QMap<QString, QList<cs8Program *>> callList;
  // check if a local variable hides the global variable
  foreach (cs8Program *program, m_programModel->programList()) {
    QStringList list = program->getCalls();

    foreach (QString callName, list) {
      callList[program->name()] << programModel()->getProgramByName(callName);
    }
  }

  return callList;
}

QStringList cs8Application::getCallList(cs8Program *program) const {
  QStringList list;
  //
  QMapIterator<QString, QList<cs8Program *>> i(m_callList);
  while (i.hasNext()) {
    i.next();
    for (auto prog : i.value()) {
      if (prog == program)
        list << i.key();
    }
  }
  list.removeDuplicates();
  return list;
}

QString cs8Application::getProjectPath() const { return m_projectPath; }

void cs8Application::checkObsoleteProgramFiles(QStringList &output) {
  // check for obsolete PGX files
  // retrieve list of pgx files from file system

  QDir dir;
  dir.setPath(projectPath());
  QStringList pgxFileList = dir.entryList(QStringList() << "*.pgx");
  foreach (QString pgxFileName, pgxFileList) {
    bool found = false;
    foreach (cs8Program *program, m_programModel->programList()) {
      if (program->fileName() == pgxFileName)
        found = true;
    }
    if (!found) {
      output.append(
          QString(
              "<level>Warning<CLASS>PRG<P1>%1<P2>CODE<line>%4<msg>%2<file>%3")
              .arg(pgxFileName, "Warning: Program file is obsolete", "", ""));
    }
  }
}

QString cs8Application::moduleDocumentation() const {
  return m_moduleDocumentation;
}

void cs8Application::setModuleDocumentation(
    const QString &applicationDocumentation) {
  m_moduleDocumentation = applicationDocumentation;
}

void cs8Application::setCellPath(const QString &path) {
  QString pth = QDir::fromNativeSeparators(path);
  int pos = 0;

  if ((pos = pth.indexOf(QDir::fromNativeSeparators("usr/usrapp"))) != -1)
    m_cellPath = pth.left(pos - 1);
  else
    m_cellPath = QDir::currentPath();
  m_cellPath = QDir::cleanPath(m_cellPath) + "/";
  if (m_projectPath.isEmpty())
    m_projectPath =
        m_cellPath +
        ((QDir::toNativeSeparators(m_cellPath)
              .indexOf(QDir::toNativeSeparators("usr/usrapp")) != -1)
             ? "/"
             : "/usr/usrapp/") +
        m_projectName + QDir::separator();
  // qDebug() << __FUNCTION__ << m_cellPath;
}

QString cs8Application::cellPath() const { return m_cellPath; }

QString cs8Application::cellProjectFilePath(bool cs8Format) const {
  QString pth =
      QDir::toNativeSeparators(m_projectPath + m_projectName + ".pjx");
  qDebug() << pth;
  if (cs8Format)
    pth = pth.replace(QDir::toNativeSeparators(m_cellPath + "usr/usrapp/"),
                      "Disk://");
  qDebug() << pth;
  pth = QDir::fromNativeSeparators(pth);
  qDebug() << pth;
  return pth;
}

QString cs8Application::cellDataFilePath(bool cs8Format) const {
  QString pth =
      QDir::toNativeSeparators(m_projectPath + m_projectName + ".dtx");
  pth = pth.replace(m_cellPath + "/usr/usrapp", "Disk://");
  pth = QDir::toNativeSeparators(pth);
  if (cs8Format) {
    int pos = pth.indexOf("usrapp");
    if (pos > 0) {
      pth.remove(0, pos + 7);
      pth = "Disk://" + pth.replace("\\", "/");
    }
  }
  return pth;
}

QHash<QString, QString> cs8Application::exportDirectives() const {
  return m_exportDirectives;
}

bool cs8Application::isModified() const { return m_modified; }

bool cs8Application::writeProjectFile() {
  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);
  QXmlStreamWriter stream(&buffer);
  stream.setAutoFormatting(true);
  stream.setAutoFormattingIndent(2);
  stream.setCodec("utf-8");
  stream.writeStartDocument();

  stream.writeStartElement("Project");
  stream.writeAttribute("xmlns",
                        "http://www.staubli.com/robotics/VAL3/Project/3");

  stream.writeStartElement("Parameters");
  stream.writeAttribute("version", m_projectVersion);
  stream.writeAttribute("stackSize", m_projectStackSize);
  stream.writeAttribute("millimeterUnit", m_projectMillimeterUnit);
  stream.writeEndElement();

  stream.writeStartElement("Programs");
  if (m_compactFileMode) {
    stream.writeStartElement("Program");
    stream.writeAttribute("file", m_projectName + ".pgx");
    stream.writeEndElement();
  } else {
    for (const auto item : m_programModel->programList()) {
      stream.writeStartElement("Program");
      stream.writeAttribute("file", item->fileName());
      stream.writeEndElement();
    }
  }
  stream.writeEndElement();

  stream.writeStartElement("Database");
  stream.writeStartElement("Data");
  stream.writeAttribute("file", m_projectName + ".dtx");
  stream.writeEndElement();
  stream.writeEndElement();

  stream.writeStartElement("Libraries");
  for (const auto item : m_libraryAliasModel->list()) {
    stream.writeStartElement("Library");
    stream.writeAttribute("alias", item->name());
    stream.writeAttribute("path", item->path());
    if (!item->autoLoad())
      stream.writeAttribute("autoload", item->autoLoadString());
    stream.writeEndElement();
  }
  stream.writeEndElement();

  if (m_typeModel->list().count() > 0) {
    stream.writeStartElement("Types");
    for (const auto item : m_typeModel->list()) {
      stream.writeStartElement("Type");
      stream.writeAttribute("name", item->name());
      stream.writeAttribute("path", item->path());
      stream.writeEndElement();
    }
    stream.writeEndElement();
  }

  stream.writeEndDocument();

  // match our XML output to XML output of SRS
  buffer.buffer().replace("encoding=\"UTF-8", "encoding=\"utf-8");
  if (buffer.buffer().right(1) == "\n")
    buffer.buffer().chop(1);
  //
  // insert BOM
  buffer.buffer().insert(0, static_cast<char>(0xBF));
  buffer.buffer().insert(0, static_cast<char>(0xBB));
  buffer.buffer().insert(0, static_cast<char>(0xEF));
  //
  QString fileName_ = m_projectPath + m_projectName + ".pjx";
  QFile file(fileName_);
  if (!file.open(QIODevice::WriteOnly))
    return false;
  file.write(buffer.buffer());
  return true;
}

/*
 * Move parameters to global variables if name starts with '_'
 */
void cs8Application::moveParamsToGlobals(cs8Program *program) {
  QString newName, oldName;
  QStringList parameters;
  for (int i = 0; i < program->parameterModel()->rvariableList().count(); i++) {
    // if name of parameter starts with '_' remove it from parameter list and
    // add it as a global variable
    if (program->parameterModel()->rvariableList().at(i)->name().startsWith(
            "_")) {
      cs8Variable *var = program->parameterModel()->rvariableList().takeAt(i);

      // program->parameterModel ()->variableList ().removeAt(i);
      newName = var->name();
      oldName = var->name();
      // check if variable has an "call with array index" suffix
      int parameterIndex = 0;
      int arraySize = 1;
      QRegExp rx("(_(A(\\d{1}))_(\\d{1,2}))");
      rx.indexIn(newName);
      // qDebug() << "parameter name to global:" << newName << ":"
      //         << rx.matchedLength();
      if (rx.matchedLength() > 0) {
        parameterIndex = rx.cap(3).toInt();
        arraySize = rx.cap(4).toInt();
        newName.remove(rx);
      }
      var->setScope(cs8Variable::Global);
      var->setPublic(true);
      var->setAllSizes(QString("%1").arg(arraySize));
      //
      // remove leading '_'
      newName.remove(0, 1);
      // remove trailing '_'
      if (newName.endsWith('_'))
        newName.chop(1);
      var->setName(newName, this);
      parameters << newName;
      // add variable to global list if it does not exist yet
      if (!m_globalVariableModel->variableNameList().contains(newName)) {
        m_globalVariableModel->addVariable(var);
      } else {
        // check if we need to update the documentation of the extracted
        // global variable if the global variable does not have a description
        // yet
        if (!var->description().isEmpty()) {
          cs8Variable *globalVar =
              m_globalVariableModel->getVarByName(var->name());
          if (globalVar) {
            if (globalVar->description().isEmpty())
              globalVar->setDescription(var->description());
          }
        }
      }
      QString code = program->val3Code(true);
      // append array index
      if (parameterIndex > 0) {
        newName += "[" + parameters[parameterIndex - 1] + "]";
      }
      code.replace(oldName, newName);
      program->setCode(code);
      i--;
    } else {
      parameters
          << program->parameterModel()->variableListByType().at(i)->name();
    }
  }
  emit modified(true);
}

void cs8Application::createXMLSkeleton(bool S6Format) {
  qDebug() << "Create xml structure";
  if (!S6Format) {
    // Val3 s7 format
    m_XMLDocument = QDomDocument();
    QDomProcessingInstruction process =
        m_XMLDocument.createProcessingInstruction(
            "xml", R"(version="1.0" encoding="utf-8")");
    m_XMLDocument.appendChild(process);

    m_projectSection = m_XMLDocument.createElement("Project");

    m_projectSection.setAttribute(
        "xmlns", "http://www.staubli.com/robotics/VAL3/Project/3");
    m_XMLDocument.appendChild(m_projectSection);

    m_parameters = m_XMLDocument.createElement("Parameters");
    m_parameters.setAttribute("version", m_projectVersion);
    m_parameters.setAttribute("stackSize", m_projectStackSize);
    m_parameters.setAttribute("millimeterUnit", m_projectMillimeterUnit);
    m_projectSection.appendChild(m_parameters);

    m_programSection = m_XMLDocument.createElement("Programs");
    m_projectSection.appendChild(m_programSection);

    m_dataSection = m_XMLDocument.createElement("Database");
    m_projectSection.appendChild(m_dataSection);

    m_aliasSection = m_XMLDocument.createElement("Libraries");
    m_projectSection.appendChild(m_aliasSection);

    m_typesSection = m_XMLDocument.createElement("Types");
    m_projectSection.appendChild(m_typesSection);
  } else {
    // Val3 s6 format
    m_XMLDocument = QDomDocument();
    QDomProcessingInstruction process =
        m_XMLDocument.createProcessingInstruction(
            "xml", R"(version="1.0" encoding="utf-8")");
    m_XMLDocument.appendChild(process);

    m_projectSection = m_XMLDocument.createElement("project");

    m_projectSection.setAttribute("xmlns", "ProjectNameSpace");
    m_XMLDocument.appendChild(m_projectSection);

    m_parameters = m_XMLDocument.createElement("parameters");
    m_parameters.setAttribute("version", "s6");
    m_parameters.setAttribute("stackSize", m_projectStackSize);
    m_parameters.setAttribute("millimeterUnit", m_projectMillimeterUnit);
    m_projectSection.appendChild(m_parameters);

    m_programSection = m_XMLDocument.createElement("programSection");
    m_projectSection.appendChild(m_programSection);

    m_dataSection = m_XMLDocument.createElement("dataSection");
    m_projectSection.appendChild(m_dataSection);

    m_aliasSection = m_XMLDocument.createElement("aliasSection");
    m_projectSection.appendChild(m_aliasSection);
  }
  qDebug() << "Create xml structure done";
}

void cs8Application::setCopyrightMessage(const QString &text) {
  m_copyRightMessage = text;
  foreach (cs8Program *program, m_programModel->programList()) {
    program->setCopyrightMessage(text);
  }
}

cs8VariableModel *cs8Application::globalVariableModel() const {
  return m_globalVariableModel;
}

cs8TypeModel *cs8Application::typeModel() const { return m_typeModel; }

bool cs8Application::loadProjectData() {
  QFile file;
  file.setFileName(m_projectPath + ".projectData/copyright.txt");
  if (file.open(QFile::ReadOnly))
    m_copyRightMessage = file.readAll();
  file.close();
  QSettings setting(m_projectPath + ".projectData/settings.ini",
                    QSettings::IniFormat, this);
  m_withUndocumentedSymbols =
      setting.value("AddTagForNotDocumentedSymbols", false).toBool();
  m_includeLibraryDocuments =
      setting.value("WithLibraryDocuments", false).toBool();
  return true;
}

bool cs8Application::saveProjectData() {
  if (m_copyRightMessage.isEmpty() && !m_withUndocumentedSymbols &&
      !m_includeLibraryDocuments)
    return true;
  QString filePath = m_projectPath + ".projectData";
  QDir dir;
  dir.mkpath(filePath);

  filePath = m_projectPath + ".projectData/copyright.txt";
  QFile file;
  file.setFileName(filePath);
  if (file.open(QFile::WriteOnly))
    file.write(m_copyRightMessage.toLatin1());
  file.close();

  QSettings setting(m_projectPath + ".projectData/settings.ini",
                    QSettings::IniFormat, this);
  setting.setValue("AddTagForNotDocumentedSymbols", m_withUndocumentedSymbols);
  setting.setValue("WithLibraryDocuments", m_includeLibraryDocuments);
  return true;
}

void cs8Application::exportToCClass(const QString &path) const {
  exportToHFile(path);
  exportToCppFile(path);
}

void cs8Application::exportIOList(const QString &fileName) const {

  qDebug() << "export  IOs to : " << fileName;
  QFile file;
  file.setFileName(fileName);
  if (!file.open(QFile::WriteOnly)) {
    qDebug() << "Couldn't write IO list to file " << fileName
             << file.errorString();
    return;
  }
  QTextStream stream(&file);
  stream << exportToIOList();
  file.close();
}
QString cs8Application::copyRightMessage() const { return m_copyRightMessage; }
bool cs8Application::withUndocumentedSymbols() const {
  return m_withUndocumentedSymbols;
}

void cs8Application::setWithUndocumentedSymbols(bool withUndocumentedSymbols) {
  if (m_withUndocumentedSymbols != withUndocumentedSymbols)
    emit modified(true);
  m_withUndocumentedSymbols = withUndocumentedSymbols;
  foreach (cs8Program *prog, m_programModel->programList()) {
    prog->setWithUndocumentedSymbols(m_withUndocumentedSymbols);
  }
}
