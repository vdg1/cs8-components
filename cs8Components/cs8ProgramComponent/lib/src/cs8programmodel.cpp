//
// C++ Implementation: cs8programmodel
//
// Description:
//
//
// Author: Volker Drewer-Gutland <volker.drewer@gmx.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cs8programmodel.h"
#include "cs8program.h"
#include <QDebug>
#include <QFileInfoList>
#include <QFont>
#include <QTextCodec>
cs8ProgramModel::cs8ProgramModel(QObject *parent) : QAbstractListModel(parent) {
  m_cellPath = "";
}

cs8ProgramModel::~cs8ProgramModel() {
  foreach (cs8Program *program, m_programList)
    program->deleteLater();
}

/*!
 \fn cs8ProgramModel::rowCount(const QModelIndex & index)
 */
int cs8ProgramModel::rowCount(const QModelIndex & /*index*/) const {
  return m_programList.count();
}

/*!
 \fn cs8ProgramModel::colCount(const QModelIndex & index)
 */
int cs8ProgramModel::colCount(const QModelIndex & /*index*/) const {
  return 2; // m_programList.count();
}

cs8Program *cs8ProgramModel::getProgramByName(const QString &name) {
  for (int i = 0; i < m_programList.count(); i++) {
    if (m_programList.at(i)->name().compare(name) == 0)
      return m_programList.at(i);
  }
  return 0;
}

void cs8ProgramModel::setCellPath(const QString &path) { m_cellPath = path; }

void cs8ProgramModel::append(cs8Program *program) {
  m_programList.append(program);
#if QT_VERSION >= 0x050000
  beginResetModel();
  endResetModel();
#else
  reset();
#endif
}

QList<cs8Program *> cs8ProgramModel::publicPrograms() {
  QList<cs8Program *> out;
  foreach (cs8Program *program, m_programList) {
    // qDebug() << program->name();
    if (program->isPublic())
      out << program;
  }
  return out;
}

QList<cs8Program *> cs8ProgramModel::privatePrograms() {
  QList<cs8Program *> out;
  foreach (cs8Program *program, m_programList) {
    if (!program->isPublic())
      out << program;
  }
  return out;
}

/*!
 \fn cs8ProgramModel::data(const QModelIndex & index, int role)
 */
QVariant cs8ProgramModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::FontRole) {
    if (m_programList.at(index.row())->isPublic()) {
      QFont font;
      font.setBold(true);
      return font;
    }
  }

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
    case 0:
      return m_programList.at(index.row())->name();
      break;

    case 1:
      return m_programList.at(index.row())->definition();
      break;
    }
  }

  if (role == Qt::UserRole)
    return m_programList.at(index.row())->val3Code(true);

  /// TODO why compiler error here?
  /*
      if (role == Qt::UserRole + 1)
      return m_programList.at(index.row())->parameterModel();

  if (role == Qt::UserRole + 2)
      return m_programList.at(index.row())->localVariableModel();
              */
  if (role == Qt::UserRole + 10)
    return m_programList.at(index.row())->detailedDocumentation();

  if (role == Qt::UserRole + 11)
    return m_programList.at(index.row())->briefDescription(true);

  return QVariant();
}

cs8Program *cs8ProgramModel::createProgram(const QString &programName) {
  cs8Program *program = new cs8Program(this);

  m_programList.append(program);
  connect(program, &cs8Program::globalVariableDocumentationFound, this,
          &cs8ProgramModel::slotGlobalVariableDocumentationFound);
  connect(program, &cs8Program::moduleDocumentationFound, this,
          &cs8ProgramModel::slotModuleDocumentationFound);
  connect(program, &cs8Program::moduleBriefDocumentationFound, this,
          &cs8ProgramModel::slotBriefModuleDocumentationFound);
  connect(program, &cs8Program::mainPageDocumentationFound, this,
          &cs8ProgramModel::slotMainPageDocumentationFound);
  connect(program, &cs8Program::exportDirectiveFound, this,
          &cs8ProgramModel::slotExportDirectiveFound);
  connect(program, &cs8Program::unknownTagFound, this,
          &cs8ProgramModel::slotUnknownTagFound);
  connect(program, &cs8Program::modified, this, &cs8ProgramModel::slotModified);

  program->setCellPath(m_cellPath);
  if (!programName.isEmpty())
    program->setName(programName, nullptr);
  // Q_ASSERT(!programName.isEmpty());
  return program;
}

bool cs8ProgramModel::getHasByteOrderMark() const { return m_hasByteOrderMark; }

