QT += core
QT -= gui

TARGET = cr49
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    cr49constants.h \
    ../r49.h

unix: {
    INCLUDEPATH += .. \
        ../libtoxic/sources
    LIBS += -L../libtoxic/build/unix/bin \
        -ltoxic \
        -Wl,-rpath,.
    DESTDIR = build/unix/bin
    MOC_DIR = build/unix/moc
    RCC_DIR = build/unix/rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
}
win32: {
    INCLUDEPATH += .. \
        ..\\libtoxic\\sources
    LIBS += -L..\\libtoxic\\build\\win\\bin \
        -ltoxic6 \
        -Wl,-rpath,.
    DESTDIR = build\\win\\bin
    MOC_DIR = build\\win\\moc
    RCC_DIR = build\\win\\rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build\\win\\debug
    }
    else {
        OBJECTS_DIR = build\\win\\release
    }
}
