#ifndef CS8VARIABLE_H
#define CS8VARIABLE_H
//
#include <QDebug>
#include <QDomElement>
#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QXmlStreamWriter>
//

class cs8Variable : public QObject {

  Q_OBJECT

public:
  struct symbolPosition {
    symbolPosition(int l, int c, const QString &r);
    symbolPosition();
    int line, column;
    QString reference;
  };

  enum DeclarationScope { Local, Parameter, Global };
  Q_ENUM(DeclarationScope)

  cs8Variable(QDomElement &element, const QString &descripton = QString(),
              QObject *parent = 0);
  cs8Variable(cs8Variable *var, QObject *parent);
  cs8Variable(QObject *parent);

  void setUse(QString value);

  QString use() const;

  void setDescription(QString value);
  QString description() const;

  uint size(int dimension = 0);

  QString dimension() const;
  int dimensionCount() const;
  void setDimension(const QString &dim);
  void setType(QString value);
  QString type() const;
  QChar prefix() const;
  bool isConst() const;
  void setXsiType(const QString &type);
  QString xsiType() const;

  void setName(QString value);
  QString name() const;

  QString toString(bool withTypeDefinition = true);
  QString documentation(bool withPrefix, bool forCOutput);
  bool isPublic() const;
  void setPublic(bool m_public);
  QString publicStr() const;

  QStringList father();

  void setValues(const QDomNodeList &values);
  QDomNodeList values() const;
  QString valuesToString() const;
  QString definition();

  void setScope(DeclarationScope scope);
  DeclarationScope scope() const;

  QString allSizes();
  void setAllSizes(const QString &sizes);

  QDomElement element() const;

  QVariant varValue(QString index = "0");
  void setValue(const QString &index, const QMap<QString, QString> &valueMap);
  bool isBuildInType() const;
  static QStringList buildInTypes();
  bool hasConstPrefix(QString *prefix = nullptr) const;
  static void extractArrayIndex(const QString &value, QString &name,
                                QString &index);

  void writeXMLStream(QXmlStreamWriter &stream);
  void clearSymbolReferences();
  void addSymbolReference(int lineNumber, int column,
                          const QString &programName);

  const QList<symbolPosition> &symbolReferences() const;

  const QString &linterDirective() const;
  void setLinterDirective(const QString &newLinterDirective);

protected:
  QStringList m_buildInTypes;
  QDomElement m_element;
  QString m_description;
  QString m_name;
  QDomDocumentFragment m_docFragment;
  QDomDocument m_doc;
  QString m_linterDirective;
  static QStringList setBuildInVariableTypes();

  void writeValueElements(QXmlStreamWriter &stream);
  void writeNodes(QXmlStreamWriter &stream, QDomNodeList nodes);
  QList<symbolPosition> m_lineOccurences;

signals:
  void modified();
};
QDebug operator<<(QDebug dbg, const cs8Variable::symbolPosition &type);

#endif
