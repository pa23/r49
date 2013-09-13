/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnR49R96.h

    Copyright (C) 2012-2013 Artem Petrov <pa2311@gmail.com>

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

#ifndef TXEMISSIONSONR49R96_H
#define TXEMISSIONSONR49R96_H

#include "txEmissionsBase.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>

namespace toxic {

class txEmissionsOnR49R96 : public txEmissionsBase {

public:

    txEmissionsOnR49R96(const QSharedPointer<txCommonParameters> &,
                        const QSharedPointer<txCalculationOptions> &);
    virtual ~txEmissionsOnR49R96();

    void setSourceData(const QVector<QVector<double> > &);
    void setSourceData(); // srcFileNamePoints from CommonParameters will be used
    void calculate();
    QString results() const;
    QString createReports();
    QString lastReportDir() const {
        return m_fullReportsPath;
    }

private:

    txEmissionsOnR49R96(const txEmissionsOnR49R96 &);
    txEmissionsOnR49R96 & operator=(const txEmissionsOnR49R96 &);

    void checkSrcData();
    void setupCalc();
    void prelimCalc();
    void calc_gNOx(); // or CNOx
    void calc_addPoints();
    void calc_gCO();
    void calc_gCH();
    void calc_gPT();
    void calc_Means();
    QString saveCheckoutData() const;
    QString saveReportGAS() const;
    QString saveReportPT() const;
    QString saveReportHTML() const;

    int m_NOxCalcMethod;
    enum { NOXCALCMETHOD_DIRECT, NOXCALCMETHOD_REVERCE };

    int m_gCOcalc;
    enum { GCOCALC_YES, GCOCALC_NO };

    int m_gCHcalc;
    enum { GCHCALC_YES, GCHCALC_NO };

    int m_gPTcalc;
    enum { GPTCALC_YES, GPTCALC_NO };

    int m_smoke;
    enum { SMOKE_KA1M, SMOKE_KAPERC, SMOKE_FSN };

    int m_qmdwORrd;
    enum { QMDWORRD_QMDW, QMDWORRD_RD };

    QVector<double> ma_Ne_netto;
    QVector<double> ma_Me_netto;
    QVector<double> ma_Gexh;
    QVector<double> ma_Ha;
    QVector<double> ma_Gaird;
    QVector<double> ma_Kw2;
    QVector<double> ma_Ffh;
    QVector<double> ma_Kf;
    QVector<double> ma_Kwr;
    QVector<double> ma_Khd;
    QVector<double> ma_ge_netto;

    QVector<double> ma_mNOx;
    QVector<double> ma_mCO;
    QVector<double> ma_gCO;
    QVector<double> ma_mCH;
    QVector<double> ma_gCH;

    QVector<double> ma_ror;
    QVector<double> ma_CPT;
    QVector<double> ma_mPT;
    QVector<double> ma_gPT;
    QVector<double> ma_qmedf;
    QVector<double> ma_msepi;

    double m_gNOx;
    double m_gCO;
    double m_gCH;
    double m_gPT;
    double m_gPTs;
    double m_gNOx1m;
    double m_gNOx2m;
    double m_gNOx3m;
    double m_gNOx1c;
    double m_gNOx2c;
    double m_gNOx3c;
    double m_diffNOx1;
    double m_diffNOx2;
    double m_diffNOx3;
    double m_geMean;
    double m_t0Mean;
    double m_B0Mean;
    double m_RaMean;

};

} // namespace toxic

#endif // TXEMISSIONSONR49R96_H
