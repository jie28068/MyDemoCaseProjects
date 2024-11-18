
contains(DEFINES,WIN64) {
    win64:CONFIG(release, debug|release): LIBS += -L$$PWD/x64/ -llibzmq
    else:win64:CONFIG(debug, debug|release): LIBS += -L$$PWD/x64/ -llibzmqd
} else {
    win32:CONFIG(release, debug|release): LIBS += -L$$PWD/x32/Release -llibzmq
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/x32/Debug -llibzmq
}

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/zmq.hpp \
    $$PWD/zmq_addon.hpp
