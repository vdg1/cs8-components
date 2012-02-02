TEMPLATE = app
TARGET = WagoConfig
QT += core gui network xml
HEADERS += wagoconfig.h \
 cs8wagonode.h \
 cs8modbusconfigfile.h \
 cs8modbusitem.h \
 cs8wagonodewidget.h \
 cs8modbusdelegate.h
SOURCES += main.cpp \
 wagoconfig.cpp \
 cs8wagonode.cpp \
 cs8modbusconfigfile.cpp \
 cs8modbusitem.cpp \
 cs8wagonodewidget.cpp \
 cs8modbusdelegate.cpp
FORMS += ../ui/cs8WagoConfigMainWindow.ui
RESOURCES +=
DESTDIR = ../bin
OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
CONFIG += warn_on console
