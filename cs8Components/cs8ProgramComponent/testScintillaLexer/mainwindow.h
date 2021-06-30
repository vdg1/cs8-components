#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE

class cs8ApplicationModel;
class cs8Application;
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected slots:
  void cursorPositionChanged(int currLine, int currPos);
  void textChanged();

private:
  Ui::MainWindow *ui;
  bool m_updateIndentationNeeded;
  QStringList m_blockStart, m_blockEnd;
  QRegularExpression m_rxFirstWord;
  cs8ApplicationModel *m_pool;
  cs8Application *m_app;
};
#endif // MAINWINDOW_H
