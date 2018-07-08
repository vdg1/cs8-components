#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T16:50:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MCPWidget
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


MCPWidgetExample.depends=lib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponentd

INCLUDEPATH += $$PWD/../lib
DEPENDPATH += $$PWD/../lib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ControllerComponent.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ControllerComponentd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ControllerComponent.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ControllerComponentd.lib
