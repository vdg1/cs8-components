HEADERS += src/cs8programheaderviewplugin.h
SOURCES += src/cs8programheaderviewplugin.cpp
QT += gui core
CONFIG += designer plugin \
 release
INCLUDEPATH = ../../lib/src
TARGET = cs8programheaderviewplugin
TEMPLATE = lib
LIBS = -lcs8ProjectComponent -L../../../lib
INSTALLS = target
OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
target.path = $$[QT_INSTALL_PLUGINS]/designer

