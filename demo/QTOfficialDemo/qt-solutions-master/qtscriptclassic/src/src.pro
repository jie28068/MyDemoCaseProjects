TARGET     = QtScriptClassic
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += QT_BUILD_SCRIPT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
DEFINES   += QLALR_NO_QSCRIPTGRAMMAR_DEBUG_INFO
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000       ### FIXME

# stuff copied from qbase.pri et al
!isEmpty(RCC_DIR): INCLUDEPATH += $$RCC_DIR
isEmpty(QT_ARCH):!isEmpty(ARCH):QT_ARCH=$$ARCH #another compat that will rot for change #215700
TEMPLATE	= lib
VERSION=1.0.0

mac:!contains(QMAKE_EXT_C, .mm):QMAKE_EXT_C += .mm

#load up the headers info
CONFIG += qt_install_headers
#HEADERS_PRI = $$PWD/headers.pri
#include($$HEADERS_PRI)|clear(HEADERS_PRI)

#other
DESTDIR   = $$PWD/../lib
win32:!wince*:DLLDESTDIR = $$[QT_INSTALL_BINS]

CONFIG		+= qt warn_on depend_includepath
CONFIG          += qmake_cache target_qt 
CONFIG          -= fix_output_dirs
win32|mac:!macx-xcode:CONFIG += debug_and_release build_all
linux-g++*:QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols
unix:contains(QT_CONFIG, reduce_relocations):CONFIG += bsymbolic_functions
contains(QT_CONFIG, largefile):CONFIG += largefile

#mac frameworks
mac:!static:contains(QT_CONFIG, qt_framework) {
   #QMAKE_FRAMEWORK_VERSION = 4.0
   CONFIG += lib_bundle qt_no_framework_direct_includes qt_framework
   CONFIG(debug, debug|release) {
      !build_pass:CONFIG += build_all
   } else { #release
      !debug_and_release|build_pass {
	  CONFIG -= qt_install_headers #no need to install these as well
	  FRAMEWORK_HEADERS.version = Versions
	  FRAMEWORK_HEADERS.files = $$SYNCQT.HEADER_FILES $$SYNCQT.HEADER_CLASSES
      	  FRAMEWORK_HEADERS.path = Headers
      	  equals(TARGET, QtCore) {
              #headers generated by configure
              !contains(FRAMEWORK_HEADERS.files, .*/qconfig.h) {
	          FRAMEWORK_HEADERS.files *= $$QT_BUILD_TREE/src/corelib/global/qconfig.h 
	      }
          }
      }
      QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
   }
}

mac {
   CONFIG += explicitlib
   macx-g++ {
       QMAKE_CFLAGS += -fconstant-cfstrings
       QMAKE_CXXFLAGS += -fconstant-cfstrings
   }
}

win32:!shared:CONFIG += static

win32-borland {
    mng:QMAKE_CFLAGS_WARN_ON	+= -w-par
    mng:QMAKE_CXXFLAGS_WARN_ON	+= -w-par
    # Keep the size of the .tds file for the Qt library smaller than
    # 34 Mbytes to avoid linking problems
    QMAKE_CFLAGS_DEBUG += -vi -y-
    QMAKE_CXXFLAGS_DEBUG += -vi -y-
}

win32 {
    CONFIG += zlib
    INCLUDEPATH += tmp
    !static: DEFINES+=QT_MAKEDLL
}
win32-borland:INCLUDEPATH += kernel

aix-g++* {
     QMAKE_CFLAGS   += -mminimal-toc
     QMAKE_CXXFLAGS += -mminimal-toc
}

embedded {
     EMBEDDED_H = $$EMBEDDED_CPP
}

DEPENDPATH += ;$$NETWORK_H;$$KERNEL_H;$$WIDGETS_H;$$SQL_H;$$TABLE_H;$$DIALOGS_H;
DEPENDPATH += $$ICONVIEW_H;$$OPENGL_H;$$THREAD_H;$$TOOLS_H;$$CODECS_H;
DEPENDPATH += $$WORKSPACE_H;$$XML_H;$$STYLES_H;$$COMPAT_H
embedded:DEPENDPATH += ;$$EMBEDDED_H

!static:PRL_EXPORT_DEFINES += QT_SHARED

# make sure the paths are set up so local apps will run right away
mac:CONFIG += absolute_library_soname
#always install the library
win32 {
   dlltarget.path=$$[QT_INSTALL_BINS]
   INSTALLS += dlltarget
}
target.path=$$DESTDIR
INSTALLS += target

#headers
qt_install_headers {
    INSTALL_HEADERS = $$SYNCQT.HEADER_FILES
    equals(TARGET, QtCore) {
       #headers generated by configure
       INSTALL_HEADERS *= $$QT_BUILD_TREE/src/corelib/global/qconfig.h \
                          $$QT_SOURCE_TREE/src/corelib/arch/$$QT_ARCH/arch
    }

    equals(TARGET, phonon) {
        class_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET/Phonon
    } else {
        flat_headers.files = $$INSTALL_HEADERS
        flat_headers.path = $$[QT_INSTALL_HEADERS]/Qt
        INSTALLS += flat_headers

        class_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET
    }
    class_headers.files = $$SYNCQT.HEADER_CLASSES
    INSTALLS += class_headers

    targ_headers.files = $$INSTALL_HEADERS
    targ_headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET
    INSTALLS += targ_headers
}

contains(QT_PRODUCT, OpenSource.*):DEFINES *= QT_OPENSOURCE
DEFINES += QT_NO_CAST_TO_ASCII QT_ASCII_CAST_WARNINGS
contains(QT_CONFIG, qt3support):DEFINES *= QT3_SUPPORT
DEFINES *= QT_MOC_COMPAT #we don't need warnings from calling moc code in our generated code

TARGET = $$qtLibraryTarget($$TARGET$$QT_LIBINFIX) #do this towards the end
win32:!wince*:QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${TARGET}1.dll

moc_dir.name = moc_location
moc_dir.variable = QMAKE_MOC

uic_dir.name = uic_location
uic_dir.variable = QMAKE_UIC

QMAKE_TARGET_COMPANY = Digia Plc and/or its subsidiary(-ies).
QMAKE_TARGET_PRODUCT = Qt4
QMAKE_TARGET_DESCRIPTION = C++ application development framework.
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).

win32:DEFINES+=_USE_MATH_DEFINES

include(script.pri)