/*
    libtoxic

    File: cycleemissions.h

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

#ifndef CYCLEEMISSIONS_H
#define	CYCLEEMISSIONS_H

#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <string>

#include <QSharedPointer>
#include <QString>
#include <QVector>

using std::string;

class CycleEmissions {

public:

    explicit CycleEmissions(QSharedPointer<LibtoxicParameters>, QSharedPointer<CommonParameters>);
    virtual ~CycleEmissions();

    bool readCSV(QVector< QVector<double> >);
    bool calculate();
    QString createReports(bool);
    QString lastReportsDir() const;

private:

    CycleEmissions(const CycleEmissions &orig);
    CycleEmissions &operator =(const CycleEmissions &);

    bool NenCalcMethod;
    bool GairVals;
    bool NOxCalcMethod;
    bool gCOcalc;
    bool gCHcalc;
    bool EGRcalc;
    ptrdiff_t smoke;
    bool CheckMeas;
    bool qmdwVSrd; // )))

    QSharedPointer<LibtoxicParameters> params;
    QSharedPointer<CommonParameters> config;

    QVector< QVector<double> > array_DataForCalc;   //
    QVector<double> array_n;
    QVector<double> array_Me_brutto;
    QVector<double> array_Ne_brutto;
    QVector<double> array_N_fan;
    QVector<double> array_w;
    QVector<double> array_t0;
    QVector<double> array_B0;
    QVector<double> array_Ra;
    QVector<double> array_dPn;
    QVector<double> array_Gair;
    QVector<double> array_Gfuel;
    QVector<double> array_CNOx;
    QVector<double> array_gNOx;
    QVector<double> array_CCO;
    QVector<double> array_CCH;
    QVector<double> array_CCO2in;
    QVector<double> array_CCO2out;
    QVector<double> array_CO2;
    QVector<double> array_Ka1m;
    QVector<double> array_KaPerc;
    QVector<double> array_FSN;
    QVector<double> array_Pr;
    QVector<double> array_ts;
    QVector<double> array_tauf;
    QVector<double> array_qmdw;
    QVector<double> array_qmdew;
    QVector<double> array_rd;
    QVector<double> array_Ne_netto;                 //
    QVector<double> array_Me_netto;
    QVector<double> array_alpha;
    QVector<double> array_alpha_O2;
    QVector<double> array_Gexh;
    QVector<double> array_Gexhd;
    QVector<double> array_Pb;
    QVector<double> array_Pa;
    QVector<double> array_Ha;
    QVector<double> array_Gaird;
    QVector<double> array_Kw2;
    QVector<double> array_Ffh;
    QVector<double> array_Kf;
    QVector<double> array_Kwr;
    QVector<double> array_Khd;
    QVector<double> array_fa;
    QVector<double> array_ge;
    QVector<double> array_mNOx;                     //
    QVector<double> array_mCO;
    QVector<double> array_gCO;
    QVector<double> array_mCH;
    QVector<double> array_gCH;
    QVector<double> array_ror;                      //
    QVector<double> array_CPT;
    QVector<double> array_mPT;
    QVector<double> array_gPT;
    QVector<double> array_qmedf;
    QVector<double> array_msepi;
    QVector<double> array_rEGR;                     //
    QVector<double> array_alpha_res;
    QVector<double> array_diff_alpha;               //

    double gNOx;
    double gCO;
    double gCH;
    double gPT;
    double gPTs;
    double gNOx1m;
    double gNOx2m;
    double gNOx3m;
    double gNOx1c;
    double gNOx2c;
    double gNOx3c;
    double diffNOx1;
    double diffNOx2;
    double diffNOx3;
    double geMean;
    double t0Mean;
    double B0Mean;
    double RaMean;
    double mf1c;
    double mf1d;
    double mf2c;
    double mf2d;
    double mf;
    double qmedfl;
    double msep;
    double mPT;

    ptrdiff_t NumberOfPoints;

    bool preCalculate();
    bool calculate_gNOx(); // or CNOxs
    bool calculateAdditionalPoints();
    bool calculate_gCO();
    bool calculate_gCH();
    bool calculate_gPT();
    bool calculate_rEGR();
    bool calculate_Means();
    bool compareAlpha();
    bool checkTestConditions() const;

    std::string mytime;
    QString fullReportsPath;

};

#endif	/* CYCLEEMISSIONS_H */
