/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsBase.h

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXEMISSIONSBASE_H
#define TXEMISSIONSBASE_H

#include "txCalculationInterface.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"

#include <QSharedPointer>
#include <QVector>
#include <QString>

namespace toxic {

class txEmissionsBase : public txCalculationInterface {

public:

    txEmissionsBase();
    virtual ~txEmissionsBase();

protected:

    void prepSrcArrays(const QVector< QVector<double> > &);
    void baseCalc();
    void calc_rEGR();
    void compareAlpha();
    void checkTestConditions();
    QString saveSourceData() const;
    QString saveCalcOptions() const;

    int m_NeCalcMethod;
    enum { NECALCMETHOD_THROUGHME, NECALCMETHOD_THROUGHNE };

    int m_GairVals;
    enum { GAIRVALS_THROUGHMASSFLOW, GAIRVALS_THROUGHNOZZLE };

    int m_EGRcalc;
    enum { EGRCALC_YES, EGRCALC_NO };

    int m_checkMeas;
    enum { CHECKMEAS_YES, CHECKMEAS_NO };

    QSharedPointer<txCommonParameters> m_commonParameters;
    QSharedPointer<txCalculationOptions> m_calculationOptions;

    ptrdiff_t m_numberOfPoints;

    QVector<double> ma_n;
    QVector<double> ma_Me_brutto;
    QVector<double> ma_Ne_brutto;
    QVector<double> ma_N_fan;
    QVector<double> ma_w;
    QVector<double> ma_t0;
    QVector<double> ma_B0;
    QVector<double> ma_Ra;
    QVector<double> ma_dPn;
    QVector<double> ma_Gair;
    QVector<double> ma_Gfuel;
    QVector<double> ma_CNOx;
    QVector<double> ma_gNOx;
    QVector<double> ma_CCO;
    QVector<double> ma_CCH;
    QVector<double> ma_CCO2in;
    QVector<double> ma_CCO2out;
    QVector<double> ma_CO2;
    QVector<double> ma_Ka1m;
    QVector<double> ma_KaPerc;
    QVector<double> ma_FSN;
    QVector<double> ma_Pr;
    QVector<double> ma_ts;
    QVector<double> ma_tauf;
    QVector<double> ma_qmdw;
    QVector<double> ma_qmdew;
    QVector<double> ma_rd;

    QVector<double> ma_alpha;
    QVector<double> ma_alpha_O2;
    QVector<double> ma_Pb;
    QVector<double> ma_Pa;
    QVector<double> ma_fa;
    QVector<double> ma_ge_brutto;

    QVector<double> ma_rEGR;
    QVector<double> ma_alpha_res;

    QVector<double> ma_diff_alpha;

    QString m_currTime;
    QString m_testConditions;
    QString m_fullReportsPath;

};

} // namespace toxic

#endif // TXEMISSIONSBASE_H
