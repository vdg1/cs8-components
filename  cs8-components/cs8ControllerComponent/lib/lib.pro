TEMPLATE = lib
HEADERS += src/cs8controller.h \
    src/cs8fileengine.h \
    src/cs8fileenginehandler.h \
    src/cs8ControllerComponent.h \
    src/ftpengine.h \
    src/p_cs8ftpinstance.h \
    src/cs8fileengineiterator.h \
    src/cs8fileinfo.h \
    src/cs8dirmodel.h \
    src/cs8logindatadialog.h \
    src/cs8networkscanner.h
SOURCES += src/cs8controller.cpp \
    src/cs8fileengine.cpp \
    src/cs8fileenginehandler.cpp \
    src/ftpengine.cpp \
    src/p_cs8ftpinstance.cpp \
    src/cs8fileengineiterator.cpp \
    src/cs8fileinfo.cpp \
    src/cs8dirmodel.cpp \
    src/cs8logindatadialog.cpp \
    src/cs8networkscanner.cpp

QT += core \
    network \
 xml
CONFIG += staticlib build_all
#TARGET = cs8ControllerComponent
DESTDIR = ../../lib/
#OBJECTS_DIR = build
#UI_DIR = build
#MOC_DIR = build

FORMS += ui/cs8logindatadialog.ui
DEFINES += DLLEXPORT
win32 {
 build_pass:CONFIG(debug, debug|release) {
    TARGET = cs8ControllerComponentd
    OBJECTS_DIR = buildd
    UI_DIR = build
    MOC_DIR = buildd

 } else {
    TARGET = cs8ControllerComponent
    OBJECTS_DIR = build
    UI_DIR = build
    MOC_DIR = build

  }
}
