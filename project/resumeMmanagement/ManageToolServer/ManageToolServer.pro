TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
QT  += network sql
#CONFIG -= qt

#基础功能模块
include(../Base/Base.pri)
#数据库模块
include(../DataBase/DataBase.pri)

include(../Model/Model.pri)

include(../ReplyAndService/ReplyAndService.pri)

INCLUDEPATH +=  $$PWD/..
INCLUDEPATH += sessions/

SOURCES += \
        main.cpp \
        managetoolserver.cpp \
        dbmanager.cpp \
        sessions/sessionbaselogin.cpp \
        sessions/sessionbasequerygroupinfo.cpp \
        sessions/sessionbasequerygroupusers.cpp \
        sessions/sessionbasequeryusrinfo.cpp \
        sessions/sessionbasequeryworklog.cpp \
        sessions/sessionbaseregist.cpp \
        sessions/sessionbaseusergroupcreate.cpp \
        sessions/sessionbaseusergroupupdate.cpp \
        sessions/sessionhrmfilterresume.cpp \
        sessions/sessionhrmqueryresume.cpp \
        sessions/sessionhrmupdatearchive.cpp \
        sessions/sessionhrmuploadarchive.cpp \
        sessions/sessionhrmuploadresume.cpp \
        sessions/sessionplancreate.cpp \
        sessions/sessionplanfinish.cpp \
        sessions/sessionplannotify.cpp \
        sessions/sessionplanquery.cpp \
        sessions/sessionplanstatis.cpp \
        sessions/sessionplanupdate.cpp


HEADERS += \
    managetoolserver.h \
    dbmanager.h \
    sessions/sessionbaselogin.h \
    sessions/sessionbasequerygroupinfo.h \
    sessions/sessionbasequerygroupusers.h \
    sessions/sessionbasequeryusrinfo.h \
    sessions/sessionbasequeryworklog.h \
    sessions/sessionbaseregist.h \
    sessions/sessionbaseusergroupcreate.h \
    sessions/sessionbaseusergroupupdate.h \
    sessions/sessionhrmfilterresume.h \
    sessions/sessionhrmqueryresume.h \
    sessions/sessionhrmupdatearchive.h \
    sessions/sessionhrmuploadarchive.h \
    sessions/sessionhrmuploadresume.h \
    sessions/sessionplancreate.h \
    sessions/sessionplanfinish.h \
    sessions/sessionplannotify.h \
    sessions/sessionplanquery.h \
    sessions/sessionplanstatis.h \
    sessions/sessionplanupdate.h \
    sessions/sessions.h

QT       += axcontainer                 #用来支持QAxwidget库
LIBS += -L$$PWD//../lib/ -llibpoppler-qt5
