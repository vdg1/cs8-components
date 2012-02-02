QT = gui \
    core \
    network
CONFIG += qt \
    warn_on \
    dll \
    debug
DESTDIR = ../bin
OBJECTS_DIR = build
MOC_DIR = build
HEADERS += src/logeventitem.h \
    src/cs8loggerwidget.h \
    src/textEdit.h \
    src/linenumbers.h \
    src/selectionborder.h \
    src/pluginsinterfaces.h \
    src/editor.h \
    src/lineedit.h \
    src/cs8logstatisticswidget.h \
    src/cs8statisticmodel.h \
    src/cs8statisticfilterproxy.h \
    src/cs8statisticplot.h \
    src/cs8plotdata.h \
    src/cs8plotcurve.h \
    src/cs8LoggerComponent.h
SOURCES += src/logeventitem.cpp \
    src/cs8loggerwidget.cpp \
    src/textEdit.cpp \
    src/linenumbers.cpp \
    src/selectionborder.cpp \
    src/editor.cpp \
    src/lineedit.cpp \
    src/cs8logstatisticswidget.cpp \
    src/cs8statisticmodel.cpp \
    src/cs8statisticfilterproxy.cpp \
    src/cs8statisticplot.cpp \
    src/cs8plotdata.cpp \
    src/cs8plotcurve.cpp
TEMPLATE = lib
TARGET = cs8LoggerComponent
FORMS += ui/findwidget.ui \
    ui/gotoline.ui \
    ui/cs8logstatisticswidgetbase.ui \
    ui/cs8plotstatisticswidgetbase.ui
RESOURCES += resources/resources.qrc
unix { 
    SOURCES += 
    LIBS += -lqwt
}
win32 { 
    INCLUDEPATH += C:/Qwt-5.0.2/include
    LIBS += -L../bin \
        -lqwt5d
}
