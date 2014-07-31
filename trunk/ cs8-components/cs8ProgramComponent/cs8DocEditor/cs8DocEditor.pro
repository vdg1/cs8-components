# -------------------------------------------------
# Project created by QtCreator 2010-01-24T20:07:55
# -------------------------------------------------
QT += xml \
    xmlpatterns
TARGET = "Val3 Documentation Editor"
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    highlighter.cpp \
    dialogcopyrighteditor.cpp
HEADERS += mainwindow.h \
    codeeditor.h \
    highlighter.h \
    dialogcopyrighteditor.h
FORMS += mainwindow.ui \
    dialogcopyrighteditor.ui



CONFIG +=qcodedit

OTHER_FILES += \
    Val3copyright.txt

DESTDIR = ../bin

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponentd

INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponent.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponentd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponent.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponentd.lib
