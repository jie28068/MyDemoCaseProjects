#-------------------------------------------------
#
# Project created by QtCreator 2021-07-14T16:36:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoPacking
TEMPLATE = app
debug:   DESTDIR = $$PWD/../bin/debug
release: DESTDIR = $$PWD/../bin/release

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 依赖的第三方运行库、目标文件目录
THIRD_LIB = $$PWD/ThirdLib/*
DEST_DIR = $${DESTDIR}

# 在win下，需将/替换为\\才能正确识别路径
win32 {
    THIRD_LIB = $$replace(THIRD_LIB, /, \\)  # replace函数的第一个参数必须是大写
    DEST_DIR = $$replace(DEST_DIR, /, \\)
}

# 执行第三方库和Qt依赖库拷贝
QMAKE_POST_LINK += $$QMAKE_COPY $${THIRD_LIB} $${DEST_DIR} & \
                   windeployqt -libdir $${DESTDIR} $${DESTDIR}/$${TARGET}.exe
