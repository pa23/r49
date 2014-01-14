/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txIdentification.cpp

    Copyright (C) 2012-2014 Artem Petrov <pa2311@gmail.com>

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

#include "txIdentification.h"

namespace toxic {

QString toxicIdentification::name() const {
    return "libtoxic";
}

QString toxicIdentification::version() const {
    return TOXIC_VERSION;
}

QString toxicIdentification::description() const {
    return "Calculation of modes and specific emissions for stationary "
            "diesel engine test cycles.";
}

QString toxicIdentification::copyrightYears() const {
    return "2009-2014";
}

QString toxicIdentification::authors() const {
    return "Artem Petrov <pa2311@gmail.com>";
}

QString toxicIdentification::codeHosting() const {
    return "https://github.com/pa23/r49";
}

QString toxicIdentification::authorsBlog() const {
    return "http://pa2311.blogspot.com";
}

QString toxicIdentification::licenseInformation() const {
    return "This program is free software: you can redistribute it "
            "and/or modify it under the terms of the GNU General "
            "Public License as published by the Free Software "
            "Foundation, either version 3 of the License, or (at "
            "your option) any later version. This program is "
            "distributed in the hope that it will be useful, but "
            "WITHOUT ANY WARRANTY; without even the implied warranty "
            "of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
            "See the GNU General Public License for more details. "
            "You should have received a copy of the GNU General Public "
            "License along with this program. If not, see "
            "<http://www.gnu.org/licenses/>.";
}

} // namespace toxic
