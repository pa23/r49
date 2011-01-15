/*
    reducedpower.h

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

#ifndef REDUCEDPOWER_H
#define REDUCEDPOWER_H

#include "double2darray.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <string>

#include <QString>

using std::string;

class ReducedPower {

public:

    explicit ReducedPower(LibtoxicParameters *, CommonParameters *);
    virtual ~ReducedPower();

    bool readCSV(double **data = 0, ptrdiff_t n = 0, ptrdiff_t m = 0);
    bool reducePower();
    QString createReports();
    QString lastReportsDir();

private:

    ReducedPower(const ReducedPower &orig);
    ReducedPower &operator =(const ReducedPower &);

    bool GetDataFromCSV_OK;
    bool ReducePower_OK;

    LibtoxicParameters *params;
    CommonParameters *config;

    Double2DArray *Array_DataForCalc;      double **array_DataForCalc;               //
    Double2DArray *Array_n;                double **array_n;
    Double2DArray *Array_Me_brutto;        double **array_Me_brutto;
    Double2DArray *Array_t0;               double **array_t0;
    Double2DArray *Array_B0;               double **array_B0;
    Double2DArray *Array_Ra;               double **array_Ra;
    Double2DArray *Array_S;                double **array_S;
    Double2DArray *Array_pk;               double **array_pk;
    Double2DArray *Array_Gfuel;            double **array_Gfuel;
    Double2DArray *Array_N_k;              double **array_N_k;
    Double2DArray *Array_N_fan;            double **array_N_fan;
    Double2DArray *Array_Ne_brutto;        double **array_Ne_brutto;                 //
    Double2DArray *Array_qcs;              double **array_qcs;
    Double2DArray *Array_fm;               double **array_fm;
    Double2DArray *Array_pa;               double **array_pa;
    Double2DArray *Array_ps;               double **array_ps;
    Double2DArray *Array_fa;               double **array_fa;
    Double2DArray *Array_alphad;           double **array_alphad;
    Double2DArray *Array_Ne_reduced;       double **array_Ne_reduced;
    Double2DArray *Array_Ne_brake_reduced; double **array_Ne_brake_reduced;
    Double2DArray *Array_Ne_netto_reduced; double **array_Ne_netto_reduced;
    Double2DArray *Array_Me_netto_reduced; double **array_Me_netto_reduced;
    Double2DArray *Array_ge_netto_reduced; double **array_ge_netto_reduced;

    ptrdiff_t NumberOfPoints;
    std::string mytime;
    QString fullReportsPath;
    double n_rated;
    ptrdiff_t i_rated;
    double N_fan_rated;

    void SetRate();

};

#endif /* REDUCEDPOWER_H */
