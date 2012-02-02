TEMPLATE = app
QT = gui \
    core \
    xml \
    network
CONFIG += qt \
    warn_on
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
HEADERS = src/mainwindowimpl.h
SOURCES = src/mainwindowimpl.cpp \
    src/main.cpp
INCLUDEPATH += ../lib \
    ../../cs8ControllerComponent/lib/src \
    ../lib/src
FORMS += ui/mainwindow.ui
LIBS += ../../lib/libcs8ProjectComponent.a \
    -L../../lib/ \
    -lcs8ControllerComponent
TARGETDEPS += ../../lib/libcs8ProjectComponent.a
RCC_DIR = build
