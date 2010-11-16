QT += network
TARGET = Qr49
TEMPLATE = app
SOURCES += preferencesdialog.cpp \
    undoredotable.cpp \
    checkoutdatadialog.cpp \
    elrsmokecalcdialog.cpp \
    abcspeedscalcdialog.cpp \
    valuedialog.cpp \
    filtermassdialog.cpp \
    main.cpp \
    qr49.cpp \
    helpdialog.cpp \
    newversions.cpp
HEADERS += preferencesdialog.h \
    checkoutdatadialog.h \
    undoredotable.h \
    elrsmokecalcdialog.h \
    abcspeedscalcdialog.h \
    qr49constants.h \
    valuedialog.h \
    filtermassdialog.h \
    qr49.h \
    helpdialog.h \
    newversions.h
FORMS += preferencesdialog.ui \
    checkoutdatadialog.ui \
    elrsmokecalcdialog.ui \
    abcspeedscalcdialog.ui \
    valuedialog.ui \
    filtermassdialog.ui \
    qr49.ui \
    helpdialog.ui
RESOURCES = qr49.qrc
RC_FILE = qr49icon.rc
TRANSLATIONS = qr49_ru.ts
CONFIG += console
unix: {
    INCLUDEPATH += ../libtoxic/sources
    LIBS += -L../libtoxic/build/unix/bin \
        -ltoxic \
        -Wl,-rpath,.
    DESTDIR = build/unix/bin
    MOC_DIR = build/unix/moc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
}
win32: {
    INCLUDEPATH += c:\builds\libtoxic\sources
    LIBS += -Lc:\builds\libtoxic\build\win\bin \
        -ltoxic4 \
        -Wl,-rpath,.
    DESTDIR = build\win\bin
    MOC_DIR = build\win\moc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build\win\debug
    }
    else {
        OBJECTS_DIR = build\win\release
    }
}
