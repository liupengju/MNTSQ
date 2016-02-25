#-------------------------------------------------
#
# Project created by QtCreator 2015-11-01T12:08:34
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MNTSQ
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    connectdialog.cpp

HEADERS  += mainwindow.h \
    connectdialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    icon.qrc
