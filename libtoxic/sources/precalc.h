/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: precalc.h

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

#ifndef PRECALC_H
#define	PRECALC_H

#include <QVector>

bool calcABC(const double &,
             const double &,
             double *,
             double *,
             double *,
             double *,
             double *,
             double *,
             double *);

bool calcELR(const double &,
             const double &,
             const double &,
             const double &,
             const double &,
             const double &,
             const double &,
             const double &,
             const double &,
             double *);

double calcGair(const double &,
                const double &,
                const double &,
                const double &);

double calcNfan(const double &,
                const double &,
                const double &);

double Ka1m2KaPerc(const double &,
                   const double &);

double KaPerc2Ka1m(const double &,
                   const double &);

double val_Pa(const double &);

bool nonZeroArray(const QVector<double> &);

#endif	/* PRECALC_H */
