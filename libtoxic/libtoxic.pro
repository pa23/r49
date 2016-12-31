#
#    libtoxic
#    Calculation of modes and specific emissions for stationary
#    diesel engine test cycles.
#
#    File: libtoxic.pro
#
#    Copyright (C) 2009-2016 Artem Petrov <pa2311@gmail.com>
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

QT -= gui

TARGET = toxic

TEMPLATE = lib

VERSION = 7.5.4

HEADERS += \
    sources/txAuxiliaryFunctions.h \
    sources/txCalculationOptions.h \
    sources/txCommonParameters.h \
    sources/txConstants.h \
    sources/txDataReader.h \
    sources/txEmissionLimits.h \
    sources/txEmissionsOnCycle.h \
    sources/txEmissionsOnGOST51249.h \
    sources/txEmissionsOnR49R96.h \
    sources/txIdentification.h \
    sources/txError.h \
    sources/txEmissionsBase.h \
    sources/txCalculationInterface.h \
    sources/txReducedPower.h \
    sources/txPointsOfCycle.h

SOURCES += \
    sources/txAuxiliaryFunctions.cpp \
    sources/txCalculationOptions.cpp \
    sources/txCommonParameters.cpp \
    sources/txDataReader.cpp \
    sources/txEmissionLimits.cpp \
    sources/txEmissionsOnCycle.cpp \
    sources/txEmissionsOnGOST51249.cpp \
    sources/txEmissionsOnR49R96.cpp \
    sources/txError.cpp \
    sources/txEmissionsBase.cpp \
    sources/txReducedPower.cpp \
    sources/txPointsOfCycle.cpp \
    sources/txIdentification.cpp

QMAKE_CXXFLAGS += -std=c++11 -W -pedantic

unix: {
    DESTDIR = ../r49-bin/unix/libs/r49
    RCC_DIR = build/unix/rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/unix/debug
    }
    else {
        OBJECTS_DIR = build/unix/release
    }
    target.path = $$PREFIX/lib
    INSTALLS += target
}

win32: {
    DESTDIR = ../r49-bin/win/libs/r49
    RCC_DIR = build/win/rc
    CONFIG (debug, debug|release) {
        OBJECTS_DIR = build/win/debug
    }
    else {
        OBJECTS_DIR = build/win/release
    }
}
