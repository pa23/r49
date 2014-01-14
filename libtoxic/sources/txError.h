/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txError.h

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

#ifndef TXERROR_H
#define TXERROR_H

#include <QString>

namespace toxic {

class txError {

public:

    txError(const QString &);
    QString val_toxicErrMsg() const { return m_errMsg; }

private:

    QString m_errMsg;

};

} // namespace toxic

#endif // TXERROR_H
