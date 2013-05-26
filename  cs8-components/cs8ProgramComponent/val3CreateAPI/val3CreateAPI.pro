#-------------------------------------------------
#
# Project created by QtCreator 2012-02-05T16:51:48
#
#-------------------------------------------------

QT       += core gui xml

TARGET = val3CreateAPI
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += ../../lib/cs8ProjectComponent.lib

INCLUDEPATH +=  ../lib\
                ../lib/src

