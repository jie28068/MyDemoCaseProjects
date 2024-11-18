#-------------------------------------------------
#
# Project created by QtCreator 2021-07-05T10:38:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OpenCLKernelCompiler
TEMPLATE = app

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
        Platform.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        Platform.h \
        mainwindow.h

FORMS += \
        mainwindow.ui

win32 { # win
    OPENCL_INSTALL_DIR = $$quote(C:/Program Files (x86)/AMD APP)
    INCLUDEPATH += $$OPENCL_INSTALL_DIR/include
    LIBS += -L$$OPENCL_INSTALL_DIR/lib/x86_64 -lOpenCL
} else { # linux
    OPENCL_INSTALL_DIR = /opt/AMDAPP
    INCLUDEPATH += $$OPENCL_INSTALL_DIR/include
    LIBS += -L$$OPENCL_INSTALL_DIR/lib/x86_64 -lOpenCL
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
