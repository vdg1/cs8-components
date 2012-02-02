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
    bool isPublic() {
        return m_programSection.attribute("access","private")=="public";
    }

    QString name() {
        return m_programSection.attribute("name");
    }
    QString fileName() {
        return name() + ".pgx";
    }
    QString definition();
    QString documentation(bool withPrefix=true);
    QString extractDocumentation(const QString & code_);
    QString extractCode(const QString & code_);

private:
    cs8VariableModel* m_localVariableModel;
    cs8ParameterModel* m_parameterModel;
    void printChildNodes(const QDomElement & element);

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

protected:
    bool parseProgramDoc(const QDomDocument & doc);
    QDomElement m_root;
    QDomElement m_paramSection;
    QDomElement m_programSection;
    QDomElement m_localSection;
    QDomElement m_codeSection;
    QString m_description;

signals:
    void globalVariableDocumentationFound(const QString & name,
                                          const QString & document);
    void moduleDocumentationFound(const QString & document);


};
#endif

