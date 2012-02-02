#ifndef CS8APPLICATION_H
#define CS8APPLICATION_H
//
#include <QObject>
#include <QDomDocument>
#include <QList>

#include "cs8program.h"
#include "cs8programmodel.h"
#include "cs8libraryaliasmodel.h"
//class cs8ProgramModel;

//
class cs8Application: public QObject {
Q_OBJECT

public:
        QString projectPath();
        void save();
        bool loadDataFile(const QString & fileName);
        bool open(const QString & filePath);
        bool openFromPathName(const QString & filePath);
        cs8Application(QObject* parent = 0);
        bool parseProject(const QDomDocument & doc);

        cs8ProgramModel* programModel() const;
        void exportToCFile(const QString & path);
        QString exportToCSyntax();
        bool exportInterfacePrototype(const QString & path);
        QString name() {
                return m_projectName;
        }
        bool loadDocumentationFile(const QString & fileName);
        QString documentation();

protected:
        QString m_projectName;
        QString m_projectPath;
        cs8ProgramModel* m_programModel;
        cs8VariableModel* m_variableModel;
        cs8LibraryAliasModel* m_libraryAliasModel;
        //QDomDocument m_dataDoc;
        QString m_documentation;

        QDomElement m_parameters;
        QDomElement m_programSection;
        QDomElement m_dataSection;
        QDomElement m_aliasSection;
        QDomElement m_typesSection;

protected slots:
        void slotGlobalVariableDocumentationFound(const QString & name,
                        const QString & document);
        void slotModuleDocumentationFound(const QString & document);
}	;
#endif

