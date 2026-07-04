#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T12:26:13
# Kept for reference; the Linux build uses the Makefile.
#
#-------------------------------------------------

QT       += core gui printsupport opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FNN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    core/point.h \
    core/henon.h \
    core/ikeda.h \
    core/lorenz.h \
    qcustomplot.h

FORMS    += mainwindow.ui
