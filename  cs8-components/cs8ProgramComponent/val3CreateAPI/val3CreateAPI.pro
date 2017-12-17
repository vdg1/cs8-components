#-------------------------------------------------
#
# Project created by QtCreator 2012-02-05T16:51:48
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 3dcore


TARGET = val3CreateAPI
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

#LIBS += ../../lib/cs8ProjectComponent.lib

INCLUDEPATH +=  ../lib


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponentd

INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponent.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponentd.lib
