/*
    csvread.cpp

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QFile>

csvRead::csvRead() :
        n(0),
        m(0) {
}

csvRead::~csvRead() {

    data.clear();
}

csvRead::csvRead(const csvRead &orig) {

    (void)(&orig);
}

csvRead &csvRead::operator =(const csvRead &x) {

    (void)(&x);
    return *this;
}

bool csvRead::ReadData(QString filename, QString csvdelimiter, ptrdiff_t *stringsNumber) {

    QFile dataFile(filename);

    if ( !(dataFile.open(QIODevice::ReadOnly)) ) {

        qDebug() << "libfuns ERROR: csvRead: ReadData:" << filename << "not found!";

        return false;
    }

    QString s;
    n = 0;
    m = 0;

    while ( !dataFile.atEnd() ) {

        s = dataFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) ) {

            data.push_back(s.split(csvdelimiter, QString::SkipEmptyParts));

            n++;
        }
    }

    dataFile.close();

    *stringsNumber = n;

    m = data.at(0).size();

    return true;
}

bool csvRead::CheckArrayDimension(ptrdiff_t m1) {

    if (m1 == m) {

        return true;
    }
    else {

        return false;
    }
}

bool csvRead::FillArray(double **array) const {

    for (ptrdiff_t i=0; i<data.size(); i++) {

        for (ptrdiff_t j=0; j<data[i].size(); j++) {

            array[i][j] = data.at(i).at(j).toDouble();
        }
    }

    return true;
}
