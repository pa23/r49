/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: precalc.cpp

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

#include "precalc.h"
#include "libtoxicconstants.h"
#include "toxicerror.h"

#include <QVector>

#include <cmath>

void calcABC(const double &n_hi,
             const double &n_lo,
             double *A,
             double *B,
             double *C,
             double *a1,
             double *a2,
             double *a3,
             double *n_ref) {

    if (n_hi < n_lo) {

        throw ToxicError("n_hi must be greater than n_lo!");
    }

    const double delta = (n_hi - n_lo) / 4.0;

    *A = n_lo + delta;
    *B = *A   + delta;
    *C = *B   + delta;

    *a1 = (*B + *C) / 2.0;
    *a2 = (*A + *B) / 2.0;
    *a3 = (*B + *C) / 2.0;

    *n_ref = n_lo + 0.95 * (n_hi - n_lo);
}

double Gair(const double &Dn,
            const double &B0,
            const double &t0,
            const double &dPn) {

    return 0.0084591 * pow(Dn, 2.0) *
            sqrt((1.019716213 * dPn * 7.500616827 * B0)/(t0 + 273.0));
}

double N_fan(const double &N_fan_rated,
             const double &n,
             const double &n_rated) {

    return N_fan_rated * pow(n / n_rated, 3.0);
}

double Ka1m2KaPerc(const double &Ka1m, const double &L) {

    return 100.0 * (1.0 - exp(- Ka1m * L));
}

double KaPerc2Ka1m(const double &KaPerc, const double &L) {

    return (-1.0 / L) * log(1 - KaPerc / 100.0);
}

double val_Pa(const double &x) {

    for ( ptrdiff_t i=0; i<(PAARRAYSIZE-1); i++ ) {

        if ( (x > ARRAYT0[i]) && (x < ARRAYT0[i+1]) ) {

            return ARRAYPA[i] + (ARRAYPA[i+1] - ARRAYPA[i]) *
                    (x - ARRAYT0[i]) / (ARRAYT0[i+1] - ARRAYT0[i]);
        }
        else if (x == ARRAYT0[i]) {

            return ARRAYPA[i];
        }
        else if (x == ARRAYT0[i+1]) {

            return ARRAYPA[i+1];
        }
    }

    return 0;
}

double val_rhoAir(const double &x) {

    for ( ptrdiff_t i=0; i<(RHOAIRARRAYSIZE-1); i++ ) {

        if ( (x > ARRAYTAIR[i]) && (x < ARRAYTAIR[i+1]) ) {

            return ARRAYRHOAIR[i] + (ARRAYRHOAIR[i+1] - ARRAYRHOAIR[i]) *
                    (x - ARRAYTAIR[i]) / (ARRAYTAIR[i+1] - ARRAYTAIR[i]);
        }
        else if (x == ARRAYTAIR[i]) {

            return ARRAYRHOAIR[i];
        }
        else if (x == ARRAYTAIR[i+1]) {

            return ARRAYRHOAIR[i+1];
        }
    }

    return 0;
}

bool nonZeroArray(const QVector<double> &data) {

    double sum = 0;

    for ( QVector<double>::const_iterator it=data.begin(); it!=data.end();
          ++it ) {

        sum += *it;
    }

    if ( sum < 0.0000001 ) { return false; }
    else                   { return true;  }
}
