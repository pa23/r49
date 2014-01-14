/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txReducedPower.h

    Copyright (C) 2009-2014 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXREDUCEDPOWER_H
#define TXREDUCEDPOWER_H

#include "txCalculationInterface.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"

#include <QString>
#include <QVector>
#include <QSharedPointer>

namespace toxic {

class txReducedPower : public txCalculationInterface {

public:

    txReducedPower(const QSharedPointer<txCommonParameters> &,
                   const QSharedPointer<txCalculationOptions> &);
    virtual ~txReducedPower();

    void setSourceData(const QVector<QVector<double> > &);
    void setSourceData(); // srcFileNamePoints from CommonParameters will be used
    void calculate();
    QString results() const {
        return "";
    }
    QString createReports();
    QString lastReportDir() const {
        return m_fullReportsPath;
    }

private:

    txReducedPower(const txReducedPower &);
    txReducedPower & operator=(const txReducedPower &);

    void prepSrcArrays(const QVector< QVector<double> > &);
    void defRate();
    QString saveSourceData() const;
    QString saveCheckoutData() const;

    QSharedPointer<txCommonParameters> m_commonParameters;
    QSharedPointer<txCalculationOptions> m_calculationOptions;

    ptrdiff_t m_numberOfPoints;

    QVector<double> ma_n;
    QVector<double> ma_Me_brutto;
    QVector<double> ma_t0;
    QVector<double> ma_B0;
    QVector<double> ma_Ra;
    QVector<double> ma_S;
    QVector<double> ma_pk;
    QVector<double> ma_Gfuel;
    QVector<double> ma_N_k;
    QVector<double> ma_N_fan;
    QVector<double> ma_t_cool;
    QVector<double> ma_t_oil;
    QVector<double> ma_tk;
    QVector<double> ma_tks;
    QVector<double> ma_t_fuel;
    QVector<double> ma_pks;
    QVector<double> ma_Gair;

    QVector<double> ma_Ne_brutto;
    QVector<double> ma_ge_brutto;
    QVector<double> ma_qcs;
    QVector<double> ma_fm;
    QVector<double> ma_pa;
    QVector<double> ma_ps;
    QVector<double> ma_fa;
    QVector<double> ma_alphad;
    QVector<double> ma_Ne_reduced;
    QVector<double> ma_Ne_netto_reduced;
    QVector<double> ma_Me_netto_reduced;
    QVector<double> ma_ge_netto_reduced;

    QString m_currTime;
    QString m_fullReportsPath;

    double m_n_rated;
    ptrdiff_t m_i_rated;
    double m_N_fan_rated;

};

} // namespace toxic

#endif // TXREDUCEDPOWER_H
