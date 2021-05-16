//
// C++ Interface: cs8programmodel
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CS8PROGRAMMODEL_H
#define CS8PROGRAMMODEL_H

#include "cs8variable.h"

#include <QAbstractListModel>
#include <QList>

class cs8Program;
class cs8VariableModel;

/**
 @author Volker Drewer-Gutland <volker.drewer@gmx.de>
 */
class cs8ProgramModel : public QAbstractListModel {
  Q_OBJECT
public:
  QList<cs8Program *> programList() { return m_programList; }
  cs8VariableModel *localVariableModel(const QModelIndex &index);
  cs8VariableModel *parameterModel(const QModelIndex &index);
  QList<cs8Variable *> referencedGlobalVriableModel(const QModelIndex &index);

  void clear();
  bool addProgramFile(const QString &filePath);
  cs8ProgramModel(QObject *parent = 0);

  ~cs8ProgramModel();
  virtual int rowCount(const QModelIndex &) const;
  virtual int colCount(const QModelIndex &) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role = Qt::EditRole);
  QList<cs8Program *> publicPrograms();
  QList<cs8Program *> privatePrograms();
  cs8Program *getProgramByName(const QString &name);
  void setCellPath(const QString &path);
  void append(cs8Program *program);

  // void createProgram(cs8Program *program);
  cs8Program *createProgram(const QString &programName = QString());

  bool getHasByteOrderMark() const;
  void setHasByteOrderMark(bool hasByteOrderMark);

protected:
  QList<cs8Program *> m_programList;
  QString m_cellPath;
  bool m_hasByteOrderMark;

protected slots:
  void slotGlobalVariableDocumentationFound(const QString &name,
                                            const QString &document);
  void slotModuleDocumentationFound(const QString &document);
  void slotBriefModuleDocumentationFound(const QString &document);
  void slotMainPageDocumentationFound(const QString &document);
  void slotExportDirectiveFound(const QString &module, const QString &function);
  void slotUnknownTagFound(const QString &tagType, const QString &tagName,
                           const QString &tagText);
  void slotModified();

signals:
  void globalVariableDocumentationFound(const QString &name,
                                        const QString &document);
  void moduleDocumentationFound(const QString &document);
  void briefModuleDocumentationFound(const QString &document);
  void mainPageDocumentationFound(const QString &document);
  void exportDirectiveFound(const QString &module, const QString &function);
  void unknownTagFound(const QString &tagType, const QString &tagName,
                       const QString &tagText);
  void modified(bool);
};

#endif
