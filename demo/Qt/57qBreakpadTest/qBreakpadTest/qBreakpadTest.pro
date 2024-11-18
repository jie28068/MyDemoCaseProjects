QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

############ for qBreakpad ############
# qBreakpad中需要使用到network模块
QT += network

# 启用多线程、异常、RTTI、STL支持
CONFIG += thread exceptions rtti stl

# without c++11 & AppKit library compiler can't solve address for symbols
CONFIG += c++11
macx: LIBS += -framework AppKit

# 配置头文件搜索路径和链接库路径
INCLUDEPATH += $$PWD/qBreakpad/include
win32 {
    CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/qBreakpad/lib/win/debug -lqBreakpad
    } else {
    LIBS += -L$$PWD/qBreakpad/lib/win/release -lqBreakpad
    }
}
unix {
LIBS += -L$$PWD/qBreakpad/lib/linux -lqBreakpad
}
############ for qBreakpad ############

############ 让release模式下，生成pdb文件或程序内嵌调试信息 ############
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
############ 让release模式下，生成pdb文件或程序内嵌调试信息 ############

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
