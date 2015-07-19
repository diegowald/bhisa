#-------------------------------------------------
#
# Project created by QtCreator 2015-07-15T08:40:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftpManger
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    ftpmanager.cpp \
    dlglogin.cpp \
    file.cpp

HEADERS  += mainwindow.h \
    ftpmanager.h \
    dlglogin.h \
    file.h

FORMS    += mainwindow.ui \
    dlglogin.ui

RESOURCES += \
    iconos.qrc

INCLUDEPATH += ../3rdparty/poco-1.6.0/Net/include
INCLUDEPATH += ../3rdparty/poco-1.6.0/Foundation/include


unix|win32: LIBS += -L$$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64/ -lPocoFoundationd

INCLUDEPATH += $$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64
DEPENDPATH += $$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64

unix|win32: LIBS += -L$$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64/ -lPocoNetd

INCLUDEPATH += $$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64
DEPENDPATH += $$PWD/../3rdparty/poco-1.6.0/lib/Linux/x86_64
