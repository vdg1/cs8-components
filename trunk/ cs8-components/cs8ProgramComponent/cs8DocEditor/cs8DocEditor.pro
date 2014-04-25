# -------------------------------------------------
# Project created by QtCreator 2010-01-24T20:07:55
# -------------------------------------------------
QT += xml \
    xmlpatterns
TARGET = "Val3 Documentation Editor"
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    highlighter.cpp
HEADERS += mainwindow.h \
    codeeditor.h \
    highlighter.h
FORMS += mainwindow.ui


INCLUDEPATH += ../lib \
    ../../cs8ControllerComponent/lib/src \
    ../lib/src
LIBS +=   -L../../lib/ \
    -lcs8ControllerComponentd \
    -lcs8ProjectComponentd

CONFIG +=qcodedit

OTHER_FILES += \
    Val3copyright.txt

DESTDIR = ../bin

