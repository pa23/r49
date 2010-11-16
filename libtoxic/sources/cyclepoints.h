/*
    cyclepoints.h

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

#ifndef _CYCLEPOINTS_H
#define	_CYCLEPOINTS_H

#include "libtoxicconstants.h"
#include "double2darray.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

class CyclePoints {

public:
    CyclePoints(LibtoxicParameters *, CommonParameters *);
    virtual ~CyclePoints();
    bool GetDataFromCSV(double **data = 0, ptrdiff_t n = 0, ptrdiff_t m = 0);
    bool FillArrays();
    QString CreateReport() const;

private:
    CyclePoints(const CyclePoints &orig);
    CyclePoints &operator =(const CyclePoints &);
    bool FillArrays_OK;
    LibtoxicParameters *params;
    CommonParameters *config;
    ptrdiff_t NumberOfPoints;
    double n_hi, n_lo;
    double A, B, C, a1, a2, a3, n_ref;
    double idle, n_rated;
    double N_fan_rated;
    double Ne_A, Ne_B, Ne_C, Ne_a1, Ne_a2, Ne_a3;
    double n_interim, Ne_interim, Ne_rated;
    Double2DArray *Array_n;            double **array_n;
    Double2DArray *Array_Me_brutto;    double **array_Me_brutto;
    Double2DArray *Array_Ne_brutto;    double **array_Ne_brutto;
    Double2DArray *Array_N_fan;        double **array_N_fan;
    Double2DArray *Array_w;            double **array_w;

};

#endif	/* _CYCLEPOINTS_H */
