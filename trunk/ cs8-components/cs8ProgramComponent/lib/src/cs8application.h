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
#include "cs8typemodel.h"

//


class cs8Application: public QObject {
Q_OBJECT

public:
        QString projectPath();
        bool save();
        bool loadDataFile(const QString & fileName);
        bool saveDataFile(const QString & fileName);
        bool open(const QString & filePath);
        bool openFromPathName(const QString & filePath);
        cs8Application(QObject* parent = 0);
        bool parseProject(const QDomDocument & doc);

        cs8ProgramModel* programModel() const;
        cs8LibraryAliasModel* libraryModel() const;
        void exportToCFile(const QString & path);
        QString exportToCSyntax();
        bool exportInterfacePrototype(const QString & path);
        QString name() const {
                return m_projectName;
        }
        void setName(const QString & name);

        bool loadDocumentationFile(const QString & fileName);
        QString documentation();
        QString checkVariables() const;
        void setCellPath(const QString & path);
        QString cellPath() const;
        QString cellProjectFilePath() const;
        QString cellDataFilePath() const;
        bool writeProjectFile();
        QHash<QString, QString> exportDirectives() const {return m_exportDirectives;}

protected:
        QHash<QString, QString> m_exportDirectives;
        QString m_projectName;
        QString m_projectPath;
        cs8ProgramModel* m_programModel;
        cs8VariableModel* m_globalVariableModel;
        cs8LibraryAliasModel* m_libraryAliasModel;
        cs8TypeModel *m_typeModel;
        //QDomDocument m_dataDoc;
        QString m_documentation;
        QString m_cellPath;

        QDomDocument m_XMLDocument;
        QDomElement m_parameters;
        QDomElement m_programSection;
        QDomElement m_dataSection;
        QDomElement m_aliasSection;
        QDomElement m_typesSection;
        QDomElement m_projectSection;

        void createXMLSkeleton();

protected slots:
        void slotGlobalVariableDocumentationFound(const QString & name,
                        const QString & document);
        void slotModuleDocumentationFound(const QString & document);
        void slotExportDirectiveFound(const QString & module, const QString & function);
}	;
#endif

