#ifndef CS8APPLICATION_H
#define CS8APPLICATION_H
//
#include <QObject>
#include <QDomDocument>
#include <QList>
#include <QMultiHash>

#include "cs8program.h"
#include "cs8programmodel.h"
#include "cs8libraryaliasmodel.h"
#include "cs8globalvariablemodel.h"
#include "cs8typemodel.h"

//


class cs8Application : public QObject
{
Q_OBJECT

public:
QString projectPath(bool cs8Format=false);
bool save();
bool loadDataFile(const QString & fileName);
bool saveDataFile(const QString & fileName);
bool open(const QString & pfxFilePath);
bool openFromPathName(const QString & filePath);
cs8Application(QObject* parent = 0);
bool parseProject(const QDomDocument & doc);

cs8ProgramModel* programModel() const;
cs8LibraryAliasModel* libraryModel() const;

QString exportToCImplementation();
QString exportToCDefinition();
bool exportInterfacePrototype(const QString & path);
bool integrateInterface(cs8Application *sourceApplication);
QString name() const
{
    return m_projectName;
}
void setName(const QString & name);

bool loadDocumentationFile(const QString &);
QString moduleDocumentationFormatted(QString withSlashes=QString("///"));
QString mainPageDocumentationFromatted(QString withSlashes=QString("///"));
QString performPrecompilerChecks();
void setCellPath(const QString & path);
QString cellPath() const;
QString cellProjectFilePath(bool cs8Format=false) const;
QString cellDataFilePath(bool cs8Format=false) const;
bool writeProjectFile();
QHash<QString, QString> exportDirectives() const
{
    return m_exportDirectives;
}
bool isModified() const
{
    return m_modified;
}
void moveParamsToGlobals(cs8Program *program);
void setCopyrightMessage(const QString &text);
cs8VariableModel *globalVariableModel() const;
cs8TypeModel *typeModel() const;
bool loadProjectData();
bool saveProjectData();
void exportToCClass(const QString path);

QString copyRightMessage() const;

bool withUndocumentedSymbols() const;
void setWithUndocumentedSymbols(bool withUndocumentedSymbols);

QMap<QString, QMap<QString, QString> *> getEnumerations();
void checkPrograms(QStringList &output);
void checkEnumerations(QStringList &output);
void checkGlobalData(QStringList &output);
void checkObsoleteProgramFiles(QStringList &output);
QString applicationDocumentation() const;
void setApplicationDocumentation(const QString &applicationDocumentation);

void initPrecompilerSettings();
QString mainPageDocumentation() const;
void setMainPageDocumentation(const QString &mainPageDocumentation);

QString version() const;


bool includeLibraryDocuments() const;
void setIncludeLibraryDocuments(bool includeLibraryDocuments);

QMap<QString, bool> buildGlobalDataReferenceMap();
QMap<QString, QList<cs8Program *> > buildCallList();
QStringList getCallList(cs8Program *program);
QMap<QString, bool> getReferencedMap() const;

QString getProjectPath() const;

void setProjectPath(const QString &pth);
bool importVPlusFile(const QString & fileName);

protected:
QHash<QString, QString> m_exportDirectives;
QHash<QString, QString> m_pragmaList;
QMap<QString, bool> m_globalDataReferencedMap;
QMap<QString, QList<cs8Program *> > m_callList;
QString m_projectName;
QString m_projectPath;
cs8ProgramModel* m_programModel;
cs8GlobalVariableModel* m_globalVariableModel;
cs8LibraryAliasModel* m_libraryAliasModel;
cs8TypeModel *m_typeModel;
//QDomDocument m_dataDoc;
QString m_moduleDocumentation, m_mainPageDocumentation;
QString m_cellPath;
QString m_copyRightMessage;
QString m_version;
bool m_modified;
bool m_withUndocumentedSymbols;
bool m_includeLibraryDocuments;

QDomDocument m_XMLDocument;
QDomElement m_parameters;
QDomElement m_programSection;
QDomElement m_dataSection;
QDomElement m_aliasSection;
QDomElement m_typesSection;
QDomElement m_projectSection;

void createXMLSkeleton();

protected slots:
void slotGlobalVariableDocumentationFound(const QString & name, const QString & document);
void slotModuleDocumentationFound(const QString & document);
void slotMainPageDocumentationFound(const QString & document);
void slotExportDirectiveFound(const QString & module, const QString & function);
void slotUnknownTagFound(const QString & tagType, const QString & tagName, const QString & tagText);
void setModified(bool modified);

signals:
void modified(bool);

private:
bool reportUnusedPublicGlobalVariables;
bool reportHiddenGlobalVariables;
bool reportParametersPostfix;
bool reportToDos;
void exportToCppFile(const QString & path);
void exportToHFile(const QString & path);
};
#endif

