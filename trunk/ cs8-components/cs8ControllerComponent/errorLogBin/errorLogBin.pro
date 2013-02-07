
TEMPLATE = app
QT = gui core network \
 xml
CONFIG += qt warn_on
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build

FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp
INCLUDEPATH = ../lib/src \
        build
#LIBS += ../../lib/libcs8ControllerComponentd.a
#POST_TARGETDEPS += ../../lib/libcs8ControllerComponent.a



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ControllerComponentd
else:unix: LIBS += -L$$OUT_PWD/../lib/ -lcs8ControllerComponent

INCLUDEPATH += $$PWD/../../lib
DEPENDPATH += $$PWD/../../lib

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ControllerComponent.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ControllerComponentd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../lib/libcs8ControllerComponent.a
