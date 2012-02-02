HEADERS += src/cs8programdataviewplugin.h
SOURCES += src/cs8programdataviewplugin.cpp
QT += gui core
CONFIG += designer plugin  \
 release
INCLUDEPATH = ../../lib/src
TARGET = cs8programdataviewplugin
TEMPLATE = lib
LIBS = -lcs8ProjectComponent -L../../../lib
INSTALLS = target
OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
target.path = $$[QT_INSTALL_PLUGINS]/designer

