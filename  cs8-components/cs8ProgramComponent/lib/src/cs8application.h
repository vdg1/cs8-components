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

        bool loadDocumentationFile(const QString &);
        QString documentation();
        QString checkVariables();
        void setCellPath(const QString & path);
        QString cellPath() const;
        QString cellProjectFilePath() const;
        QString cellDataFilePath() const;
        bool writeProjectFile();
        QHash<QString, QString> exportDirectives() const {return m_exportDirectives;}
        bool isModified() const {return m_modified;}
        void moveParamsToGlobals(cs8Program *program);
        void setCopyrightMessage(const QString &text);
        cs8VariableModel *globalVariableModel() const;
        cs8TypeModel *typeModel() const;

protected:
        QHash<QString, QString> m_exportDirectives;
        QHash<QString, QString> m_pragmaList;
        QString m_projectName;
        QString m_projectPath;
        cs8ProgramModel* m_programModel;
        cs8GlobalVariableModel* m_globalVariableModel;
        cs8LibraryAliasModel* m_libraryAliasModel;
        cs8TypeModel *m_typeModel;
        //QDomDocument m_dataDoc;
        QString m_documentation;
        QString m_cellPath;
        bool m_modified;

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
        void slotUnknownTagFound(const QString & tagType, const QString & tagName, const QString & tagText);
        void setModified(bool modified);

signals:
        void modified(bool);

    private:
        bool reportUnusedPublicGlobalVariables;
}	;
#endif

