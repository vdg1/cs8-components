TEMPLATE = app
FORMS += ui/cs8WagoConfigMainWindow.ui \
    ui/progressWidget.ui
SOURCES += src/main.cpp \
    src/wagoconfig.cpp \
    src/progressdialogbaseimpl.cpp
HEADERS += src/wagoconfig.h \
    src/progressdialogbaseimpl.h
QT += gui \
    core \
    network \
    xml
INCLUDEPATH = ../lib/src \
    ../lib/build \
    ../lib \
    ../../cs8ControllerComponent/lib/src \
    ../../cs8ControllerComponent/lib/build
LIBS = -lcs8WagoConfigComponent \
    -L../../lib \
    -lcs8ControllerComponent
DESTDIR = ../bin
TARGET = ModbusConfig
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
TARGETDEPS += ../../lib/libcs8WagoConfigComponent.a
RESOURCES += ../../resources/cs8ComponentsResources.qrc
CONFIG += debug \
    console
