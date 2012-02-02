TEMPLATE = app
QT = gui core network xml
CONFIG += qt warn_on console
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h
SOURCES = src/dialogimpl.cpp src/main.cpp
INCLUDEPATH = ../../cs8ControllerComponent/lib/src ../lib/src
LIBS = -L../../lib -lcs8ControllerComponent -lcs8LicenseComponent
DEPENDPATH = ../lib/src
TARGETDEPS += ../../lib/libcs8ControllerComponent.a ../../lib/libcs8LicenseComponent.a

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}
