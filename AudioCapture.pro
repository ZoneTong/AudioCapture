#-------------------------------------------------
#
# Project created by QtCreator 2019-04-17T19:40:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioCapture
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    audio.cpp

HEADERS  += mainwindow.h \
    audio.h

FORMS    +=

#LIBS += -L$$PWD/lib/ -lOle32
LIBS +=  -lOle32
