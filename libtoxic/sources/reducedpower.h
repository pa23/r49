/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: reducedpower.h

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

#ifndef REDUCEDPOWER_H
#define REDUCEDPOWER_H

#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

using std::string;

class ReducedPower {

public:

    explicit ReducedPower(const QSharedPointer<LibtoxicParameters> &, const QSharedPointer<CommonParameters> &);
    virtual ~ReducedPower();

    bool readCSV(const QVector< QVector<double> > &);
    bool reducePower();
    QString createReports();
    QString lastReportsDir() const;

private:

    ReducedPower(const ReducedPower &orig);
    ReducedPower &operator =(const ReducedPower &);

    QSharedPointer<LibtoxicParameters> params;
    QSharedPointer<CommonParameters> config;

    QVector< QVector<double> > array_DataForCalc;   //
    QVector<double> array_n;
    QVector<double> array_Me_brutto;
    QVector<double> array_t0;
    QVector<double> array_B0;
    QVector<double> array_Ra;
    QVector<double> array_S;
    QVector<double> array_pk;
    QVector<double> array_Gfuel;
    QVector<double> array_N_k;
    QVector<double> array_N_fan;
    QVector<double> array_Ne_brutto;                 //
    QVector<double> array_qcs;
    QVector<double> array_fm;
    QVector<double> array_pa;
    QVector<double> array_ps;
    QVector<double> array_fa;
    QVector<double> array_alphad;
    QVector<double> array_Ne_reduced;
    QVector<double> array_Ne_brake_reduced;
    QVector<double> array_Ne_netto_reduced;
    QVector<double> array_Me_netto_reduced;
    QVector<double> array_ge_netto_reduced;

    ptrdiff_t NumberOfPoints;
    QString mytime;
    QString fullReportsPath;
    double n_rated;
    ptrdiff_t i_rated;
    double N_fan_rated;

    void setRate();

};

#endif /* REDUCEDPOWER_H */
