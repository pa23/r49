/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txReportsProcessing.h

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TXREPORTSPROCESSING_H
#define TXREPORTSPROCESSING_H

#include <QString>

namespace toxic {

void changeEngineInfo(
        const QString &, // path to report file
        const QString &  // text for Engine field
        );

QString createCommonReport( // return path to common report
        const QString &,    // path to gaseous emissions report
        const QString &     // path to PT emissions report
        );

} // namespace toxic

#endif // TXREPORTSPROCESSING_H