#
# Project created by QtCreator 2011-12-16T12:25:17
#
#-------------------------------------------------

QT       += core gui

TARGET = dehaze
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += statistics.cpp \
    main.cpp \
    matrix.cpp \
    transmission.cpp \
    gui/imagemodel.cpp \
    gui/controller.cpp \
    gui/pixmapview.cpp


HEADERS += \
    matrix.h \
    statistics.h \
    transmission.cpp \
    triple.h \
    gui/imagemodel.h \
    gui/controller.h \
    gui/pixmapview.h
