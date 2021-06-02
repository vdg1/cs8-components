#include "cs8referencesandlinter.h"
#include "cs8application.h"

#include <QDebug>

cs8ReferencesAndLinter::cs8ReferencesAndLinter() {}
cs8ReferencesAndLinter::symbolPosition::symbolPosition(int l, int c,
                                                       const QString &r)
    : line(l), column(c), reference(r) {}

cs8ReferencesAndLinter::symbolPosition::symbolPosition()
    : line(0), column(0), reference("") {}

const QList<cs8ReferencesAndLinter::symbolPosition> &
cs8ReferencesAndLinter::symbolReferences() const {
  // qDebug() << __FUNCTION__ << this << name() << ":" << m_references;
  return m_references;
}

const QString &cs8ReferencesAndLinter::linterDirective() const {
  return m_linterDirective;
}

void cs8ReferencesAndLinter::setLinterDirective(
    const QString &newLinterDirective) {
  m_linterDirective = newLinterDirective;
}

void cs8ReferencesAndLinter::setLinterMessages(const QString msg) {
  m_linterMessage = msg;
}

void cs8ReferencesAndLinter::clearSymbolReferences(const QString &name) {
  if (name.isEmpty())
    m_references.clear();
  else {
    QMutableListIterator<cs8ReferencesAndLinter::symbolPosition> i(
        m_references);
    while (i.hasNext()) {
      if (i.next().reference == name)
        i.remove();
    }
  }
}

void cs8ReferencesAndLinter::addSymbolReference(int lineNumber, int column,
                                                const QString &programName) {
  m_references.append(symbolPosition(lineNumber, column, programName));
}

void cs8ReferencesAndLinter::updateReference(cs8Application *application,
                                             const QString &from,
                                             const QString &to) {
  QListIterator<symbolPosition> i(m_references);
  i.toBack();
  while (i.hasPrevious()) {
    auto ref = i.previous();
    // qDebug() << ref;
    auto prog = application->programModel()->getProgramByName(ref.reference);
    Q_ASSERT(prog);
    qDebug() << prog->name();
    QStringList c = prog->val3Code(true).split("\n");
    // QString b = c[ref.line];
    c[ref.line].replace(ref.column, from.length(), to);
    // qDebug() << "before:" << b << "after:" << c[ref.line];
    prog->setCode(c.join("\n"), true);
    // qDebug() << prog->val3Code(true);
  }
}

QDebug operator<<(QDebug dbg,
                  const cs8ReferencesAndLinter::symbolPosition &type) {
  dbg.nospace() << "reference(" << type.line << "," << type.column
                << type.reference << ")";
  return dbg.maybeSpace();
}
