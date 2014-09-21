#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T18:12:35
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt7005TCPServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectionmanager.cpp \
    clienthandler.cpp \
    filemanager.cpp

HEADERS  += mainwindow.h \
    connectionmanager.h \
    clienthandler.h \
    filemanager.h \
    ServerDefs.h

FORMS    += mainwindow.ui
