QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS LIBTEST

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    core/src/cevent.cpp \
    core/src/ceventloop.cpp \
    core/src/cobject.cpp \
    core/src/cthread.cpp \
    main.cpp \
    mainwindow.cpp \
    net/src/ringbuffer.cpp \
    net/src/tcpconnect.cpp \
    test.cpp \
    tool/src/cbitarray.cpp \
    tool/src/cbytearray.cpp \
    tool/src/cstring.cpp \
    tool/src/logsys.cpp

HEADERS += \
    core/src/cevent.h \
    core/src/ceventloop.h \
    core/src/cobject.h \
    core/src/cobjectdef.h \
    core/src/crefcount.h \
    core/src/cthread.h \
    mainwindow.h \
    net/src/ringbuffer.h \
    net/src/tcpconnect.h \
    test.h \
    tool/src/cbitarray.h \
    tool/src/cbytearray.h \
    tool/src/cstring.h \
    tool/src/logsys.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
