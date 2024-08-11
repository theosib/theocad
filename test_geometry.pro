# Project configuration
TEMPLATE = app
TARGET = test_geometry
CONFIG += console c++17 warn_on release

# Compiler and linker settings
QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -Wall -Wextra -g

# Include paths
INCLUDEPATH += /usr/include \
               /usr/local/include \
               /opt/homebrew/Cellar/eigen/3.4.0_1/include \
               /opt/homebrew/Cellar/boost/1.85.0/include

# Library paths
QMAKE_LFLAGS += -L/usr/lib \
                -L/usr/local/lib \
                -L/opt/homebrew/Cellar/boost/1.85.0/lib

# Libraries to link
LIBS += -lboost_system

# Source files
SOURCES += bodies.cpp \
           geometry.cpp \
           test.cpp \
           cad_visualizer.cpp \
           triangle.cpp \
           rational_circle.cpp \
           transforms.cpp

# Header files (optional, for clarity)
HEADERS += bodies.hpp \
           cad_visualizer.hpp \
           geometry.hpp \
           rational_circle.hpp \
           transforms.hpp \
           collections.hpp

# Qt modules
QT += core gui widgets 3dcore 3drender 3dextras 3dinput 3dlogic

# Clean
QMAKE_CLEAN += $(OBJECTS) $(TARGET)
