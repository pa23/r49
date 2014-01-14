#
#    cr49
#    Calculation of modes and specific emissions for stationary
#    diesel engine test cycles.
#
#    File: cr49.pro
#
#    Copyright (C) 2009-2014 Artem Petrov <pa2311@gmail.com>
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

QT += core
QT -= gui

TARGET = cr49

CONFIG += console

TEMPLATE = app

VERSION = 1.3.0
VER_MAJ = 1
VER_MIN = 3
VER_PAT = 0

SOURCES += sources/main.cpp
HEADERS += sources/constants.h

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
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/win/debug
    }
    else {
        OBJECTS_DIR = build/win/release
    }
}
