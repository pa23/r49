/*
    csvread.cpp

    Copyright (C) 2009, 2010, 2011 Artem Petrov <pa2311@gmail.com>

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

#include "csvread.h"
#include "libtoxicconstants.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QFile>

csvRead::csvRead(QString fname, QString csvdelim, ptrdiff_t hdrLinesNum) {

    filename = fname;
    csvdelimiter = csvdelim;
    headerLinesNumber = hdrLinesNum;

    readFile();
}

csvRead::~csvRead() {
}

csvRead::csvRead(const csvRead &orig) {

    (void)(&orig);
}

csvRead &csvRead::operator =(const csvRead &x) {

    (void)(&x);
    return *this;
}

void csvRead::readFile() {

    QFile dataFile(filename);

    if ( !(dataFile.open(QIODevice::ReadOnly)) ) {

        qDebug() << Q_FUNC_INFO << ":::" << filename << "not found!";

        return;
    }

    QString s;

    while ( !dataFile.atEnd() ) {

        s = dataFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) ) {

            data.push_back(s.split(csvdelimiter, QString::SkipEmptyParts));
        }
    }

    dataFile.close();

    //

    QVector<double> row;

    for (ptrdiff_t i=headerLinesNumber; i<data.size(); i++) {

        for (ptrdiff_t j=0; j<data.at(i).size(); j++) {

            row.push_back( data.at(i).at(j).toDouble() );
        }

        doubleData.push_back(row);
        row.clear();
    }

    //

    if ( !data.isEmpty() ) {

        headers = data.at(0);
    }
}

QVector< QVector<double> > csvRead::csvData() const {

    return doubleData;
}

QStringList csvRead::csvHeaders() const {

    return headers;
}
