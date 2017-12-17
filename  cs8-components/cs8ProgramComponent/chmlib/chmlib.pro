#-------------------------------------------------
#
# Project created by QtCreator 2015-04-20T15:11:06
#
#-------------------------------------------------

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/chm_lib.c \
    src/enum_chmLib.c \
    src/enumdir_chmLib.c \
    src/extract_chmLib.c \
    src/lzx.c

HEADERS +=  \
    src/chm_lib.h \
    src/lzx.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = ../../lib
win32 {
 build_pass:CONFIG(debug, debug|release) {
    TARGET = chmlibd
    OBJECTS_DIR = buildd
    UI_DIR = build
    MOC_DIR = buildd

 } else {
    TARGET = chmlib
    OBJECTS_DIR = build
    UI_DIR = build
    MOC_DIR = build

  }
}
