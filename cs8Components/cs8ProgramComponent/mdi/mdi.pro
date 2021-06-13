QT += widgets xml webchannel webengine webenginewidgets 3dcore
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
                codeeditor.h \
                formmdichildtest.h \
                highlighter.h \
                mdichild.h
SOURCES       = main.cpp \
                codeeditor.cpp \
                formmdichildtest.cpp \
                highlighter.cpp \
                mainwindow.cpp \
                mdichild.cpp
RESOURCES     = mdi.qrc

FORMS += \
    formmdichildtest.ui \
    mainwindow.ui\
    mdichild.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponent
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcs8ProjectComponentd

INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponent.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcs8ProjectComponentd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponent.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/cs8ProjectComponentd.lib

