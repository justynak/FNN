# Makefile for the FNN application (replaces FNN.pro / qmake)
# Requires: qtbase5-dev, libqt5opengl5-dev

QT_INC   := /usr/include/x86_64-linux-gnu/qt5
QT_BIN   := /usr/lib/x86_64-linux-gnu/qt5/bin
MOC      := $(QT_BIN)/moc
UIC      := $(QT_BIN)/uic

BUILD    := build
TARGET   := $(BUILD)/FNN

SOURCES  := main.cpp \
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
            qcustomplot.cpp

MOC_HEADERS := mainwindow.h qcustomplot.h
FORMS       := mainwindow.ui

DEFINES  := -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_PRINTSUPPORT_LIB \
            -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH  := -I. -I$(BUILD) -I$(QT_INC) \
            -I$(QT_INC)/QtOpenGL -I$(QT_INC)/QtPrintSupport \
            -I$(QT_INC)/QtWidgets -I$(QT_INC)/QtGui -I$(QT_INC)/QtCore

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -fPIC $(DEFINES) $(INCPATH)
LIBS     := -lQt5OpenGL -lQt5PrintSupport -lQt5Widgets -lQt5Gui -lQt5Core -lGL

OBJECTS    := $(SOURCES:%.cpp=$(BUILD)/%.o)
MOC_SRCS   := $(MOC_HEADERS:%.h=$(BUILD)/moc_%.cpp)
MOC_OBJS   := $(MOC_SRCS:%.cpp=%.o)
UI_HEADERS := $(FORMS:%.ui=$(BUILD)/ui_%.h)
DEPS       := $(OBJECTS:%.o=%.d) $(MOC_OBJS:%.o=%.d)

all: $(TARGET)

$(TARGET): $(OBJECTS) $(MOC_OBJS)
	$(CXX) -o $@ $^ $(LIBS)

# ui_*.h must exist before any source that includes it is compiled
$(OBJECTS): $(UI_HEADERS)

$(BUILD)/%.o: %.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/moc_%.cpp: %.h | $(BUILD)
	$(MOC) $(DEFINES) $(INCPATH) $< -o $@

$(BUILD)/moc_%.o: $(BUILD)/moc_%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/ui_%.h: %.ui | $(BUILD)
	$(UIC) $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

run: $(TARGET)
	./$(TARGET)

-include $(DEPS)

.PHONY: all clean run
