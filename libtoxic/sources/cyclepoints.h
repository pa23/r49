/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: cyclepoints.h

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

#ifndef CYCLEPOINTS_H
#define	CYCLEPOINTS_H

#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <QSharedPointer>
#include <QVector>

class CyclePoints {

public:

    CyclePoints(const QSharedPointer<LibtoxicParameters> &,
                const QSharedPointer<CommonParameters> &);
    ~CyclePoints();

    void readCSV(const QVector< QVector<double> > &);
    void fillArrays();
    QString createReport() const;

private:

    CyclePoints(const CyclePoints &);
    CyclePoints & operator=(const CyclePoints &);

    QSharedPointer<LibtoxicParameters> params;
    QSharedPointer<CommonParameters> config;

    double n_hi, n_lo;
    double A, B, C, a1, a2, a3, n_ref;
    double idle, n_rated;
    double N_fan_rated;
    double Ne_A, Ne_B, Ne_C, Ne_a1, Ne_a2, Ne_a3;
    double n_interim, Ne_interim, Ne_rated;

    QVector<double> array_n;
    QVector<double> array_Me_brutto;
    QVector<double> array_Ne_brutto;
    QVector<double> array_N_fan;
    QVector<double> array_w;

    void arraysInit(const std::ptrdiff_t &);

};

#endif	/* CYCLEPOINTS_H */
