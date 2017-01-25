#-------------------------------------------------
#
# Project created by QtCreator 2015-06-29T16:43:11
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SourceAdminTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    query.cpp \
    serverinfo.cpp \
    settings.cpp \
    worker.cpp \
    customitems.cpp \
    rcon.cpp \
    simplecrypt.cpp \
    main_events.cpp \
    main_slots.cpp \
    rcon_slots.cpp \
    query_slots.cpp \
    loghandler.cpp \
    loghandler_slots.cpp

HEADERS  += mainwindow.h \
    query.h \
    serverinfo.h \
    settings.h \
    worker.h \
    customitems.h \
    rcon.h \
    simplecrypt.h \
    ui_mainwindow.h \
    loghandler.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc

win32 {
    RC_FILE = sourceadmin.rc
    LIBS += -lws2_32
    LIBS += -lIPHlpApi
}
macx {
    ICON = icons/icon.icns
}

INCLUDEPATH += $$PWD/thirdparty/miniupnpc
INCLUDEPATH += $$PWD/thirdparty/libmaxminddb
DEPENDPATH += $$PWD/thirdparty/miniupnpc
DEPENDPATH += $$PWD/thirdparty/libmaxminddb

DISTFILES += \
    GeoLite2-Country.mmdb

win32: LIBS += -L$$PWD/thirdparty/miniupnpc/lib/win/ -lminiupnpc -L$$PWD/thirdparty/libmaxminddb/lib/win/ -lmaxminddb
macx: LIBS += -L$$PWD/thirdparty/miniupnpc/lib/mac/ -lminiupnpc -L$$PWD/thirdparty/libmaxminddb/lib/mac/ -lmaxminddb
