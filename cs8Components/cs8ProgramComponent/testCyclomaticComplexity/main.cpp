#include "cs8program.h"
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QStringList c;
  c << "begin";
  c << "A = 10";
  c << "if B > C ";
  c << "A = B";
  c << "else";
  c << "A = C";
  c << "endIf";
  c << "putln(A)";
  c << "putln(B)";
  c << "putln(C)";
  c << "end";

  int r = cs8Program::cyclomaticComplexity(c.join("\n"));
  qDebug() << "expected 2 calculated : " << r;
  Q_ASSERT(2 == r);

  c.clear();
  c << "begin";
  c << "if A == 10";
  c << "if B > C ";
  c << "A = B";
  c << "else";
  c << "A = C";
  c << "endIf";
  c << "endIf";
  c << "putln(A)";
  c << "putln(B)";
  c << "putln(C)";
  c << "end";

  r = cs8Program::cyclomaticComplexity(c.join("\n"));
  qDebug() << "expected 3 calculated : " << r;
  Q_ASSERT(3 == r);

  c.clear();

  c << "end";
  c << "i = 0";
  c << "n=4";

  c << "while (i<n-1) ";
  c << "j = i + 1";

  c << "while (j<n) ";

  c << "if A[i]<A[j] ";
  c << "swap(A[i], A[j])";
  c << "else";
  c << "A = C";
  c << "endIf";

  c << "endWhile";
  c << "i=i+1";

  c << "endWhile";

  c << "end";

  r = cs8Program::cyclomaticComplexity(c.join("\n"));
  qDebug() << "expected 4 calculated : " << r;
  Q_ASSERT(4 == r);
}
