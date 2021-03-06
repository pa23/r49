/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txDataReader.h

    Copyright (C) 2009-2016 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXDATAREADER_H
#define TXDATAREADER_H

#include <QString>
#include <QStringList>
#include <QVector>

namespace toxic {

class txDataReader {

public:

    txDataReader();

    void readFile(const QString &,  // filename
                  const QString &); // delimeter

    QVector< QVector<double> > val_data() const {
        return ma_doubleData;
    }

    QStringList val_headers() const {
        return m_headers;
    }

private:

    QVector< QVector<double> > ma_doubleData;
    QStringList m_headers;

};

} // namespace toxic

#endif // DATAREADER_H
