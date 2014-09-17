#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T18:11:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt7005TCPClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clientconnector.cpp

HEADERS  += mainwindow.h \
    clientconnector.h

FORMS    += mainwindow.ui
