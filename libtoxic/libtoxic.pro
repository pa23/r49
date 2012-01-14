#    libtoxic
#    Calculation of modes and specific emissions for stationary
#    diesel engine test cycles.
#
#    File: libtoxic.pro
#
#    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>
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

QT -= gui
TARGET = toxic
TEMPLATE = lib
VERSION = 6.3.0
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
    DESTDIR = build\\win\\bin
    RCC_DIR = build\\win\\rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build\\win\\debug
    }
    else {
        OBJECTS_DIR = build\\win\\release
    }
}
