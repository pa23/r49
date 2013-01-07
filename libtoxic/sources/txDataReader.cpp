/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txDataReader.cpp

    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>

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

#include "txDataReader.h"
#include "txError.h"

#include <QRegExp>
#include <QFile>
#include <QIODevice>

namespace toxic {

txDataReader::txDataReader() {
}

void txDataReader::readFile(const QString &fileName,
                            const QString &delimiter) {

    QFile dataFile(fileName);

    if ( !dataFile.open(QIODevice::ReadOnly) ) {
        throw txError("Can not open data file " + fileName + "!");
    }

    QString str;
    QStringList strlst;
    QRegExp regexp("^[-+]?[0-9]*[.,]?[0-9]+([eE][-+]?[0-9]+)?");
    QVector<double> vec;

    while ( !dataFile.atEnd() ) {

        str = dataFile.readLine().trimmed();

        if ( str.isEmpty() ) {
            continue;
        }

        strlst = str.split(delimiter, QString::SkipEmptyParts);

        if ( strlst[0].contains(regexp) ) {

            for ( ptrdiff_t i=0; i<strlst.size(); i++ ) {
                vec.push_back(strlst[i].toDouble());
            }

            ma_doubleData.push_back(vec);
        }
        else {
            m_headers.push_back(str);
        }

        strlst.clear();
        vec.clear();
    }

    dataFile.close();
}

} // namespace toxic
