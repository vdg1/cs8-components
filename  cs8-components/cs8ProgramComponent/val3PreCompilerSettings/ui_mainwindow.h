/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *cpUnusedPublicGlobalVar;
    QCheckBox *cpHiddenGlobalVariables;
    QCheckBox *cpParametersPostfix;
    QCheckBox *cpReportsToDos;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(362, 200);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cpUnusedPublicGlobalVar = new QCheckBox(centralWidget);
        cpUnusedPublicGlobalVar->setObjectName(QString::fromUtf8("cpUnusedPublicGlobalVar"));

        verticalLayout->addWidget(cpUnusedPublicGlobalVar);

        cpHiddenGlobalVariables = new QCheckBox(centralWidget);
        cpHiddenGlobalVariables->setObjectName(QString::fromUtf8("cpHiddenGlobalVariables"));

        verticalLayout->addWidget(cpHiddenGlobalVariables);

        cpParametersPostfix = new QCheckBox(centralWidget);
        cpParametersPostfix->setObjectName(QString::fromUtf8("cpParametersPostfix"));

        verticalLayout->addWidget(cpParametersPostfix);

        cpReportsToDos = new QCheckBox(centralWidget);
        cpReportsToDos->setObjectName(QString::fromUtf8("cpReportsToDos"));

        verticalLayout->addWidget(cpReportsToDos);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(centralWidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        verticalLayout->addWidget(buttonBox);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 362, 21));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "VAL3 Precompiler Settings", 0, QApplication::UnicodeUTF8));
        cpUnusedPublicGlobalVar->setText(QApplication::translate("MainWindow", "Report public, global variables that are not used in the application.", 0, QApplication::UnicodeUTF8));
        cpHiddenGlobalVariables->setText(QApplication::translate("MainWindow", "Report global variables that are hidden by local variables.", 0, QApplication::UnicodeUTF8));
        cpParametersPostfix->setText(QApplication::translate("MainWindow", "Report program parameters that do not have postfix \"_\".", 0, QApplication::UnicodeUTF8));
        cpReportsToDos->setText(QApplication::translate("MainWindow", "Report TODOs.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
