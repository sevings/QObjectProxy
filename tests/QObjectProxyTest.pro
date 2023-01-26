QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    ../QObjectProxy.cpp \
    QObjectProxyTest.cpp \
    main.cpp

HEADERS +=  \
    ../QObjectProxy.h \
    QObjectProxyTest.h \
    Subclass.h
