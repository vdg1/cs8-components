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
LIBS += ../../lib/libcs8ProjectComponent.a \
    -L../../lib/ \
    -lcs8ControllerComponent
TARGETDEPS += ../../lib/libcs8ProjectComponent.a
RCC_DIR = build
HEADERS = 
