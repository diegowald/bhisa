#-------------------------------------------------
#
# Project created by QtCreator 2015-07-15T08:40:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftpManger
TEMPLATE = app


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
