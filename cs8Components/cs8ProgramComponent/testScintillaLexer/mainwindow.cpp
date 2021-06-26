#include "mainwindow.h"
#include "cs8application.h"
#include "csscilexer.h"
#include "qscilexerval3.h"
#include "ui_mainwindow.h"
#include <QDebug>

QString text = R"(
begin
  //\brief
  //Check if tool changer is working properly
  if false
    //\doc
    //This routine initializes and verifies the state of the tool changer. It is called the first time a robot cycle is started after system start.
    //
    //\param REC_ Obsolete
    //
    //\param quit_ Returns TRUE if an error was detected when intializing the tool changer.
    //
  endIf

  //  switch REC_
  //    default
  //      call debug:error(libPath(),"Invalid recipe number: "+toString("",REC_))
  //      quit_=true
  //    break
  //  endSwitch

  // check gripper changer status consistency
  if bChPowerUpCheck
    call debug:warning(libPath(),"Checking tool changer state after power up")

    // lock tool changer
    IO:doChLock=true
    IO:doChUnlock=false
    watch(IO:diChLocked==true,3)
    switch true
      // changer detects presence of tool, but tool ID is 0
      case IO:diChReady2Lock==true and dioGet(IO:diToolID)==0
        call status:raise(AL_CH_INCONSIST,("EWA detected but ID could not be read"))
        call CYCLE:cancelCycle()
        quit_=true
      break
      // changer detects no tool, but tool ID is not 0
      case IO:diChReady2Lock==false and dioGet(IO:diToolID)!=0
        call status:raise(AL_CH_INCONSIST,("No EWA detected but ID is not 0"))
        call CYCLE:cancelCycle()
        quit_=true
      break
      // changer does not detect locked signal
      case IO:diChLocked==false
        call status:raise(AL_CH_INCONSIST,("Locked signal not detected"))
        call CYCLE:cancelCycle()
        quit_=true
      break
      // changer detects unlocked signal
      case IO:diChUnlocked==true
        call status:raise(AL_CH_INCONSIST,("Unlocked signal detected"))
        call CYCLE:cancelCycle()
        quit_=true
      break
      // all good, put changer in to defined state
      default
        // EWA detected
        if IO:diChReady2Lock==true
          // keep the changer locked, we can continue from here
          bChPowerUpCheck=false
        else
          //unlock the changer
          // lock tool changer
          IO:doChLock=false
          IO:doChUnlock=true
          watch(IO:diChUnlocked==true,3)
          switch true
            // tool ID is not 0
            case dioGet(IO:diToolID)!=0
              call status:raise(AL_CH_INCONSIST,("EWA unlocked but ID is not 0"))
              call CYCLE:cancelCycle()
              quit_=true
            break
            // changer does not detect unlocked signal
            case IO:diChUnlocked==false
              call status:raise(AL_CH_INCONSIST,("Unlocked signal not detected"))
              call CYCLE:cancelCycle()
              quit_=true
            break
            // changer detects locked signal
            case IO:diChLocked==true
              call status:raise(AL_CH_INCONSIST,("Locked signal detected"))
              call CYCLE:cancelCycle()
              quit_=true
            break
            // all good, put changer in to defined state
            default
              bChPowerUpCheck=false
            break
          endSwitch
        endIf
      break
    endSwitch
    nStateGripper=GR_UNKNOWN
  endIf
end
)";
void MainWindow::cursorPositionChanged(int currLine, int currPos) {
  qDebug() << __FUNCTION__ << currLine << currPos;
  static int prevLine = -1;
  if (prevLine != currLine) {
    if ((prevLine >= 0) && (currLine != prevLine) &&
        m_updateIndentationNeeded) {
      int indentation = 0;
      for (int line = 0; line < ui->textEdit->lines(); line++) {
        QString lineText = ui->textEdit->text(line);

        QString firstWord = m_rxFirstWord.match(lineText).captured(1);
        if (m_blockStart.contains(firstWord)) {
          // indentation = ui->textEdit->indentation(line);
          ui->textEdit->setIndentation(line, indentation);
          indentation = indentation + 2;
          ui->textEdit->setIndentation(line + 1, indentation);
          // ui->textEdit->setCursorPosition(line + 1, lineText.length() - 1);
        } else if (m_blockEnd.contains(firstWord)) {
          // indentation = ui->textEdit->indentation(line);
          indentation = qMax(indentation - 2, 0);
          ui->textEdit->setIndentation(line, indentation);
        } else {
          ui->textEdit->setIndentation(line, indentation);
        }
        if (line == currLine) {
          qDebug() << "Adjust cursor position" << line << lineText.length();
          ui->textEdit->setCursorPosition(line, lineText.length());
        }
      }
      m_updateIndentationNeeded = false;
    }
    /*
            int prevIndentation = ui->textEdit->indentation(prevLine);
            qDebug() << "prev indentation" << prevIndentation << " of line "
                     << prevLine;
            QString prevText = ui->textEdit->text(prevLine);
            if (prevText.trimmed().startsWith("for")) {
              QString currLine = ui->textEdit->text(line);
              if (currLine.isEmpty()) {
                QString text = ui->textEdit->text();
              } else {
                ui->textEdit->setIndentation(line, prevIndentation + 1);
              }
            }
          }*/
    prevLine = currLine;
  }
}

void MainWindow::textChanged() { m_updateIndentationNeeded = true; }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_app = new cs8Application(this);
  m_app->openFromPathName(
      R"(D:\data\Staubli\SRS\01 Novo Nordisk, Kalundborg\31983 API\Controller1\usr\usrapp\aInsulinHandlin)");
  ui->textEdit->setText(
      m_app->programModel()->programList()[0]->val3Code(true));
  ui->textEdit->setIndentationGuides(true);
  ui->textEdit->setMarginWidth(0, "0000");
  ui->textEdit->setMarginLineNumbers(0, true);
  ui->textEdit->setFolding(QsciScintilla::CircledTreeFoldStyle);
  ui->textEdit->setEolMode(QsciScintilla::EolUnix);

  ui->textEdit->setCaretForegroundColor(QColor("#ff0000ff"));
  ui->textEdit->setCaretLineVisible(true);
  ui->textEdit->setCaretLineBackgroundColor(QColor("#1f0000ff"));
  ui->textEdit->setCaretWidth(6);

  ui->textEdit->setIndentationsUseTabs(false);
  ui->textEdit->setTabWidth(2);
  ui->textEdit->setIndentationGuides(true);
  ui->textEdit->setIndentationWidth(2);
  ui->textEdit->setTabIndents(false);
  // ui->textEdit->setAutoIndent(true);

  ui->textEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);
  ui->textEdit->setAutoCompletionThreshold(2);

  csSciLexer *lexer = new csSciLexer(this);
  lexer->setApp(m_app);
  ui->textEdit->setLexer(lexer);
  connect(ui->textEdit, &QsciScintilla::cursorPositionChanged, this,
          &MainWindow::cursorPositionChanged);
  connect(ui->textEdit, &QsciScintilla::textChanged, this,
          &MainWindow::textChanged);

  m_blockStart =
      QString("begin if switch case default for do while").split(" ");
  m_blockEnd =
      QString("end endWhile endFor endIf until break endSwitch").split(" ");
  m_rxFirstWord.setPattern(R"RX(^\s*([a-zA-Z0-9]+))RX");
}

MainWindow::~MainWindow() { delete ui; }
