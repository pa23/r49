QT += core gui network
CONFIG += console
TARGET = Qr49
TEMPLATE = app
SOURCES += preferencesdialog.cpp \
    undoredotable.cpp \
    checkoutdatadialog.cpp \
    valuedialog.cpp \
    filtermassdialog.cpp \
    main.cpp \
    qr49.cpp \
    helpdialog.cpp \
    newversions.cpp \
    dataimportdialog.cpp \
    tablewidgetfunctions.cpp \
    additionalcalculationsdialog.cpp
HEADERS += preferencesdialog.h \
    checkoutdatadialog.h \
    undoredotable.h \
    qr49constants.h \
    valuedialog.h \
    filtermassdialog.h \
    qr49.h \
    helpdialog.h \
    newversions.h \
    ../r49.h \
    dataimportdialog.h \
    tablewidgetfunctions.h \
    additionalcalculationsdialog.h
FORMS += preferencesdialog.ui \
    checkoutdatadialog.ui \
    valuedialog.ui \
    filtermassdialog.ui \
    qr49.ui \
    helpdialog.ui \
    dataimportdialog.ui \
    additionalcalculationsdialog.ui
RESOURCES = qr49.qrc
RC_FILE = qr49icon.rc
TRANSLATIONS = qr49_ru.ts
unix: {
    INCLUDEPATH += ../libtoxic/sources ..
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
    INCLUDEPATH += ..\\libtoxic\\sources ..
    LIBS += -L..\\libtoxic\\build\\win\\bin \
        -ltoxic5 \
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









