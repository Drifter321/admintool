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
    loghandler_slots.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    query.h \
    serverinfo.h \
    settings.h \
    worker.h \
    customitems.h \
    rcon.h \
    simplecrypt.h \
    ui_mainwindow.h \
    loghandler.h \
    util.h

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
    bundle.files = $$PWD/GeoLite2-Country.mmdb $$PWD/commands.xml $$PWD/app_list_map.ini
    bundle.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += bundle
}

INCLUDEPATH += $$PWD/thirdparty/miniupnpc
INCLUDEPATH += $$PWD/thirdparty/libmaxminddb
DEPENDPATH += $$PWD/thirdparty/miniupnpc
DEPENDPATH += $$PWD/thirdparty/libmaxminddb

DISTFILES += \
    GeoLite2-Country.mmdb \
    commands.xml \
    app_list_map.ini

win32: LIBS += -L$$PWD/thirdparty/miniupnpc/lib/win/ -lminiupnpc -L$$PWD/thirdparty/libmaxminddb/lib/win/ -llibmaxminddb
macx: LIBS += -L$$PWD/thirdparty/miniupnpc/lib/mac/ -lminiupnpc -L$$PWD/thirdparty/libmaxminddb/lib/mac/ -lmaxminddb
unix:!macx: QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/lib
unix:!macx: LIBS += -L$$PWD/thirdparty/miniupnpc/lib/linux/ -lminiupnpc -L$$PWD/thirdparty/libmaxminddb/lib/linux/ -lmaxminddb
unix:!macx: DESTDIR = package/admintool
