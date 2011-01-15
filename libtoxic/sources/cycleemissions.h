/*
    cycleemissions.h

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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

#include "double2darray.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <string>

#include <QString>

using std::string;

class CycleEmissions {

public:

    explicit CycleEmissions(LibtoxicParameters *, CommonParameters *);
    virtual ~CycleEmissions();

    bool readCSV(double **data = 0, ptrdiff_t n = 0, ptrdiff_t m = 0);
    bool calculate();
    QString createReports(bool);
    QString lastReportsDir();

private:

    CycleEmissions(const CycleEmissions &orig);
    CycleEmissions &operator =(const CycleEmissions &);

    bool GetDataFromCSV_OK;
    bool PreCalculate_OK;
    bool MakeCalculation_gNOx_OK;
    bool MakeCalculation_gCO_OK;
    bool MakeCalculation_gCH_OK;
    bool MakeCalculation_gPT_OK;
    bool MakeCalculation_rEGR_OK;
    bool CompareAlpha_OK;

    bool NenCalcMethod;
    bool GairVals;
    bool NOxCalcMethod;
    bool gCOcalc;
    bool gCHcalc;
    bool EGRcalc;
    ptrdiff_t smoke;
    bool CheckMeas;
    bool qmdwVSrd; // )))

    LibtoxicParameters *params;
    CommonParameters *config;

    Double2DArray *Array_DataForCalc;  double **array_DataForCalc;               //
    Double2DArray *Array_n;            double **array_n;
    Double2DArray *Array_Me_brutto;    double **array_Me_brutto;
    Double2DArray *Array_Ne_brutto;    double **array_Ne_brutto;
    Double2DArray *Array_N_fan;        double **array_N_fan;
    Double2DArray *Array_w;            double **array_w;
    Double2DArray *Array_t0;           double **array_t0;
    Double2DArray *Array_B0;           double **array_B0;
    Double2DArray *Array_Ra;           double **array_Ra;
    Double2DArray *Array_dPn;          double **array_dPn;
    Double2DArray *Array_Gair;         double **array_Gair;
    Double2DArray *Array_Gfuel;        double **array_Gfuel;
    Double2DArray *Array_CNOx;         double **array_CNOx;
    Double2DArray *Array_gNOx;         double **array_gNOx;
    Double2DArray *Array_CCO;          double **array_CCO;
    Double2DArray *Array_CCH;          double **array_CCH;
    Double2DArray *Array_CCO2in;       double **array_CCO2in;
    Double2DArray *Array_CCO2out;      double **array_CCO2out;
    Double2DArray *Array_CO2;          double **array_CO2;
    Double2DArray *Array_Ka1m;         double **array_Ka1m;
    Double2DArray *Array_KaPerc;       double **array_KaPerc;
    Double2DArray *Array_FSN;          double **array_FSN;
    Double2DArray *Array_Pr;           double **array_Pr;
    Double2DArray *Array_ts;           double **array_ts;
    Double2DArray *Array_tauf;         double **array_tauf;
    Double2DArray *Array_qmdw;         double **array_qmdw;
    Double2DArray *Array_qmdew;        double **array_qmdew;
    Double2DArray *Array_rd;           double **array_rd;
    Double2DArray *Array_Ne_netto;     double **array_Ne_netto;                 //
    Double2DArray *Array_Me_netto;     double **array_Me_netto;
    Double2DArray *Array_alpha;        double **array_alpha;
    Double2DArray *Array_alpha_O2;     double **array_alpha_O2;
    Double2DArray *Array_Gexh;         double **array_Gexh;
    Double2DArray *Array_Gexhd;        double **array_Gexhd;
    Double2DArray *Array_Pb;           double **array_Pb;
    Double2DArray *Array_Pa;           double **array_Pa;
    Double2DArray *Array_Ha;           double **array_Ha;
    Double2DArray *Array_Gaird;        double **array_Gaird;
    Double2DArray *Array_Kw2;          double **array_Kw2;
    Double2DArray *Array_Ffh;          double **array_Ffh;
    Double2DArray *Array_Kf;           double **array_Kf;
    Double2DArray *Array_Kwr;          double **array_Kwr;
    Double2DArray *Array_Khd;          double **array_Khd;
    Double2DArray *Array_mNOx;         double **array_mNOx;                     //
    Double2DArray *Array_mCO;          double **array_mCO;
    Double2DArray *Array_gCO;          double **array_gCO;
    Double2DArray *Array_mCH;          double **array_mCH;
    Double2DArray *Array_gCH;          double **array_gCH;
    Double2DArray *Array_ror;          double **array_ror;                      //
    Double2DArray *Array_CPT;          double **array_CPT;
    Double2DArray *Array_mPT;          double **array_mPT;
    Double2DArray *Array_gPT;          double **array_gPT;
    Double2DArray *Array_qmedf;        double **array_qmedf;
    Double2DArray *Array_msepi;        double **array_msepi;
    Double2DArray *Array_rEGR;         double **array_rEGR;                     //
    Double2DArray *Array_alpha_res;    double **array_alpha_res;
    Double2DArray *Array_diff_alpha;   double **array_diff_alpha;               //

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

    std::string mytime;
    QString fullReportsPath;

};

#endif	/* CYCLEEMISSIONS_H */