void cs8ProgramModel::setHasByteOrderMark(bool hasByteOrderMark) {
  m_hasByteOrderMark = hasByteOrderMark;
}

void cs8ProgramModel::updateCodeModel() {
  for (auto prog : qAsConst(m_programList)) {
    prog->clearSymbolReferences();
  }
  for (auto prog : qAsConst(m_programList)) {
    prog->updateCodeModel();
  }
}

QStringList cs8ProgramModel::programFolders() const {
  QStringList list;
  const auto l = programList();
  for (const cs8Program *program : l) {
    QString str = program->fileName();
    if (str.indexOf("/") != -1)
      str = str.mid(0, str.indexOf("/"));
    else
      str = "";
    if (!str.isEmpty())
      list << str + "/";
  }
  list.removeDuplicates();
  list << ".";
  return list;
}

bool cs8ProgramModel::addProgramFile(const QString &projectPath,
                                     const QString &filePath) {
  QFile file(projectPath + filePath);
  if (!file.open(QIODevice::ReadOnly))
    return false;
  m_hasByteOrderMark =
      QTextCodec::codecForUtfText(file.peek(4), nullptr) != nullptr;
  // if (m_hasByteOrderMark)
  //    qDebug() << "File has BOM";

  QDomDocument doc;
  if (!doc.setContent(&file)) {
    file.close();
    return false;
  }
  file.close();

  QDomElement programsSection = doc.documentElement();
  Q_ASSERT(!programsSection.isNull());
  // printChildNodes(m_programsSection);

  for (int i = 0; i < programsSection.childNodes().count(); i++) {
    QDomElement programSection = programsSection.childNodes().at(i).toElement();
    cs8Program *program = createProgram();
    program->setHasByteOrderMark(m_hasByteOrderMark);
    program->parseProgramSection(programSection, "");
    program->setFilePath(projectPath, filePath);
  }
  // QDomElement programSection = programsSection.firstChild().toElement();
  // parseProgramSection(programSection, code);

  // QFileInfo info(filePath);
  // cs8Program *program = createProgram(info.baseName());
  // if (!program->open(filePath))
//    qDebug() << "Opening program " << filePath << " failed";
#if QT_VERSION >= 0x050000
  beginResetModel();
  endResetModel();
#else
  reset();
#endif
  return true;
}

void cs8ProgramModel::slotGlobalVariableDocumentationFound(
    const QString &name, const QString &document) {
  emit globalVariableDocumentationFound(name, document);
}

void cs8ProgramModel::slotModuleDocumentationFound(const QString &document) {
  emit moduleDocumentationFound(document);
}

void cs8ProgramModel::slotBriefModuleDocumentationFound(
    const QString &document) {
  emit briefModuleDocumentationFound(document);
}

void cs8ProgramModel::slotMainPageDocumentationFound(const QString &document) {
  emit mainPageDocumentationFound(document);
}

void cs8ProgramModel::slotExportDirectiveFound(const QString &module,
                                               const QString &function) {
  emit exportDirectiveFound(module, function);
}

void cs8ProgramModel::slotUnknownTagFound(const QString &tagType,
                                          const QString &tagName,
                                          const QString &tagText) {
  emit unknownTagFound(tagType, tagName, tagText);
}

void cs8ProgramModel::slotModified() { emit modified(true); }

void cs8ProgramModel::clear() {
  m_programList.clear();
#if QT_VERSION >= 0x050000
  beginResetModel();
  endResetModel();
#else
  reset();
#endif
}

QList<cs8Program *> cs8ProgramModel::programList() const {
  return m_programList;
}

cs8VariableModel *
cs8ProgramModel::localVariableModel(const QModelIndex &index) const {
  if (index.isValid())
    return m_programList.at(index.row())->localVariableModel();
  else
    return 0;
}

cs8VariableModel *cs8ProgramModel::parameterModel(const QModelIndex &index) {
  if (index.isValid())
    return m_programList.at(index.row())->parameterModel();
  else
    return 0;
}

QList<cs8Variable *>
cs8ProgramModel::referencedGlobalVariables(const QModelIndex &index) {
  if (index.isValid())
    return m_programList.at(index.row())->referencedGlobalVariables();
  else
    return QList<cs8Variable *>();
}

bool cs8ProgramModel::setData(const QModelIndex &index, const QVariant &value,
                              int role) {
  if (!index.isValid())
    return false;

  if (role == Qt::UserRole + 10)
    m_programList.at(index.row())->setDetailedDocumentation(value.toString());

  if (role == Qt::UserRole + 11)
    m_programList.at(index.row())->setBriefDescription(value.toString());

  return true;
}
