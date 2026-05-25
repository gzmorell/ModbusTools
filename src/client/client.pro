TEMPLATE = app

include(../version.pri)

TARGET = mbclient

CONFIG += no_keywords

DESTDIR  = ../../bin

QT = core gui widgets

unix:QMAKE_RPATHDIR += .

#CONFIG += console
#CONFIG -= app_bundle

INCLUDEPATH += . ..         \
    $$PWD/../../modbus/src  \
    $$PWD/../core/sdk       \
    $$PWD/../core/core      \
    $$PWD/../core           \
    $$PWD/core

include(core/core.pri)
include(project/project.pri)
include(gui/gui.pri)
include(runtime/runtime.pri)

HEADERS +=

SOURCES += \
    main.cpp

LIBS  += -L../../bin -lmbcore
LIBS  += -L../../bin -lmodbus

RC_ICONS = gui/icons/mbclient.ico
