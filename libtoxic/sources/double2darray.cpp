/*
    double2darray.cpp

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

#include "double2darray.h"

Double2DArray::Double2DArray(ptrdiff_t n1, ptrdiff_t m1) :
        n(0), m(0), a2d(0) {

    n = n1; m = m1;

    a2d = new double *[n];
    for (ptrdiff_t i=0; i<n; i++) { a2d[i] = new double[m]; }

    for (ptrdiff_t i=0; i<n; i++) {

        for (ptrdiff_t j=0; j<m; j++) {

            a2d[i][j] = 0;
        }
    }
}

Double2DArray::~Double2DArray() {

    for (ptrdiff_t i=0; i<n; i++) { delete [] a2d[i]; }
    delete [] a2d;
}

Double2DArray::Double2DArray(const Double2DArray &orig) {

    (void)(&orig);
}

Double2DArray &Double2DArray::operator =(const Double2DArray &x) {

    (void)(&x);
    return *this;
}

double **Double2DArray::arrayPointer() const {

    return a2d;
}
