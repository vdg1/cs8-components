
QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 3dcore

TARGET = val3LinterSysTray
TEMPLATE = app

DESTDIR = ../bin

RESOURCES += \
    Val3LinterSysTray.qrc

DISTFILES += \
    preferences-desktop-icons.png

SOURCES += \
    main.cpp
