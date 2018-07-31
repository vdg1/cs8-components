
TEMPLATE = app
QT = gui core network \
 xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt warn_on
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
TARGET = errorlog2

FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponentd

INCLUDEPATH += $$PWD/../../include
DEPENDPATH += $$PWD/../../include
LIBS += -lWs2_32
win32:CONFIG(release, debug|release):PRE_TARGETDEPS +=$$OUT_PWD/../../lib/cs8ControllerComponent.lib
else:win32:CONFIG(debug, debug|release):PRE_TARGETDEPS +=$$OUT_PWD/../../lib/cs8ControllerComponentd.lib
