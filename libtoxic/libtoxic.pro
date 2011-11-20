QT -= gui
TARGET = toxic
TEMPLATE = lib
VERSION = 6.1.0
SOURCES += sources/reducedpower.cpp \
    sources/precalc.cpp \
    sources/libtoxicparameters.cpp \
    sources/emissionlimits.cpp \
    sources/cyclepoints.cpp \
    sources/cycleemissions.cpp \
    sources/commonparameters.cpp \
    sources/csvread.cpp \
    sources/toxicerror.cpp
HEADERS += sources/reducedpower.h \
    sources/precalc.h \
    sources/libtoxicparameters.h \
    sources/libtoxicconstants.h \
    sources/emissionlimits.h \
    sources/cyclepoints.h \
    sources/cycleemissions.h \
    sources/commonparameters.h \
    sources/csvread.h \
    ../r49.h \
    sources/toxicerror.h
unix: {
    INCLUDEPATH += ..
        #/usr/include/c++/4.6
    DESTDIR = build/unix/bin
    RCC_DIR = build/unix/rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
}
win32: {
    INCLUDEPATH += ..
        #C:\\MinGW32\\lib\\gcc\\mingw32\\4.5.2\\include\\c++
    DESTDIR = build\\win\\bin
    RCC_DIR = build\\win\\rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build\\win\\debug
    }
    else {
        OBJECTS_DIR = build\\win\\release
    }
}




