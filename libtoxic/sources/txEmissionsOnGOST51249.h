/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnGOST51249.h

    Copyright (C) 2012-2016 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXEMISSIONSONGOST51249_H
#define TXEMISSIONSONGOST51249_H

#include "txEmissionsBase.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace toxic {

class txEmissionsOnGOST51249 : public txEmissionsBase {

public:

    txEmissionsOnGOST51249(const QSharedPointer<txCommonParameters> &,
                           const QSharedPointer<txCalculationOptions> &);
    virtual ~txEmissionsOnGOST51249();

    void setSourceData(const QVector<QVector<double> > &);
    void setSourceData(); // srcFileNamePoints from CommonParameters will be used
    void calculate();
    QString results() const;
    QString createReports();
    QString lastReportDir() const {
        return m_fullReportsPath;
    }

private:

    txEmissionsOnGOST51249(const txEmissionsOnGOST51249 &);
    txEmissionsOnGOST51249 & operator=(const txEmissionsOnGOST51249 &);

    void checkSrcData();
    void setupCalc();
    void prelimCalc();
    void calc_gNOx(); // or CNOx
    void calc_gCO();
    void calc_gCH();
    void calc_Means();
    QString saveCheckoutData() const;
    QString saveReportGAS() const;

    int m_NOxCalcMethod;
    enum { NOXCALCMETHOD_DIRECT, NOXCALCMETHOD_REVERCE };

    int m_gCOcalc;
    enum { GCOCALC_YES, GCOCALC_NO };

    int m_gCHcalc;
    enum { GCHCALC_YES, GCHCALC_NO };

    int m_smoke;
    enum { SMOKE_KA1M, SMOKE_KAPERC, SMOKE_FSN };

    QVector<double> ma_Gexh;
    QVector<double> ma_Gexhd;

    double m_gNOx;
    double m_gCO;
    double m_gCH;
    double m_geMean;
    double m_t0Mean;
    double m_B0Mean;
    double m_RaMean;

};

} // namespace toxic

#endif // TXEMISSIONSONGOST51249_H
