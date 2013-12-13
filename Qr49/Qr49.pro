#
#    Qr49
#    Calculation of modes and specific emissions for stationary
#    diesel engine test cycles.
#
#    File: Qr49.pro
#
#    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU General Public License for more details.
#    You should have received a copy of the GNU General Public License
#    along with this program. If not, see <http://www.gnu.org/licenses/>.
#

QT += core gui widgets printsupport

TARGET = Qr49

TEMPLATE = app

VERSION = 3.11.1

SOURCES += sources/preferencesdialog.cpp \
    sources/undoredotable.cpp \
    sources/checkoutdatadialog.cpp \
    sources/valuedialog.cpp \
    sources/filtermassdialog.cpp \
    sources/main.cpp \
    sources/qr49.cpp \
    sources/dataimportdialog.cpp \
    sources/tablewidgetfunctions.cpp \
    sources/reportsettingsdialog.cpp

HEADERS += sources/preferencesdialog.h \
    sources/checkoutdatadialog.h \
    sources/undoredotable.h \
    sources/valuedialog.h \
    sources/filtermassdialog.h \
    sources/qr49.h \
    sources/dataimportdialog.h \
    sources/tablewidgetfunctions.h \
    sources/constants.h \
    sources/reportsettingsdialog.h

FORMS += sources/preferencesdialog.ui \
    sources/checkoutdatadialog.ui \
    sources/valuedialog.ui \
    sources/filtermassdialog.ui \
    sources/qr49.ui \
    sources/dataimportdialog.ui \
    sources/reportsettingsdialog.ui

RESOURCES = sources/qr49.qrc

TRANSLATIONS = translations/qr49_ru.ts

RC_FILE += sources/qr49.rc

QMAKE_CXXFLAGS += -std=c++11 -W -pedantic

unix: {
    INCLUDEPATH += .. \
        ../libtoxic/sources
    LIBS += -L../r49-bin/unix/libs/r49 \
        -ltoxic \
        -Wl,--rpath=. \
        -Wl,--rpath=libs/r49 \
        -Wl,--rpath=libs/Qt \
        -Wl,--rpath=libs/common
    DESTDIR = ../r49-bin/unix
    MOC_DIR = build/unix/moc
    RCC_DIR = build/unix/rc
    UI_DIR = build/unix/ui_h
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
    target.path = $$PREFIX/bin
    INSTALLS += target
}

win32: {
    INCLUDEPATH += .. \
        ../libtoxic/sources
    LIBS += -L../r49-bin/win/libs/r49 \
        -ltoxic7 \
        -Wl,--rpath=. \
        -Wl,--rpath=libs/r49 \
        -Wl,--rpath=libs/Qt \
        -Wl,--rpath=libs/common
    DESTDIR = ../r49-bin/win
    MOC_DIR = build/win/moc
    RCC_DIR = build/win/rc
    UI_DIR = build/win/ui_h
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/win/debug
    }
    else {
        OBJECTS_DIR = build/win/release
    }
}

updateqm.input = TRANSLATIONS
updateqm.output = qr49_ru.qm
updateqm.commands = \
    lrelease Qr49.pro &&\
    $(MKDIR) ${DESTDIR}langs/ &&\
    $(COPY) translations/*.qm ${DESTDIR}langs/
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
