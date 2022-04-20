#-------------------------------------------------
#
# Project created by QtCreator 2012-02-02T18:54:55
#
#-------------------------------------------------

QT       += core xml concurrent
TARGET = val3Check
TEMPLATE = app

DESTDIR = ../bin/preCompilerPackage

SOURCES += main.cpp \
    ../common/versionInfo.cpp \
    cs8linter.cpp

EXTRA_FILES.path    = $${DESTDIR}
EXTRA_FILES.files   += $$(QTDIR)/bin/Qt5Core.dll
EXTRA_FILES.files   += $$(QTDIR)/bin/Qt5Gui.dll
EXTRA_FILES.files   += $$(QTDIR)/bin/Qt5Xml.dll
INSTALLS       += EXTRA_FILES

win32:RC_FILE = val3Compiler.rc

OTHER_FILES += \
    val3Compiler.rc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponentd

INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponent.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponentd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponent.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponentd.lib

LIBS += -lversion

RESOURCES += \
    val3precompiler.qrc

HEADERS += \
    ../common/versionInfo.h \
    cs8linter.h

# CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
