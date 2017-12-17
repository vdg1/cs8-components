TEMPLATE = app
QT = core \
    xml \
    gui
CONFIG += qt \
    warn_on
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
SOURCES = src/main.cpp
INCLUDEPATH += ../lib \
    ../../cs8ControllerComponent/lib/src \
    ../lib/src
LIBS +=   -L../../lib/ \
    -lcs8ControllerComponentd \
    -lcs8ProjectComponentd

RCC_DIR = build
HEADERS =
