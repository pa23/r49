QT -= gui
TARGET = toxic
TEMPLATE = lib
VERSION = 5.0.0
SOURCES += sources/reducedpower.cpp \
    sources/precalc.cpp \
    sources/libtoxicparameters.cpp \
    sources/emissionlimits.cpp \
    sources/cyclepoints.cpp \
    sources/cycleemissions.cpp \
    sources/commonparameters.cpp \
    sources/csvread.cpp \
    sources/datetime.cpp \
    sources/stringoperations.cpp
HEADERS += sources/reducedpower.h \
    sources/precalc.h \
    sources/libtoxicparameters.h \
    sources/libtoxicconstants.h \
    sources/emissionlimits.h \
    sources/cyclepoints.h \
    sources/cycleemissions.h \
    sources/commonparameters.h \
    sources/csvread.h \
    sources/datetime.h \
    sources/stringoperations.h \
    ../r49.h
INCLUDEPATH += ..
unix: {
    DESTDIR = build/unix/bin
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
}
win32: {
    DESTDIR = build\win\bin
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build\win\debug
    }
    else {
        OBJECTS_DIR = build\win\release
    }
}
