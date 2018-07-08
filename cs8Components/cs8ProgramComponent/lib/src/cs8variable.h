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
//

class cs8Variable : public QObject {

  Q_OBJECT

public:
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

  void setName(QString value);
  QString name() const;
  cs8Variable(QDomElement &element, const QString &descripton = QString());
  cs8Variable(cs8Variable *var);
  cs8Variable();

  QString toString(bool withTypeDefinition = true);
  QString documentation(bool withPrefix, bool forCOutput);
  bool isPublic() const;

  QStringList father();

  void setPublic(bool m_public);

  QDomNodeList values();
  QString definition();

  void setGlobal(bool global);
  bool isGlobal() const;

  QString allSizes();
  void setAllSizes(const QString &sizes);

  QDomElement element(QDomDocument *doc = nullptr, bool val3S6Format = false) const;

  QVariant varValue(QString index = "0");
  void setValue(const QString &index, const QMap<QString, QString> &valueMap);
  bool isBuildInType() const;
  static QStringList buildInTypes(bool val3S6Format = false);
  bool hasConstPrefix(QString *prefix = nullptr) const;
  static void extractArrayIndex(const QString &value, QString &name, QString &index);

protected:
  QStringList m_buildInTypes;
  QDomElement m_element;
  QString m_description;
  QDomDocumentFragment m_docFragment;
  QDomDocument m_doc;
  bool m_global;
  static QStringList setBuildInVariableTypes(bool val3S6Format = false);

signals:
  void modified();
};
#endif