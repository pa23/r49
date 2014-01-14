/*
    Qr49
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: tablewidgetfunctions.h

    Copyright (C) 2011-2014 Artem Petrov <pa2311@gmail.com>

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

#ifndef TABLEWIDGETFUNCTIONS_H
#define TABLEWIDGETFUNCTIONS_H

#include <QTableWidget>

const ptrdiff_t tableRowHeight = 20;

enum {
    ADDROWS_ABOVE,
    ADDROWS_BELOW,
    ADDROWS_BOTTOM
};

void addRows(
        QTableWidget *,
        const ptrdiff_t, // number of new rows
        const ptrdiff_t  // insert mode (above or under curren)
        );

void formatRow(QTableWidget *, ptrdiff_t);

#endif // TABLEWIDGETFUNCTIONS_H
