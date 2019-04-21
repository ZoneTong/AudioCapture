#-------------------------------------------------
#
# Project created by QtCreator 2019-04-17T19:40:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioCapture
TEMPLATE = app

RESOURCES = ac.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    audio.cpp

HEADERS  += mainwindow.h \
    audio.h

FORMS    +=

#LIBS += -L$$PWD/lib/ -lOle32

win32{
    DEFINES  += WIN32
    LIBS +=  -lOle32
}

DISTFILES += \
    myQss.qss \
    README.md
