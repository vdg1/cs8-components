#ifndef CSSCILEXER_H
#define CSSCILEXER_H

#include "Qsci/qsciapis.h"
#include "Qsci/qscilexercustom.h"
#include <QObject>
#include <QRegularExpression>

class cs8Application;
class csSciLexer : public QsciLexerCustom {
  Q_OBJECT
public:
  enum styleType {
    Default = 0,
    Comment = 1,
    Keyword = 2,
    Operator = 3,
    Alias = 4,
    String = 5,
    Number = 6,
    Brackets = 7,
    GlobalData = 8,
    LocalData = 9,
    ParameterData = 10,
    AliasOperator = 11,
    SubProgram = 12,
    Unknown
  };
  Q_ENUM(styleType);

  csSciLexer(QObject *parent = 0);
  void styleText(int start, int end) override;

  const char *language() const override;
  QString description(int style) const override;

  cs8Application *app() const;
  void setApp(cs8Application *newApp);

private:
  QRegularExpression tokenRx;
  QStringList keywords;
  QStringList operators;
  QStringList brackets;

protected:
  const char *blockStartKeyword_(int *style) const;
  const char *blockStart_(int *style) const;
  const char *blockEnd_(int *style) const;
  void defineStyles();
  cs8Application *m_app;
  QsciAPIs *m_api;
};

#endif // CSSCILEXER_H
