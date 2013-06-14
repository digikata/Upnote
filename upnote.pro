#-------------------------------------------------
#
# Project created by QtCreator 2013-06-08T13:17:18
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = upnote
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    searcher.cpp

HEADERS  += mainwindow.h \
    searcher.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    notes.txt
