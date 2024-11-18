QT       += core gui network sql axcontainer
RC_ICONS = ManageTool.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#引入库libzmq消息队列模块
#include(../libzmq/ZeroMQ.pri)
#请求和服务模块
include(../ReplyAndService/ReplyAndService.pri)
#人力资源管理工具模块
include(HRMTool/HRMTool.pri)
include(Plan/Plan.pri)
#独立功能控件
include(FunctionWidgets/FunctionWidgets.pri)
include(../Base/Base.pri)
include(../DataBase/DataBase.pri)
include(../Model/Model.pri)

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/..
INCLUDEPATH += $$PWD

SOURCES += \
    idmanager.cpp \
    main.cpp \
    klwidget.cpp \
    managetoolclient.cpp \
    mtreply.cpp \
    serviceobject.cpp \
    userlogin.cpp

HEADERS += \
    idmanager.h \
    klwidget.h \
    managetoolclient.h \
    mtreply.h \
    serviceobject.h \
    userlogin.h

FORMS += \
    klwidget.ui \
    userlogin.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    style_original.qrc

LIBS += -L$$PWD//../lib/ -llibpoppler-qt5

#LIBS += -L$$PWD//../lib/ -lpoppler-qt5
