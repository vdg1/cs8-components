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
LIBS += ../../lib/libcs8ProjectComponent.a \
    -L../../lib/ \
    -lcs8ControllerComponent
POST_TARGETDEPS += ../../lib/libcs8ProjectComponent.a
CONFIG +=qcodedit

OTHER_FILES += \
    Val3copyright.txt
