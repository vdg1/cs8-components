HEADERS += src/cs8programcodeviewplugin.h
SOURCES += src/cs8programcodeviewplugin.cpp
QT += gui core
CONFIG += designer plugin \
 release
INCLUDEPATH = ../../lib/src
TARGET = cs8programcodeviewplugin
TEMPLATE = lib
LIBS = -lcs8ProjectComponent -L../../../lib
INSTALLS = target
OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
target.path = $$[QT_INSTALL_PLUGINS]/designer

