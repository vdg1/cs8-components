
TEMPLATE = app
QT = gui core network \
 xml
CONFIG += qt warn_on  console
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp
INCLUDEPATH = ../lib/src
LIBS += ../../lib/libcs8ControllerComponentd.a
TARGETDEPS += ../../lib/libcs8ControllerComponent.a


