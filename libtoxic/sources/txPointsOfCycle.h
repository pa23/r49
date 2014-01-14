/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txPointsOfCycle.h

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

#ifndef TXPOINTSOFCYCLE_H
#define TXPOINTSOFCYCLE_H

#include "txCalculationInterface.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace toxic {

class txPointsOfCycle : public txCalculationInterface {

public:

    txPointsOfCycle(const QSharedPointer<txCommonParameters> &,
                    const QSharedPointer<txCalculationOptions> &);
    virtual ~txPointsOfCycle();

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

    txPointsOfCycle(const txPointsOfCycle &);
    txPointsOfCycle & operator=(const txPointsOfCycle &);

    void prepSrcData(const QVector< QVector<double> > &);
    void prepArrays();

    QSharedPointer<txCommonParameters> m_commonParameters;
    QSharedPointer<txCalculationOptions> m_calculationOptions;

    ptrdiff_t m_numberOfPoints;

    QVector<double> ma_n;
    QVector<double> ma_Me_brutto;
    QVector<double> ma_Ne_brutto;
    QVector<double> ma_N_fan;
    QVector<double> ma_w;

    double m_n_hi, m_n_lo;
    double m_A, m_B, m_C, m_a1, m_a2, m_a3, m_n_ref;
    double m_idle, m_n_rated;
    double m_N_fan_rated;
    double m_Ne_A, m_Ne_B, m_Ne_C, m_Ne_a1, m_Ne_a2, m_Ne_a3;
    double m_n_interim, m_Ne_interim, m_Ne_rated;

    QString m_fullReportsPath;

};

} // namespace toxic

#endif // TXPOINTSOFCYCLE_H
