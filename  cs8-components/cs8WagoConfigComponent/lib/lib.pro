TEMPLATE = lib
SOURCES += src/cs8modbusconfigfile.cpp \
 src/cs8modbusdelegate.cpp \
 src/cs8modbusitem.cpp \
 src/cs8wagonode.cpp \
 src/cs8wagonodewidget.cpp \
 src/cs8modbuswidget.cpp \
 src/cs8wagonewaddresswidget.cpp
HEADERS += src/cs8modbusconfigfile.h \
 src/cs8modbusdelegate.h \
 src/cs8modbusitem.h \
 src/cs8wagonode.h \
 src/cs8wagonodewidget.h \
 src/cs8ModbusComponent.h \
 src/cs8modbuswidget.h \
 src/cs8wagonewaddresswidget.h
FORMS += ui/cs8WagoNodeWidget.ui ui/cs8ModbusWidget.ui ui/cs8WagoNewAddress.ui
QT += core xml network gui
CONFIG += staticlib debug
TARGET = cs8WagoConfigComponent
DESTDIR = ../../lib
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
INCLUDEPATH = ../../cs8ControllerComponent/lib/src
LIBS = -lcs8ControllerComponent -L../../cs8ControllerComponent/bin
