TEMPLATE = app
QT = gui \
    core \
    network
CONFIG += qt \
    warn_on \
    console \
    debug
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp \
    src/main.cpp
LIBS = -lcs8LoggerComponent \
    -L../bin
INCLUDEPATH -= ..\..\lib\src \
    ../../lib/src
unix { 
    INCLUDEPATH += /usr/include/qwt5
    LIBS += -lqwt
}
win32:INCLUDEPATH += C:/Qwt-5.0.2/include
