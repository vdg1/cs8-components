TEMPLATE = lib
QT += core xml
HEADERS += src/cs8license.h
SOURCES += src/cs8license.cpp
TARGET = cs8LicenseComponent
DESTDIR = ../../lib
MOC_DIR = ../build
OBJECTS_DIR = ../build
CONFIG += staticlib


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
