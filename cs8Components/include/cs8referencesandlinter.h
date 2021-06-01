#ifndef CS8REFERENCESANDLINTER_H
#define CS8REFERENCESANDLINTER_H

#include <QList>
#include <QString>

class cs8Application;

class cs8ReferencesAndLinter {
public:
  struct symbolPosition {
    symbolPosition(int l, int c, const QString &r);
    symbolPosition();
    int line, column;
    QString reference;
  };

  cs8ReferencesAndLinter();
  void clearSymbolReferences(const QString &name = QString());
  void addSymbolReference(int lineNumber, int column,
                          const QString &programName);

  const QList<symbolPosition> &symbolReferences() const;

  const QString &linterDirective() const;
  void setLinterDirective(const QString &newLinterDirective);

protected:
  QList<symbolPosition> m_references;
  QString m_linterDirective;
  void updateReference(cs8Application *application, const QString &from,
                       const QString &to);
};

QDebug operator<<(QDebug dbg,
                  const cs8ReferencesAndLinter::symbolPosition &type);

#endif // CS8REFERENCESANDLINTER_H
