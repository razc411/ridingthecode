#-------------------------------------------------
#
# Project created by QtCreator 2014-04-22T17:45:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CSXAudio
TEMPLATE = app


SOURCES += main.cpp\
        serverwindow.cpp \
    streamcontroller.cpp \
    tcpacceptor.cpp \
    multicastcontroller.cpp \
    clientinfo.cpp

HEADERS  += serverwindow.h \
    streamcontroller.h \
    tcpacceptor.h \
    multicastcontroller.h \
    clientinfo.h

FORMS    += serverwindow.ui
