#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T14:37:05
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MutualInformation
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logisticmapgenerator.cpp \
    mutualinformationsolver.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    logisticmapgenerator.h \
    mutualinformationsolver.h \
    qcustomplot.h

FORMS    += mainwindow.ui
