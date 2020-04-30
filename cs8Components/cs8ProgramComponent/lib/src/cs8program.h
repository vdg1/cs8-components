#ifndef CS8PROGRAM_H
#define CS8PROGRAM_H
//
#include "cs8globalvariablemodel.h"
#include "cs8localvariablemodel.h"
#include "cs8parametermodel.h"
#include "cs8variablemodel.h"
#include <QDebug>
#include <QDomDocument>
#include <QObject>
#include <QStringList>

//
class cs8Program : public QObject {
  Q_OBJECT

public:
  QString toDocumentedCode();
  bool save(const QString &projectPath, bool withCode = true,
            bool val3S6Format = false);
  cs8ParameterModel *parameterModel() const;
  cs8VariableModel *localVariableModel() const;
  cs8VariableModel *referencedGlobalVariables() const;

  void setPublic(bool value);

  bool isPublic() const;

  QString name() const;

  QString fileName() const;
  void setName(const QString &name);
  QString definition() const;
  QString documentation(bool withPrefix = true) const;

  QString extractCode(const QString &code_) const;

  void setCode(const QString &code, bool parseDoc_ = false,
               bool val3S6Format = false);
  void copyFromParameterModel(cs8ParameterModel *sourceModel);
  void addTag(const QString &tagType, const QString &tagName,
              const QString &tagText);
  void clearDocumentationTags();
  void setWithUndocumentedSymbols(bool withUndocumentedSymbols);
  QStringList referencedVariables() const;
  QMap<int, QString> todos();
  QStringList getCalls();

private:
  cs8VariableModel *m_localVariableModel;
  cs8ParameterModel *m_parameterModel;
  cs8VariableModel *m_referencedGlobalVarModel;
  QMultiMap<QString, QString> m_tags;
  void printChildNodes(const QDomElement &element);
  void createXMLSkeleton(bool val3S6Format = false);
  QStringList extractDocumentation(const QString &code_,
                                   int &headerLinesCount) const;
  QString m_briefDescription;
  bool m_globalDocContainer;
  QStringList variableTokens(bool onlyModifiedVars);
  QStringList m_variableTokens;

public:
  cs8Program(QObject *parent);
  // cs8Program(const QString & filePath);
  bool open(const QString &filePath);
  QString val3Code(bool withDocumentation = true);
  QString toCSyntax();
  void parseDocumentation(const QString &code);

  void setDescription(const QString &theValue);
  void setDetailedDocumentation(const QString &doc);
  QString detailedDocumentation() const;

  void setCopyrightMessage(const QString &text);
  QString copyrightMessage() const;

  QString briefDescription() const;
  void setCellPath(const QString &path);
  QString cellFilePath() const;

  void setDescriptionSection(bool val3S6Format = false);
  bool globalDocContainer() const;
  void setGlobalDocContainer(bool globalDocContainer);

  void setApplicationDocumentation(const QString &applicationDocumentation);
  void setBriefModuleDocumentation(const QString &briefDocumentation);
  void setMainPageDocumentation(const QString &applicationDocumentation);

  QDomElement programsSection() const;
  void setProgramsSection(const QDomElement &programsSection);

protected:
  bool parseProgramDoc(const QDomDocument &doc,
                       const QString &code = QString());
  void tidyUpCode(QString &code);
  QDomDocument m_XMLDocument;
  QDomElement m_programsSection;
  QDomElement m_paramSection;
  QDomElement m_programSection;
  QDomElement m_localSection;
  // QDomElement m_codeSection;
  QDomElement m_sourceSection;
  QDomElement m_descriptionSection;
  QString m_detailedDocumentation;
  QString m_cellPath;
  QString m_filePath;
  QString m_copyRightMessage;
  QString m_applicationDocumentation, m_mainPageDocumentation,
      m_briefModuleDocumentation;
  QString m_name;
  QString m_programCode;
  bool m_public;
  bool m_withIfBlock;

signals:
  void globalVariableDocumentationFound(const QString &name,
                                        const QString &document);
  void moduleDocumentationFound(const QString &document);
  void moduleBriefDocumentationFound(const QString &document);
  void mainPageDocumentationFound(const QString &document);
  void exportDirectiveFound(const QString &module, const QString &routine);
  void unknownTagFound(const QString &tagType, const QString &tagName,
                       const QString &tagText);
  void modified();
};
#endif
