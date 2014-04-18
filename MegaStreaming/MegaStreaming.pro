#-------------------------------------------------
#
# Project created by QtCreator 2014-04-15T13:36:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MegaStreaming
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    crypt.cpp

HEADERS  += mainwindow.h \
    crypt.h

FORMS    += mainwindow.ui

RESOURCES += \
    logo.qrc
