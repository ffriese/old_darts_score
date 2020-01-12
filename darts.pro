#-------------------------------------------------
#
# Project created by QtCreator 2017-06-19T00:09:30
#
#-------------------------------------------------

QT += core
QT += gui
QT += sql
QT += svg
QT += multimedia
#Qt += mqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
QMAKE_CXXFLAGS+= -std=c++11
QMAKE_LFLAGS +=  -std=c++11
TARGET = darts
TEMPLATE = app
INCLUDEPATH += include
unix{
INCLUDEPATH += external/unix
}
win{
INCLUDEPATH += external/windows
}

DEFINES += QT_DEBUG_PLUGINS

QTPLUGIN += dsengine qtmedia_audioengine
#DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/dartboardwidget.cpp \
    src/dbmanager.cpp \
    src/utils.cpp \
    src/player.cpp \
    src/dartboard.cpp \
    src/take.cpp \
    src/dart.cpp \
    src/gamemanager.cpp \
    src/leg.cpp \
    src/game.cpp \
    src/x01.cpp \
    src/cricket.cpp \
    src/set.cpp \
    src/playerwidget.cpp \
    src/newgamedialog.cpp \
    src/manageplayersdialog.cpp \
    src/globalconfig.cpp \
    src/roundtheclock.cpp \
    src/statviewer.cpp

HEADERS  += \
    include/mainwindow.h \
    include/dartboardwidget.h \
    include/dbmanager.h \
    include/utils.h \
    include/player.h \
    include/dartboard.h \
    include/take.h \
    include/dart.h \
    include/gamemanager.h \
    include/leg.h \
    include/game.h \
    include/x01.h \
    include/gamefactory.h \
    include/cricket.h \
    include/set.h \
    include/playerwidget.h \
    include/newgamedialog.h \
    include/manageplayersdialog.h \
    include/globalconfig.h \
    include/roundtheclock.h \
    include/statviewer.h

FORMS    += \
    forms/mainwindow.ui \
    forms/newgamedialog.ui \
    forms/manageplayersdialog.ui \
    forms/statviewer.ui


win32{
    RC_ICONS = $$PWD/img/app.ico
}
#LIBS +=  -L/usr/local -L/usr -lboost_geometry

message($$PWD)
unix{
     QMAKE_LFLAGS += -Wl,--rpath $$PWD/lib
     LIBS += -L$$PWD/lib -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c
#    QMAKE_LFLAGS += -Wl,--rpath /opt/ros/kinetic/lib
#    LIBS += -L/opt/ros/kinetic/lib -lroscpp -lroscpp_serialization -lrosconsole
#    INCLUDEPATH += "/opt/ros/kinetic/include"
}

CONFIG+=resources_big
RESOURCES += \
    recources.qrc
