#ifndef CS8APPLICATION_H
#define CS8APPLICATION_H
//
#include <QDomDocument>
#include <QList>
#include <QMultiHash>
#include <QObject>

#include "cs8globalvariablemodel.h"
#include "cs8libraryaliasmodel.h"
#include "cs8program.h"
#include "cs8programmodel.h"
#include "cs8typemodel.h"

//

class cs8Application : public QObject {
  Q_OBJECT

public:
  QString projectPath(bool cs8Format = false);
  bool save(bool compactMode);
  bool save();
  bool save(const QString &path, const QString &name, bool compactMode);
  bool loadDataFile(const QString &fileName);
  bool saveDataFile(const QString &fileName);
  bool open(const QString &pfxFilePath);
  bool openFromPathName(const QString &filePath);
  cs8Application(QObject *parent = nullptr);
  bool parseProject(const QDomDocument &doc);

  cs8ProgramModel *programModel() const;
  cs8LibraryAliasModel *libraryModel() const;

  QString exportToCImplementation() const;
  QString exportToCDefinition() const;
  QString exportToIOList() const;
  bool exportInterfacePrototype(const QString &path);
  bool integrateInterface(cs8Application *sourceApplication);
  QString name() const;
  void setName(const QString &name);

  bool loadDocumentationFile(const QString &);
  QString moduleDocumentationFormatted(
      const QString &withSlashes = QString("/// ")) const;
  QString mainPageDocumentationFormatted(
      const QString &withSlashes = QString("/// ")) const;
  QString briefModuleDocumentationFormatted(
      const QString &withSlashes = QString("/// ")) const;

  QString formattedDocument(const QString &doc,
                            const QString &withSlashes = QString("/// ")) const;
  QString performPrecompilerChecks();
  void setCellPath(const QString &path);
  QString cellPath() const;
  QString cellProjectFilePath(bool cs8Format = false) const;
  QString cellDataFilePath(bool cs8Format = false) const;
  bool writeProjectFile();
  QHash<QString, QString> exportDirectives() const;
  bool isModified() const;
  void moveParamsToGlobals(cs8Program *program);
  void setCopyrightMessage(const QString &text);
  cs8VariableModel *globalVariableModel() const;
  cs8TypeModel *typeModel() const;
  bool loadProjectData();
  bool saveProjectData();
  void exportToCClass(const QString &path) const;
  void exportIOList(const QString &fileName) const;

  QString copyRightMessage() const;

  bool withUndocumentedSymbols() const;
  void setWithUndocumentedSymbols(bool withUndocumentedSymbols);

  QMap<QString, QMap<QString, QString> *> getEnumerations() const;
  void checkPrograms(QStringList &output);
  void checkEnumerations(QStringList &output);
  void checkGlobalData(QStringList &output);
  void checkObsoleteProgramFiles(QStringList &output);
  QString moduleDocumentation() const;
  void setModuleDocumentation(const QString &moduleDocumentation);

  void initPrecompilerSettings();
  QString mainPageDocumentation() const;
  void setMainPageDocumentation(const QString &mainPageDocumentation);

  QString version() const;

  bool includeLibraryDocuments() const;
  void setIncludeLibraryDocuments(bool includeLibraryDocuments);

  QMap<QString, bool> buildGlobalDataReferenceMap();
  QMap<QString, QList<cs8Program *>> buildCallList();
  QStringList getCallList(cs8Program *program);
  QMap<QString, bool> getReferencedMap() const;

  QString getProjectPath() const;

  void setProjectPath(const QString &pth);
  bool importVPlusFile(const QString &fileName);

  QString briefModuleDocumentation() const;
  void setBriefModuleDocumentation(const QString &briefModuleDocumentation);

  QString getProjectVersion() const;
  void setProjectVersion(const QString &projectVersion);

  QString getProjectStackSize() const;
  void setProjectStackSize(const QString &projectStackSize);

  QString getProjectMillimeterUnit() const;
  void setProjectMillimeterUnit(const QString &projectMillimeterUnit);

  bool getCompactFileMode() const;
  void setCompactFileMode(bool singleProgramFile);

protected:
  QHash<QString, QString> m_exportDirectives;
  QHash<QString, QString> m_pragmaList;
  QMap<QString, bool> m_globalDataReferencedMap;
  QMap<QString, QList<cs8Program *>> m_callList;
  QString m_projectName;
  QString m_projectPath;
  cs8ProgramModel *m_programModel;
  cs8GlobalVariableModel *m_globalVariableModel;
  cs8LibraryAliasModel *m_libraryAliasModel;
  cs8TypeModel *m_typeModel;
  // QDomDocument m_dataDoc;
  QString m_moduleDocumentation, m_mainPageDocumentation,
      m_briefModuleDocumentation;
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
  QString m_projectVersion;
  QString m_projectStackSize;
  QString m_projectMillimeterUnit;

  void createXMLSkeleton(bool S6Format);

protected slots:
  void slotGlobalVariableDocumentationFound(const QString &name,
                                            const QString &document);
  void slotModuleDocumentationFound(const QString &document);
  void slotBriefModuleDocumentationFound(const QString &document);
  void slotMainPageDocumentationFound(const QString &document);
  void slotExportDirectiveFound(const QString &module, const QString &function);
  void slotUnknownTagFound(const QString &tagType, const QString &tagName,
                           const QString &tagText);
  void setModified(bool modified);

signals:
  void modified(bool);

private:
  bool reportUnusedPublicGlobalVariables;
  bool reportHiddenGlobalVariables;
  bool reportParametersPostfix;
  bool reportToDos;
  bool m_compactFileMode;
  void exportToCppFile(const QString &path) const;
  void exportToHFile(const QString &path) const;

  QString sanitizeSymbolName(const QString &varName);
};
#endif
