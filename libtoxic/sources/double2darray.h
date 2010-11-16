/*
    double2darray.h

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

#ifndef _DOUBLE2DARRAY_H
#define	_DOUBLE2DARRAY_H

#include <iostream>

class Double2DArray {

public:
    Double2DArray(ptrdiff_t, ptrdiff_t);
    virtual ~Double2DArray();
    double **GetPointerOnArray() const;

private:
    Double2DArray(const Double2DArray &orig);
    Double2DArray &operator =(const Double2DArray &);
    ptrdiff_t n;
    ptrdiff_t m;
    double **a2d;

};

#endif	/* _DOUBLE2DARRAY_H */
