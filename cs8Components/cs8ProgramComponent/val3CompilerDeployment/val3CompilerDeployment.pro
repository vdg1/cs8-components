#-------------------------------------------------
#
# Project created by QtCreator 2012-09-04T21:29:09
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets 3dcore

TARGET = val3LinterControlPanel
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
    ../common/versionInfo.cpp \
        mainwindow.cpp \
    dialogdeploy.cpp \
    val3PrecompilerSettings.cpp \
    chm.cpp \
    tagreader.cpp

HEADERS  += mainwindow.h \
    ../common/versionInfo.h \
    dialogdeploy.h \
    val3PrecompilerSettings.h \
    chmxx.h \
    tagreader.h

FORMS    += mainwindow.ui \
    dialogdeploy.ui \
    val3PrecompilerSettings.ui

LIBS = -lversion

OTHER_FILES += \
    val3CompilerDeployment.rc \
    val3CompilerDeployment.exe.manifest


win32:RC_FILE += \
    val3CompilerDeployment.rc \
    val3CompilerDeployment.exe.manifest

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponentd

INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponent.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponentd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponent.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponentd.lib

RESOURCES += \
    val3compilerdeployment.qrc



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../chmlib/ -lchmlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../chmlib/ -lchmlibd

INCLUDEPATH += $$PWD/../chmlib/src
DEPENDPATH += $$PWD/../chmlib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libchmlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libchmlibd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/chmlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/chmlibd.lib
