/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txIdentification.h

    Copyright (C) 2012-2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXIDENTIFICATION_H
#define TXIDENTIFICATION_H

#include <QString>

namespace toxic {

#define TOXIC_VERSION "7.2.1"

struct toxicIdentification {

    QString name() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QString authors() const;
    QString codeHosting() const;
    QString authorsBlog() const;
    QString licenseInformation() const;
};

} // namespace toxic

#endif // TXIDENTIFICATION_H
