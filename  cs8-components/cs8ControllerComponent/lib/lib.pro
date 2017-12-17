TEMPLATE = lib
HEADERS +=  src/cs8controller.h \
  #  src/cs8fileinfo.h \
  #  src/cs8dirmodel.h \
    src/cs8networkscanner.h \
    src/qftp.h \
    src/qurlinfo.h \
    src/qttelnet.h \
    src/cs8telnet.h \
    src/cs8ControllerComponent.h \
    src/cs8loginterface.h

SOURCES += src/cs8controller.cpp \
   #  src/cs8fileinfo.cpp \
   # src/cs8dirmodel.cpp \
    src/cs8networkscanner.cpp \
    src/qftp.cpp \
    src/qurlinfo.cpp \
    src/qttelnet.cpp \
    src/cs8telnet.cpp \
    src/cs8loginterface.cpp

QT += core \
    network \
    xml \
concurrent\
printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += staticlib

DESTDIR = ../../lib/



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

LIBS += -lWs2_32

headers.path    = ../../include
headers.files   += $$HEADERS
INSTALLS       += headers
