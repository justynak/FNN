#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T12:26:13
#
#-------------------------------------------------

QT       += core gui printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FNN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    icdata.cpp \
    ctimeseries.cpp \
    cfnndata.cpp \
    cmutualinformationdata.cpp \
    cembeddedsignaldata.cpp \
    icsolution.cpp \
    icsolver.cpp \
    icnotifier.cpp \
    icalgorithm.cpp \
    icdatagenerator.cpp \
    cfnnsolver.cpp \
    cmutualinformationsolver.cpp \
    cembeddingsolver.cpp \
    chenongenerator.cpp \
    clorentzgenerator.cpp \
    cikedagenerator.cpp \
    c3ddataseries.cpp \
    c2ddataseries.cpp \
    icmultipledimdataseries.cpp \
    ../qcustomplot/qcustomplot.cpp

HEADERS  += mainwindow.h \
    icdata.h \
    ctimeseries.h \
    cfnndata.h \
    cmutualinformationdata.h \
    cembeddedsignaldata.h \
    icsolution.h \
    icsolver.h \
    icnotifier.h \
    icalgorithm.h \
    icdatagenerator.h \
    cfnnsolver.h \
    cmutualinformationsolver.h \
    cembeddingsolver.h \
    chenongenerator.h \
    clorentzgenerator.h \
    cikedagenerator.h \
    c3ddataseries.h \
    c2ddataseries.h \
    icmultipledimdataseries.h \
    ../qcustomplot/qcustomplot.h

FORMS    += mainwindow.ui
