/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: csvread.h

    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>

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

#ifndef CSVREAD_H
#define	CSVREAD_H

#include <QString>
#include <QStringList>
#include <QVector>

class csvRead {

public:

    explicit csvRead(const QString &, const QString &, const ptrdiff_t &);
    virtual ~csvRead();

    void readFile();
    QVector< QVector<double> > csvData() const;
    QStringList csvHeaders() const;

private:

    csvRead(const csvRead &orig);
    csvRead &operator =(const csvRead &);

    QString filename;
    QString csvdelimiter;
    ptrdiff_t headerLinesNumber;

    QVector<QStringList> data;
    QVector< QVector<double> > doubleData;
    QStringList headers;

};

#endif	/* CSVREAD_H */
