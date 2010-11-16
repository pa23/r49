/*
    csvread.h

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

#ifndef _CSVREAD_H
#define	_CSVREAD_H

#include <QString>
#include <QStringList>
#include <QVector>

class csvRead {

public:
    csvRead();
    virtual ~csvRead();
    bool ReadData(QString, QString, ptrdiff_t *);
    bool CheckArrayDimension(ptrdiff_t);
    bool FillArray(double **) const;

private:
    csvRead(const csvRead &orig);
    csvRead &operator =(const csvRead &);
    QVector<QStringList> data;
    ptrdiff_t n;
    ptrdiff_t m;

};

#endif	/* _CSVREAD_H */
