# -------------------------------------------------
# Project created by QtCreator 2010-01-24T20:07:55
# -------------------------------------------------
QT += xml \
    xmlpatterns
TARGET = cs8DocEditor
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui
INCLUDEPATH += ../lib \
    ../../cs8ControllerComponent/lib/src \
    ../lib/src
LIBS += ../../lib/libcs8ProjectComponent.a \
    -L../../lib/ \
    -lcs8ControllerComponent
TARGETDEPS += ../../lib/libcs8ProjectComponent.a
CONFIG +=qcodedit
