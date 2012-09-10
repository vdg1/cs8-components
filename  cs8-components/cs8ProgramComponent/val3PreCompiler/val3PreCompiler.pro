#-------------------------------------------------
#
# Project created by QtCreator 2012-02-02T18:54:55
#
#-------------------------------------------------

QT       += core gui xml

TARGET = val3Check
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += ../../lib/libcs8ProjectComponent.a

INCLUDEPATH +=  ../lib\
                ../lib/src


win32:RC_FILE = val3Compiler.rc

OTHER_FILES += \
    val3Compiler.rc
