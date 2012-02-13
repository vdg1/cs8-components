#ifndef CS8PROGRAM_H
#define CS8PROGRAM_H
//
#include <QObject>
#include <QDomDocument>
#include "cs8variablemodel.h"
#include "cs8parametermodel.h"

//
class cs8Program: public QObject {
    Q_OBJECT

public:
    QString toDocumentedCode();
    bool save(const QString & projectPath, bool withCode = true);
    cs8ParameterModel* parameterModel() {
        return m_parameterModel;
    }
    cs8VariableModel* localVariableModel() const {
        return m_localVariableModel;
    }
    void setPublic(bool value) {
        m_programSection.setAttribute("access",value?"public":"private");
    }
    bool isPublic() const {
        return m_programSection.attribute("access","private")=="public";
    }

    QString name() const {
        return m_programSection.attribute("name");
    }
    QString fileName() const {
        return name() + ".pgx";
    }
    QString setName(const QString & name);
    QString definition() const;
    QString documentation(bool withPrefix=true) const;
    QString extractDocumentation(const QString & code_);
    QString extractCode(const QString & code_) const;
    QStringList variableTokens();
    void setCode(const QString & code);
    void copyFromParameterModel(cs8ParameterModel *sourceModel);

private:
    cs8VariableModel* m_localVariableModel;
    cs8ParameterModel* m_parameterModel;
    void printChildNodes(const QDomElement & element);
    void createXMLSkeleton();

public:
    cs8Program(QObject * parent);
    //cs8Program(const QString & filePath);
    bool open(const QString & filePath);
    void setVal3Code(const QString& theValue);
    QString val3Code(bool withDocumentation=true);
    QString toCSyntax();
    void parseDocumentation(const QString & code);

    void setDescription(const QString& theValue);

    QString description() const;
    void setCellPath(const QString & path);
    QString cellFilePath() const;

protected:
    bool parseProgramDoc(const QDomDocument & doc);
    QDomDocument m_XMLDocument;
    QDomElement m_programsSection;
    QDomElement m_paramSection;
    QDomElement m_programSection;
    QDomElement m_localSection;
    QDomElement m_codeSection;
    QDomElement m_descriptionSection;
    QString m_description;
    QString m_cellPath;
    QString m_filePath;

signals:
    void globalVariableDocumentationFound(const QString & name,
                                          const QString & document);
    void moduleDocumentationFound(const QString & document);


};
#endif

