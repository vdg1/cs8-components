#ifndef CS8PROGRAM_H
#define CS8PROGRAM_H
//
#include "cs8globalvariablemodel.h"
#include "cs8localvariablemodel.h"
#include "cs8parametermodel.h"
#include "cs8referencesandlinter.h"
#include "cs8variablemodel.h"

#include <QDebug>
#include <QDomDocument>
#include <QObject>
#include <QStringList>

//
class cs8Application;
class cs8Program : public QObject, public cs8ReferencesAndLinter {
  Q_OBJECT

public:
  cs8Program(QObject *parent);
  cs8Program();

  QString toDocumentedCode();
  bool save(const QString &projectPath, bool withCode = true);
  cs8ParameterModel *parameterModel() const;
  cs8VariableModel *localVariableModel() const;
  QList<cs8Variable *> referencedGlobalVariables() const;

  void setPublic(bool value);

  bool isPublic() const;

  QString name() const;

  QString fileName() const;
  bool setName(const QString &name, cs8Application *application = nullptr);
  QString definition() const;
  QString documentation(bool withPrefix = true, bool forCOutput = false) const;

  QString extractCode(const QString &code_) const;

  void setCode(const QString &code, bool parseDoc_ = false);
  void copyFromParameterModel(cs8ParameterModel *sourceModel);
  void addTag(const QString &tagType, const QString &tagName,
              const QString &tagText);
  void clearDocumentationTags();
  void setWithUndocumentedSymbols(bool withUndocumentedSymbols);
  QStringList referencedVariables() const;
  QMap<int, QString> todos() const;
  QStringList getCalls();
  int cyclomaticComplexity() const;
  static int cyclomaticComplexity(const QString &code);

private:
  cs8VariableModel *m_localVariableModel;
  cs8ParameterModel *m_parameterModel;
  cs8Application *m_application;
  QMultiMap<QString, QString> m_tags;
  void printChildNodes(const QDomElement &element);

  QStringList extractDocumentation(const QString &code_,
                                   int &headerLinesCount) const;
  QString m_briefDescription;
  bool m_globalDocContainer;
  QStringList variableTokens(bool onlyModifiedVars);
  QStringList m_variableTokens;

public:
    // cs8Program(const QString & filePath);
    bool open(const QString &projectPath, const QString &filePath);
    bool deleteSourceFile();
    QString val3Code(bool withDocumentation = true) const;
    QString toCSyntax();
    void parseDocumentation(const QString &code);

    void setBriefDescription(const QString &theValue);
    void setDetailedDocumentation(const QString &doc);
    QString detailedDocumentation() const;

    void setCopyrightMessage(const QString &text);
    QString copyrightMessage() const;

    QString briefDescription(bool trimmed) const;
    void setCellPath(const QString &path);
    QString cellFilePath() const;

    bool globalDocContainer() const;
    void setGlobalDocContainer(bool globalDocContainer);

    void setApplicationDocumentation(const QString &applicationDocumentation);
    void setBriefModuleDocumentation(const QString &briefDocumentation);
    void setMainPageDocumentation(const QString &applicationDocumentation);

    QString formattedDescriptionHeader() const;

    int getLineNumberCodeSection() const;
    void undoTranslationTags();

    bool getHasByteOrderMark() const;
    void setHasByteOrderMark(bool hasByteOrderMark);

    void parseProgramSection(const QDomElement &programSection, const QString &code);

    void writeXMLStream(QXmlStreamWriter &stream, bool withCode);

    QString getFilePath() const;
    void setFilePath(const QString &projectPath, const QString &filePath);

    QString getAdditionalHintMessage() const;
    void setAdditionalHintMessage(const QString &additionalHintMessage);

    void updateCodeModel();

protected:
  bool parseProgramDoc(const QDomDocument &doc,
                       const QString &code = QString());
  void tidyUpCode(QString &code);
  void setLinterDirective(const QString &directive, const QString &symbolName);

  QString m_detailedDocumentation;
  QString m_cellPath;
  QString m_filePath;
  QString m_projectPath;
  QString m_copyRightMessage;
  QString m_applicationDocumentation, m_mainPageDocumentation,
      m_briefModuleDocumentation, m_additionalHintMessage;
  QString m_name;
  QString m_programCode;
  bool m_public;
  bool m_withIfBlock;
  bool m_hasByteOrderMark;
  int m_lineNumberCodeSection;
  int m_headerLines;

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
