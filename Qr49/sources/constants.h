/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: constants.h

    Copyright (C) 2009-2017 Artem Petrov <pa2311@gmail.com>

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

#ifndef CONSTANTS_H
#define	CONSTANTS_H

#include <QString>
#include <QDir>

const QString QR49NAME = "Qr49";
const QString QR49VERSION = "3.12.9";
const QString CONFIGFILENAME =
        "r49data"
        + QString(QDir::separator())
        + "r49.conf";
const QString TEMPLATEDIRNAME =
        "r49data"
        + QString(QDir::separator())
        + "Templates";
const QString DOCDIRNAME = "r49data/Documentation"; // only relative path

#endif // CONSTANTS_H
