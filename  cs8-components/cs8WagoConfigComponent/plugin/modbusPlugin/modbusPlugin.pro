TEMPLATE = lib
HEADERS += src/cs8modbusplugin.h
SOURCES += src/cs8modbusplugin.cpp
CONFIG += designer plugin warn_on release
QT += core gui network
INSTALLS = target
target.path = $$[QT_INSTALL_PLUGINS]/designer
INCLUDEPATH = ../../lib/src ../../lib/build
TARGET = modbuswidgetplugin
LIBS = -lcs8WagoConfigComponent -L../../../lib -lcs8ControllerComponent