/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txAuxiliaryFunctions.h

    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXAUXILIARYFUNCTIONS_H
#define TXAUXILIARYFUNCTIONS_H

#include <QVector>
#include <QString>

namespace toxic {

void ABC(
        double,   // n_hi
        double,   // n_lo
        double *, // A
        double *, // B
        double *, // C
        double *, // a1
        double *, // a2
        double *, // a3
        double *  // n_ref
        );

double Gair(
        double, // Dn
        double, // B0
        double, // t0
        double  // dPn
        );

double N_fan(
        double, // N_fan_rated
        double, // n
        double  // n_rated
        );

double KaPerc(
        double, // Ka1m
        double  // L
        );

double Ka1m(
        double, // KaPerc
        double  // L
        );

double FSN(
        double // KaPerc
        );

double pa(
        double // t0
        );

double rhoAir(
        double, // B0
        double  // t0
        );

bool zeroArray(const QVector<double> &);

double sum(const QVector<double> &);
double minVal(const QVector<double> &);
double maxVal(const QVector<double> &);
double mean(const QVector<double> &);

QString objDescr();

} // namespace toxic

#endif // TXAUXILIARYFUNCTIONS_H
