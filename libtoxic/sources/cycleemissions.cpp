/*
    cycleemissions.cpp

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

#include "cycleemissions.h"
#include "csvread.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"
#include "datetime.h"
#include "emissionlimits.h"
#include "precalc.h"
#include "r49.h"

#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <QDebug>
#include <QString>
#include <QDir>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

CycleEmissions::CycleEmissions(LibtoxicParameters *prms, CommonParameters *cfg) :
        GetDataFromCSV_OK       (false),
        PreCalculate_OK         (false),
        MakeCalculation_gNOx_OK (false),
        MakeCalculation_gCO_OK  (false),
        MakeCalculation_gCH_OK  (false),
        MakeCalculation_gPT_OK  (false),
        MakeCalculation_rEGR_OK (false),
        CompareAlpha_OK         (false),
        NenCalcMethod (true),
        GairVals      (true),
        NOxCalcMethod (true),
        gCOcalc       (true),
        gCHcalc       (true),
        EGRcalc       (false),
        smoke         (-1),
        CheckMeas     (false),
        qmdwVSrd      (true),
        gNOx     (0), gCO      (0), gCH      (0), gPT    (0), gPTs (0),
        gNOx1m   (0), gNOx2m   (0), gNOx3m   (0),
        gNOx1c   (0), gNOx2c   (0), gNOx3c   (0),
        diffNOx1 (0), diffNOx2 (0), diffNOx3 (0),
        geMean   (0), t0Mean   (0), B0Mean   (0), RaMean (0),
        mf1c     (0), mf1d     (0), mf2c     (0), mf2d   (0), mf   (0),
        qmedfl   (0), msep     (0), mPT      (0),
        NumberOfPoints (0), mytime ("_"), fullReportsPath("_") {

    params = prms; // calculatin settings
    config = cfg;  // r49.cong file

    if (params->val_CalcConfigFile() != "_._") {

        params->readCalcConfigFile(params->val_CalcConfigFile());
    }
}

CycleEmissions::~CycleEmissions() {

    if (GetDataFromCSV_OK) {

        delete Array_DataForCalc;
        delete Array_n;
        delete Array_Me_brutto;
        delete Array_Ne_brutto;
        delete Array_N_fan;
        delete Array_w;
        delete Array_t0;
        delete Array_B0;
        delete Array_Ra;
        delete Array_dPn;
        delete Array_Gair;
        delete Array_Gfuel;
        delete Array_CNOx;
        delete Array_gNOx;
        delete Array_CCO;
        delete Array_CCH;
        delete Array_CCO2in;
        delete Array_CCO2out;
        delete Array_CO2;
        delete Array_Ka1m;
        delete Array_KaPerc;
        delete Array_FSN;
        delete Array_Pr;
        delete Array_ts;
        delete Array_tauf;
        delete Array_qmdw;
        delete Array_qmdew;
        delete Array_rd;
    }

    if (PreCalculate_OK) {

        delete Array_Ne_netto;
        delete Array_Me_netto;
        delete Array_alpha;
        delete Array_alpha_O2;
        delete Array_Gexh;
        delete Array_Gexhd;
        delete Array_Pb;
        delete Array_Pa;
        delete Array_Ha;
        delete Array_Gaird;
        delete Array_Kw2;
        delete Array_Ffh;
        delete Array_Kf;
        delete Array_Kwr;
        delete Array_Khd;
    }

    if (MakeCalculation_gNOx_OK) {

        delete Array_mNOx;
    }

    if (MakeCalculation_gCO_OK) {

        delete Array_mCO;
        delete Array_gCO;
    }

    if (MakeCalculation_gCH_OK) {

        delete Array_mCH;
        delete Array_gCH;
    }

    if (MakeCalculation_gPT_OK) {

        delete Array_ror;
        delete Array_CPT;
        delete Array_mPT;
        delete Array_gPT;
        delete Array_qmedf;
        delete Array_msepi;
    }

    if (MakeCalculation_rEGR_OK) {

        delete Array_rEGR;
        delete Array_alpha_res;
    }

    if (CompareAlpha_OK) {

        delete Array_diff_alpha;
    }
}

CycleEmissions::CycleEmissions(const CycleEmissions &orig) {

    (void)(&orig);
}

CycleEmissions &CycleEmissions::operator =(const CycleEmissions &x) {

    (void)(&x);
    return *this;
}

bool CycleEmissions::calculate() {

    QString std = params->val_Standard();

    if (!preCalculate()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: preCalculate function returns false!";

        return false;
    }

    if (!calculate_gNOx()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_gNOx function returns false!";

        return false;
    }

    if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) &&
           (params->val_AddPointsCalc() == "yes") && (NumberOfPoints == TCyclePointsNumber) ) {

        if (!calculateAdditionalPoints()) {

            qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculateAdditionalPoints function returns false!";

            return false;
        }
    }

    if (!calculate_gCO()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_gCO function returns false!";

        return false;
    }

    if (!calculate_gCH()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_gCH function returns false!";

        return false;
    }

    if (!calculate_gPT()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_gPT function returns false!";

        return false;
    }

    if (!calculate_rEGR()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_rEGR function returns false!";

        return false;
    }

    if (params->val_Standard() != "FreeCalc") {

        if (!calculate_Means()) {

            qDebug() << "libtoxic ERROR: CycleEmissions: calculate: calculate_Means function returns false!";

            return false;
        }
    }

    if (!compareAlpha()) {

        qDebug() << "libtoxic ERROR: CycleEmissions: calculate: compareAlpha function returns false!";

        return false;
    }

    return true;
}

bool CycleEmissions::readCSV(double **data, ptrdiff_t n, ptrdiff_t m) {

    GetDataFromCSV_OK = false;

    if ( (n == 0) && (m == 0) ) {

        QString filenamePoints = config->val_filenamePoints();
        QString csvdelimiter = config->val_csvDelimiter();

        csvRead *ReaderDataForCalc = new csvRead();

        if (!ReaderDataForCalc->readData(filenamePoints, csvdelimiter, &NumberOfPoints)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: readData function returns false!";

            return false;
        }

        Array_DataForCalc = new Double2DArray(NumberOfPoints, PointsFileColumnsNumber);
        array_DataForCalc = Array_DataForCalc->arrayPointer();

        if (!ReaderDataForCalc->checkArrayDimension(PointsFileColumnsNumber)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: checkArrayDimension function returns false!";

            return false;
        }

        if (!ReaderDataForCalc->fillArray(array_DataForCalc)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: fillArray function returns false!";

            return false;
        }

        NumberOfPoints -= StrsNumberForColumnCaption;

        delete ReaderDataForCalc;
    }
    else if ( (n > 0) && (m > 0) ) {

        Array_DataForCalc = new Double2DArray(1, 1);
        array_DataForCalc = data;

        NumberOfPoints = n;
    }
    else {

        qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Illegal data!";

        return false;
    }

    QString std   = params->val_Standard();
    QString addpc = params->val_AddPointsCalc();

    if (
            (
                    (
                            (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3")
                    ) &&
                    (
                            (addpc == "yes")
                    ) &&
                    (
                            (NumberOfPoints != TCyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3")
                    ) &&
                    (
                            (addpc == "no")
                    ) &&
                    (
                            (NumberOfPoints != (TCyclePointsNumber-TCycleAddPointsNumber))
                    )
            ) ||
            (
                    (
                            (std == "EU2") || (std == "EU1") || (std == "EU0") || (std == "OST") || (std == "GOST")
                    ) &&
                    (
                            (NumberOfPoints != (TCyclePointsNumber-TCycleAddPointsNumber))
                    )
            ) ||
            (
                    (
                            (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") || (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8")
                    ) &&
                    (
                            (NumberOfPoints != ECyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") || (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5")
                    ) &&
                    (
                            (NumberOfPoints != FCyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "C1")
                    ) &&
                    (
                            (NumberOfPoints != GC1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "D1")
                    ) &&
                    (
                            (NumberOfPoints != GD1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "D2")
                    ) &&
                    (
                            (NumberOfPoints != GD2CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "E1")
                    ) &&
                    (
                            (NumberOfPoints != GE1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "E2")
                    ) &&
                    (
                            (NumberOfPoints != GE2CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "E3")
                    ) &&
                    (
                            (NumberOfPoints != GE3CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "E5")
                    ) &&
                    (
                            (NumberOfPoints != GE5CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "F")
                    ) &&
                    (
                            (NumberOfPoints != GFCylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "G1")
                    ) &&
                    (
                            (NumberOfPoints != GG1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == "G2")
                    ) &&
                    (
                            (NumberOfPoints != GG2CylcePointsNumber)
                    )
            )
    ) {

        qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings!";

        return false;
    }

    Array_n = new Double2DArray(NumberOfPoints, 1);
    array_n = Array_n->arrayPointer();

    Array_Me_brutto = new Double2DArray(NumberOfPoints, 1);
    array_Me_brutto = Array_Me_brutto->arrayPointer();

    Array_Ne_brutto = new Double2DArray(NumberOfPoints, 1);
    array_Ne_brutto = Array_Ne_brutto->arrayPointer();

    Array_N_fan = new Double2DArray(NumberOfPoints, 1);
    array_N_fan = Array_N_fan->arrayPointer();

    Array_w = new Double2DArray(NumberOfPoints, 1);
    array_w = Array_w->arrayPointer();

    Array_t0 = new Double2DArray(NumberOfPoints, 1);
    array_t0 = Array_t0->arrayPointer();

    Array_B0 = new Double2DArray(NumberOfPoints, 1);
    array_B0 = Array_B0->arrayPointer();

    Array_Ra = new Double2DArray(NumberOfPoints, 1);
    array_Ra = Array_Ra->arrayPointer();

    Array_dPn = new Double2DArray(NumberOfPoints, 1);
    array_dPn = Array_dPn->arrayPointer();

    Array_Gair = new Double2DArray(NumberOfPoints, 1);
    array_Gair = Array_Gair->arrayPointer();

    Array_Gfuel = new Double2DArray(NumberOfPoints, 1);
    array_Gfuel = Array_Gfuel->arrayPointer();

    Array_CNOx = new Double2DArray(NumberOfPoints, 1);
    array_CNOx = Array_CNOx->arrayPointer();

    Array_gNOx = new Double2DArray(NumberOfPoints, 1);
    array_gNOx = Array_gNOx->arrayPointer();

    Array_CCO = new Double2DArray(NumberOfPoints, 1);
    array_CCO = Array_CCO->arrayPointer();

    Array_CCH = new Double2DArray(NumberOfPoints, 1);
    array_CCH = Array_CCH->arrayPointer();

    Array_CCO2in = new Double2DArray(NumberOfPoints, 1);
    array_CCO2in = Array_CCO2in->arrayPointer();

    Array_CCO2out = new Double2DArray(NumberOfPoints, 1);
    array_CCO2out = Array_CCO2out->arrayPointer();

    Array_CO2 = new Double2DArray(NumberOfPoints, 1);
    array_CO2 = Array_CO2->arrayPointer();

    Array_Ka1m = new Double2DArray(NumberOfPoints, 1);
    array_Ka1m = Array_Ka1m->arrayPointer();

    Array_KaPerc = new Double2DArray(NumberOfPoints, 1);
    array_KaPerc = Array_KaPerc->arrayPointer();

    Array_FSN = new Double2DArray(NumberOfPoints, 1);
    array_FSN = Array_FSN->arrayPointer();

    Array_Pr = new Double2DArray(NumberOfPoints, 1);
    array_Pr = Array_Pr->arrayPointer();

    Array_ts = new Double2DArray(NumberOfPoints, 1);
    array_ts = Array_ts->arrayPointer();

    Array_tauf = new Double2DArray(NumberOfPoints, 1);
    array_tauf = Array_tauf->arrayPointer();

    Array_qmdw = new Double2DArray(NumberOfPoints, 1);
    array_qmdw = Array_qmdw->arrayPointer();

    Array_qmdew = new Double2DArray(NumberOfPoints, 1);
    array_qmdew = Array_qmdew->arrayPointer();

    Array_rd = new Double2DArray(NumberOfPoints, 1);
    array_rd = Array_rd->arrayPointer();

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        array_n        [i][0] = array_DataForCalc[i+1][ 1];
        array_Me_brutto[i][0] = array_DataForCalc[i+1][ 2];
        array_Ne_brutto[i][0] = array_DataForCalc[i+1][ 3];
        array_N_fan    [i][0] = array_DataForCalc[i+1][ 4];
        array_w        [i][0] = array_DataForCalc[i+1][ 5];
        array_t0       [i][0] = array_DataForCalc[i+1][ 6];
        array_B0       [i][0] = array_DataForCalc[i+1][ 7];
        array_Ra       [i][0] = array_DataForCalc[i+1][ 8];
        array_dPn      [i][0] = array_DataForCalc[i+1][ 9];
        array_Gair     [i][0] = array_DataForCalc[i+1][10];
        array_Gfuel    [i][0] = array_DataForCalc[i+1][11];
        array_CNOx     [i][0] = array_DataForCalc[i+1][12];
        array_gNOx     [i][0] = array_DataForCalc[i+1][13];
        array_CCO      [i][0] = array_DataForCalc[i+1][14];
        array_CCH      [i][0] = array_DataForCalc[i+1][15];
        array_CCO2in   [i][0] = array_DataForCalc[i+1][16];
        array_CCO2out  [i][0] = array_DataForCalc[i+1][17];
        array_CO2      [i][0] = array_DataForCalc[i+1][18];
        array_Ka1m     [i][0] = array_DataForCalc[i+1][19];
        array_KaPerc   [i][0] = array_DataForCalc[i+1][20];
        array_FSN      [i][0] = array_DataForCalc[i+1][21];
        array_Pr       [i][0] = array_DataForCalc[i+1][22];
        array_ts       [i][0] = array_DataForCalc[i+1][23];
        array_tauf     [i][0] = array_DataForCalc[i+1][24];
        array_qmdw     [i][0] = array_DataForCalc[i+1][25];
        array_qmdew    [i][0] = array_DataForCalc[i+1][26];
        array_rd       [i][0] = array_DataForCalc[i+1][27];
    }

    mytime = dateTimeNow();

    GetDataFromCSV_OK = true;

    //

    if (params->val_Standard() != "FreeCalc") {

        if (!nonZeroArray(array_n, &NumberOfPoints)) {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (n)!";
            return false;
        }

        if (nonZeroArray(array_Me_brutto, &NumberOfPoints)) {

            NenCalcMethod = true;
        }
        else if (nonZeroArray(array_Ne_brutto, &NumberOfPoints)) {

            NenCalcMethod = false;
        }
        else {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (Me_b or Ne_b)!";
            return false;
        }

        if (!nonZeroArray(array_t0, &NumberOfPoints)) {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (t0)!";
            return false;
        }

        if (!nonZeroArray(array_B0, &NumberOfPoints)) {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (B0)!";
            return false;
        }

        if (!nonZeroArray(array_Ra, &NumberOfPoints)) {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (Ra)!";
            return false;
        }

        if (nonZeroArray(array_Gair, &NumberOfPoints)) {

            GairVals = true;
        }
        else if (nonZeroArray(array_dPn, &NumberOfPoints)) {

            GairVals = false;
        }
        else {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (Gair or dPn)!";
            return false;
        }

        if (!nonZeroArray(array_Gfuel, &NumberOfPoints)) {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (Gfuel)!";
            return false;
        }

        if (nonZeroArray(array_CNOx, &NumberOfPoints)) {

            NOxCalcMethod = true;
        }
        else if (nonZeroArray(array_gNOx, &NumberOfPoints)) {

            NOxCalcMethod = false;
        }
        else {

            qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings (CNOx or gNOx)!";
            return false;
        }

        if (nonZeroArray(array_CCO, &NumberOfPoints)) {

            gCOcalc = true;
        }
        else {

            gCOcalc = false;
        }

        if (nonZeroArray(array_CCH, &NumberOfPoints)) {

            gCHcalc = true;
        }
        else {

            gCHcalc = false;
        }

        if ( nonZeroArray(array_CCO2in, &NumberOfPoints) && nonZeroArray(array_CCO2out, &NumberOfPoints) ) {

            EGRcalc = true;
        }
        else {

            EGRcalc = false;
        }

        if (nonZeroArray(array_CO2, &NumberOfPoints)) {

            CheckMeas = true;
        }
        else {

            CheckMeas = false;
        }

        if (params->val_PTcalc() != "no") {

            if ( !nonZeroArray(array_Pr, &NumberOfPoints) || !nonZeroArray(array_ts, &NumberOfPoints) ||
                 ( !nonZeroArray(array_Ka1m, &NumberOfPoints) &&
                   !nonZeroArray(array_KaPerc, &NumberOfPoints) &&
                   !nonZeroArray(array_FSN, &NumberOfPoints) ) ) {

                qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings!";
                return false;
            }

            if (params->val_PTcalc() == "ThroughPTmass") {

                if ( !nonZeroArray(array_tauf, &NumberOfPoints) || !nonZeroArray(array_qmdew, &NumberOfPoints) ||
                     ( !nonZeroArray(array_qmdw, &NumberOfPoints) &&
                       !nonZeroArray(array_rd, &NumberOfPoints) ) ) {

                    qDebug() << "libtoxic ERROR: CycleEmissions: readCSV: Bad source data or calculation settings!";
                    return false;
                }

                if (nonZeroArray(array_qmdw, &NumberOfPoints)) {

                    qmdwVSrd = true;
                }
                else if (nonZeroArray(array_rd, &NumberOfPoints)) {

                    qmdwVSrd = false;
                }
            }
        }

        if (nonZeroArray(array_FSN, &NumberOfPoints)) {

            smoke = 2;
        }
        else if (nonZeroArray(array_Ka1m, &NumberOfPoints)) {

            smoke = 0;
        }
        else if (nonZeroArray(array_KaPerc, &NumberOfPoints)) {

            smoke = 1;
        }
        else {

            smoke = -1;
        }
    }

    //

    return true;
}

bool CycleEmissions::preCalculate() {

    PreCalculate_OK = false;

    Array_Ne_netto = new Double2DArray(NumberOfPoints, 1);
    array_Ne_netto = Array_Ne_netto->arrayPointer();

    Array_Me_netto = new Double2DArray(NumberOfPoints, 1);
    array_Me_netto = Array_Me_netto->arrayPointer();

    Array_alpha = new Double2DArray(NumberOfPoints, 1);
    array_alpha = Array_alpha->arrayPointer();

    Array_alpha_O2 = new Double2DArray(NumberOfPoints, 1);
    array_alpha_O2 = Array_alpha_O2->arrayPointer();

    Array_Gexh = new Double2DArray(NumberOfPoints, 1);
    array_Gexh = Array_Gexh->arrayPointer();

    Array_Gexhd = new Double2DArray(NumberOfPoints, 1);
    array_Gexhd = Array_Gexhd->arrayPointer();

    Array_Pb = new Double2DArray(NumberOfPoints, 1);
    array_Pb = Array_Pb->arrayPointer();

    Array_Pa = new Double2DArray(NumberOfPoints, 1);
    array_Pa = Array_Pa->arrayPointer();

    Array_Ha = new Double2DArray(NumberOfPoints, 1);
    array_Ha = Array_Ha->arrayPointer();

    Array_Gaird = new Double2DArray(NumberOfPoints, 1);
    array_Gaird = Array_Gaird->arrayPointer();

    Array_Kw2 = new Double2DArray(NumberOfPoints, 1);
    array_Kw2 = Array_Kw2->arrayPointer();

    Array_Ffh = new Double2DArray(NumberOfPoints, 1);
    array_Ffh = Array_Ffh->arrayPointer();

    Array_Kf = new Double2DArray(NumberOfPoints, 1);
    array_Kf = Array_Kf->arrayPointer();

    Array_Kwr = new Double2DArray(NumberOfPoints, 1);
    array_Kwr = Array_Kwr->arrayPointer();

    Array_Khd = new Double2DArray(NumberOfPoints, 1);
    array_Khd = Array_Khd->arrayPointer();

    QString std = params->val_Standard();

    double L0    = config->val_L0();
    double WH    = config->val_WH();
    double WO2   = config->val_WO2();
    double WN    = config->val_WN();
    double roAir = config->val_roAir();

    double Ffw = 0;
    double Ffd = 0;

    if ( (std == "C1") || (std == "D1") || (std == "D2") ||
         (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
         (std == "F") || (std == "G1") || (std == "G2") ) {

        QString FuelType = params->val_FuelType();

        if (FuelType == "diesel") {

            Ffw = 0.75;
            Ffd = -0.77;
        }
        else if (FuelType == "motor") {

            Ffw = 0.72;
            Ffd = -0.74;
        }
        else if (FuelType == "mazut") {

            Ffw = 0.69;
            Ffd = -0.71;
        }
        else {

            qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

            return false;
        }
    }

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        if (NenCalcMethod) {

            array_Ne_brutto[i][0] = array_Me_brutto[i][0] * array_n[i][0] / 9550.0;
        }
        else if (!NenCalcMethod) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        if ( (std == "C1") || (std == "D1") || (std == "D2") || (std == "E1") || (std == "E2") ||
             (std == "E3") || (std == "E5") || (std == "F") || (std == "G1") || (std == "G2") || (std == "OST") || (std == "GOST") ||
             (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
             (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
             (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
             (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

            array_Ne_netto[i][0] = array_Ne_brutto[i][0];
        }
        else {

            array_Ne_netto[i][0] = array_Ne_brutto[i][0] - array_N_fan[i][0];
        }

        array_Me_netto[i][0] = array_Ne_netto[i][0] * 9550.0 / array_n[i][0];

        if (!GairVals) {

            double Dn = config->val_Dn();

            if (Dn < 1) {

                qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

                return false;
            }

            array_Gair[i][0] = 0.0084591 * pow(Dn, 2) * sqrt((1.019716213 * array_dPn[i][0] * 7.500616827 * array_B0[i][0])/(array_t0[i][0] + 273.0));
        }

        array_alpha[i][0] = array_Gair[i][0] / array_Gfuel[i][0] / L0;

        if (CheckMeas) {

            double ConcO2air = config->val_ConcO2air();

            if (ConcO2air < 1) {

                qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

                return false;
            }

            if (!EGRcalc) {

                array_alpha_O2[i][0] = (0.01 * ConcO2air * (1.0 - 0.01 * array_CO2[i][0]) + 0.273274 * 0.01 * array_CO2[i][0]) /
                                       (0.01 * ConcO2air - 0.01 * array_CO2[i][0]);
            }
        }

        if ( (std == "C1") || (std == "D1") || (std == "D2") ||
             (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
             (std == "F") || (std == "G1") || (std == "G2") ) {

            array_Gexh[i][0] = array_Gair[i][0] / roAir + Ffw * array_Gfuel[i][0];
            array_Gexhd[i][0] = array_Gair[i][0] / roAir + Ffd * array_Gfuel[i][0];
        }
        else {

            array_Gexh[i][0] = array_Gair[i][0] + array_Gfuel[i][0];
        }

        if ( (std != "C1") && (std != "D1") && (std != "D2") &&
             (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
             (std != "F") && (std != "G1") && (std != "G2") ) {

            array_Pb[i][0] = array_B0[i][0];
            array_Pa[i][0] = calcPa(&(array_t0[i][0]));
            array_Ha[i][0] = 6.22 * array_Ra[i][0] * array_Pa[i][0] / (array_Pb[i][0] - array_Pa[i][0] * array_Ra[i][0] * 0.01);

            if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") || (std == "FreeCalc") ||
                 (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
                 (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
                 (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
                 (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

                array_Gaird[i][0] = (1.0 - array_Ha[i][0] / 1000.0) * array_Gair[i][0];
            }
            else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") || (std == "OST") || (std == "GOST") ) {

                array_Gaird[i][0] = array_Gair[i][0] / (1.0 + array_Ha[i][0] / 1000.0);
            }
            else {

                qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

                return false;
            }

            if (std != "OST") {

                array_Kw2[i][0] = 1.608 * array_Ha[i][0] / (1000.0 + 1.608 * array_Ha[i][0]);
                array_Ffh[i][0] = 1.969 / (1.0 + array_Gfuel[i][0] / array_Gair[i][0]);
                array_Kf[i][0] = 0.055594 * WH + 0.0080021 * WN + 0.0070046 * WO2;
            }

            if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") || (std == "FreeCalc") ) {

                array_Kwr[i][0] = (1.0 - (1.2442 * array_Ha[i][0] + 111.19 * WH * array_Gfuel[i][0] / array_Gaird[i][0]) /
                                       (773.4 + 1.2442 * array_Ha[i][0] + array_Gfuel[i][0] / array_Gaird[i][0] * array_Kf[i][0] * 1000.0)) * 1.008;
                array_Khd[i][0] = 1.0 / (1.0 - 0.0182 * (array_Ha[i][0] - 10.71) + 0.0045 * (array_t0[i][0] + 273.0 - 298.0));
            }
            else if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
                      (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
                      (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
                      (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

                array_Kwr[i][0] = (1.0 - array_Ffh[i][0] * array_Gfuel[i][0] / array_Gaird[i][0]) - array_Kw2[i][0];
                array_Khd[i][0] = 1.0 / (1.0 + (0.309 * array_Gfuel[i][0] / array_Gaird[i][0] - 0.0266) * (array_Ha[i][0] - 10.71) +
                                      (-0.209 * array_Gfuel[i][0] / array_Gaird[i][0] + 0.00954) * (array_t0[i][0] + 273.0 - 298.0));
            }
            else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") ) {

                array_Kwr[i][0] = 1.0 - 1.85 * array_Gfuel[i][0] / array_Gaird[i][0];
                array_Khd[i][0] = 1.0 / (1.0 + (0.044 * array_Gfuel[i][0] / array_Gaird[i][0] - 0.0038) * (7.0 * array_Ha[i][0] - 75.0) +
                                           (-0.116 * array_Gfuel[i][0] / array_Gaird[i][0] + 0.0053) * 1.8 * (array_t0[i][0] + 273.0 - 302.0));
            }
            else if (std == "OST") {

                array_Kwr[i][0] = 1.0 - 1.8 * array_Gfuel[i][0] / array_Gair[i][0];
            }
            else if (std == "GOST") {

                array_Kwr[i][0] = 1.0 - 1.85 * array_Gfuel[i][0] / array_Gair[i][0];
                array_Khd[i][0] = 1.0 / (1.0 + (0.044 * array_Gfuel[i][0] / array_Gair[i][0] - 0.0038) * (7.0 * array_Ha[i][0] - 75.0) +
                                           (-0.116 * array_Gfuel[i][0] / array_Gair[i][0] + 0.0053) * 1.8 * (array_t0[i][0] + 273.0 - 302.0));
            }
            else {

                qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

                return false;
            }
        }
    }

    PreCalculate_OK = true;

    return true;
}

bool CycleEmissions::calculate_gNOx() {

    MakeCalculation_gNOx_OK = false;

    Array_mNOx = new Double2DArray(NumberOfPoints, 1);
    array_mNOx = Array_mNOx->arrayPointer();

    double muNO2 = config->val_muNO2();

    double summ_mNOx = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    QString std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == "C1") || (std == "D1") || (std == "D2") ||
         (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
         (std == "F") || (std == "G1") || (std == "G2") ) {

        if (params->val_NOxSample() == "dry") {

            for (ptrdiff_t i=0; i<n; i++) {

                summ_numerator += array_CNOx[i][0] / 10000.0 * array_Gexhd[i][0] * array_w[i][0];
                summ_denominator += array_Ne_brutto[i][0] / array_Ne_brutto[0][0] * array_w[i][0];
            }
        }
        else {

            for (ptrdiff_t i=0; i<n; i++) {

                summ_numerator += array_CNOx[i][0] / 10000.0 * array_Gexh[i][0] * array_w[i][0];
                summ_denominator += array_Ne_brutto[i][0] / array_Ne_brutto[0][0] * array_w[i][0];
            }
        }

        gNOx = 0.446 * muNO2 * summ_numerator / (array_Ne_brutto[0][0] * summ_denominator);
    }
    else {

        if (NOxCalcMethod) {

            if (params->val_NOxSample() == "dry") {

                for (ptrdiff_t i=0; i<n; i++) {

                    if (std == "OST") {

                        array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Kwr[i][0] * array_Gexh[i][0];
                    }
                    else {

                        array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Kwr[i][0] * array_Khd[i][0] * array_Gexh[i][0];
                    }

                    array_gNOx[i][0] = array_mNOx[i][0] / array_Ne_netto[i][0];

                    summ_mNOx += array_mNOx[i][0] * array_w[i][0];
                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }
            else {

                for (ptrdiff_t i=0; i<n; i++) {

                    if (std == "OST") {

                        array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Gexh[i][0];
                    }
                    else {

                        array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Khd[i][0] * array_Gexh[i][0];
                    }

                    array_gNOx[i][0] = array_mNOx[i][0] / array_Ne_netto[i][0];

                    summ_mNOx += array_mNOx[i][0] * array_w[i][0];
                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }
        }
        else if (!NOxCalcMethod) {

            if (params->val_NOxSample() == "dry") {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_mNOx[i][0] = array_gNOx[i][0] * array_Ne_netto[i][0];

                    if (std == "OST") {

                        array_CNOx[i][0] = array_mNOx[i][0] / (0.001587 * array_Kwr[i][0] * array_Gexh[i][0]);
                    }
                    else {

                        array_CNOx[i][0] = array_mNOx[i][0] / (0.001587 * array_Kwr[i][0] * array_Khd[i][0] * array_Gexh[i][0]);
                    }

                    summ_mNOx += array_mNOx[i][0] * array_w[i][0];
                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }
            else {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_mNOx[i][0] = array_gNOx[i][0] * array_Ne_netto[i][0];

                    if (std == "OST") {

                        array_CNOx[i][0] = array_mNOx[i][0] / (0.001587 * array_Gexh[i][0]);
                    }
                    else {

                        array_CNOx[i][0] = array_mNOx[i][0] / (0.001587 * array_Khd[i][0] * array_Gexh[i][0]);
                    }

                    summ_mNOx += array_mNOx[i][0] * array_w[i][0];
                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }
        }

        gNOx = summ_mNOx / summ_Ne_netto;
    }

    MakeCalculation_gNOx_OK = true;

    return true;
}

bool CycleEmissions::calculateAdditionalPoints() {

    /*
     *      Control Point (Z)
     *
     *   power
     *      |
     *      |      T
     * Ntu _|___________
     *      |                U
     *      |
     *      |           Z
     *      |
     *      |      R
     * Nrs _|___________
     *      |                S
     *      |
     *      |______|_________|______ speed
     *             |         |
     *            nrt       nus
     */

    QString std = params->val_Standard();

    for (ptrdiff_t i=(NumberOfPoints - TCycleAddPointsNumber); i<NumberOfPoints; i++) {

        if (params->val_NOxSample() == "dry") {

            if (std == "OST") {

                array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Kwr[i][0] * array_Gexh[i][0];
            }
            else {

                array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Kwr[i][0] * array_Khd[i][0] * array_Gexh[i][0];
            }
        }
        else {

            if (std == "OST") {

                array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Gexh[i][0];
            }
            else {

                array_mNOx[i][0] = 0.001587 * array_CNOx[i][0] * array_Khd[i][0] * array_Gexh[i][0];
            }
        }

        array_gNOx[i][0] = array_mNOx[i][0] / array_Ne_netto[i][0];
    }

    gNOx1m = array_gNOx[NumberOfPoints - 3][0];
    gNOx2m = array_gNOx[NumberOfPoints - 2][0];
    gNOx3m = array_gNOx[NumberOfPoints - 1][0];

    double A = array_n[1][0];
    double B = array_n[2][0];
    double C = array_n[9][0];

    double nz1 = array_n[NumberOfPoints - 3][0];
    double nz2 = array_n[NumberOfPoints - 2][0];
    double nz3 = array_n[NumberOfPoints - 1][0];

    double nrt1 = B;
    double nrt2 = A;
    double nrt3 = B;

    double nsu1 = C;
    double nsu2 = B;
    double nsu3 = C;

    double Nz1 = array_Ne_netto[NumberOfPoints - 3][0];
    double Nz2 = array_Ne_netto[NumberOfPoints - 2][0];
    double Nz3 = array_Ne_netto[NumberOfPoints - 1][0];

    double Nt1 = array_Ne_netto[7][0];
    double Nt2 = array_Ne_netto[5][0];
    double Nt3 = array_Ne_netto[2][0];

    double Nu1 = array_Ne_netto[ 9][0];
    double Nu2 = array_Ne_netto[ 3][0];
    double Nu3 = array_Ne_netto[12][0];

    double Nr1 = array_Ne_netto[3][0];
    double Nr2 = array_Ne_netto[4][0];
    double Nr3 = array_Ne_netto[8][0];

    double Ns1 = array_Ne_netto[11][0];
    double Ns2 = array_Ne_netto[ 2][0];
    double Ns3 = array_Ne_netto[10][0];

    double Ntu1 = Nt1 + (Nu1 - Nt1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double Ntu2 = Nt2 + (Nu2 - Nt2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double Ntu3 = Nt3 + (Nu3 - Nt3) * (nz3 - nrt3) / (nsu3 - nrt3);

    double Nrs1 = Nr1 + (Ns1 - Nr1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double Nrs2 = Nr2 + (Ns2 - Nr2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double Nrs3 = Nr3 + (Ns3 - Nr3) * (nz3 - nrt3) / (nsu3 - nrt3);

    double gNOt1 = array_gNOx[7][0];
    double gNOt2 = array_gNOx[5][0];
    double gNOt3 = array_gNOx[2][0];

    double gNOu1 = array_gNOx[ 9][0];
    double gNOu2 = array_gNOx[ 3][0];
    double gNOu3 = array_gNOx[12][0];

    double gNOr1 = array_gNOx[3][0];
    double gNOr2 = array_gNOx[4][0];
    double gNOr3 = array_gNOx[8][0];

    double gNOs1 = array_gNOx[11][0];
    double gNOs2 = array_gNOx[ 2][0];
    double gNOs3 = array_gNOx[10][0];

    double gNOtu1 = gNOt1 + (gNOu1 - gNOt1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double gNOtu2 = gNOt2 + (gNOu2 - gNOt2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double gNOtu3 = gNOt3 + (gNOu3 - gNOt3) * (nz3 - nrt3) / (nsu3 - nrt3);

    double gNOrs1 = gNOr1 + (gNOs1 - gNOr1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double gNOrs2 = gNOr2 + (gNOs2 - gNOr2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double gNOrs3 = gNOr3 + (gNOs3 - gNOr3) * (nz3 - nrt3) / (nsu3 - nrt3);

    gNOx1c = gNOrs1 + (gNOtu1 - gNOrs1) * (Nz1 - Nrs1) / (Ntu1 - Nrs1);
    gNOx2c = gNOrs2 + (gNOtu2 - gNOrs2) * (Nz2 - Nrs2) / (Ntu2 - Nrs2);
    gNOx3c = gNOrs3 + (gNOtu3 - gNOrs3) * (Nz3 - Nrs3) / (Ntu3 - Nrs3);

    diffNOx1 = (gNOx1m - gNOx1c) / gNOx1c * 100.0;
    diffNOx2 = (gNOx2m - gNOx2c) / gNOx2c * 100.0;
    diffNOx3 = (gNOx3m - gNOx3c) / gNOx3c * 100.0;

    return true;
}

bool CycleEmissions::calculate_gCO() {

    MakeCalculation_gCO_OK = false;

    Array_mCO = new Double2DArray(NumberOfPoints, 1);
    array_mCO = Array_mCO->arrayPointer();

    Array_gCO = new Double2DArray(NumberOfPoints, 1);
    array_gCO = Array_gCO->arrayPointer();

    double muCO = config->val_muCO();

    double summ_mCO = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    QString std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == "C1") || (std == "D1") || (std == "D2") ||
         (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
         (std == "F") || (std == "G1") || (std == "G2") ) {

        for (ptrdiff_t i=0; i<n; i++) {

            summ_numerator += array_CCO[i][0] / 10000.0 * array_Gexhd[i][0] * array_w[i][0];
            summ_denominator += array_Ne_brutto[i][0] / array_Ne_brutto[0][0] * array_w[i][0];
        }

        gCO = 0.446 * muCO * summ_numerator / (array_Ne_brutto[0][0] * summ_denominator);
    }
    else {

        for (ptrdiff_t i=0; i<n; i++) {

            array_mCO[i][0] = 0.000966 * array_CCO[i][0] * array_Kwr[i][0] * array_Gexh[i][0]; // always is dry
            array_gCO[i][0] = array_mCO[i][0] / array_Ne_netto[i][0];

            summ_mCO += array_mCO[i][0] * array_w[i][0];
            summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
        }

        gCO = summ_mCO / summ_Ne_netto;
    }

    MakeCalculation_gCO_OK = true;

    return true;
}

bool CycleEmissions::calculate_gCH() {

    MakeCalculation_gCH_OK = false;

    Array_mCH = new Double2DArray(NumberOfPoints, 1);
    array_mCH = Array_mCH->arrayPointer();

    Array_gCH = new Double2DArray(NumberOfPoints, 1);
    array_gCH = Array_gCH->arrayPointer();

    double muCH = config->val_muCH();

    double summ_mCH = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    QString std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == "C1") || (std == "D1") || (std == "D2") ||
         (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
         (std == "F") || (std == "G1") || (std == "G2") ) {

        for (ptrdiff_t i=0; i<n; i++) {

            summ_numerator += array_CCH[i][0] / 10000.0 * array_Gexh[i][0] * array_w[i][0];
            summ_denominator += array_Ne_brutto[i][0] / array_Ne_brutto[0][0] * array_w[i][0];
        }

        gCH = 0.446 * muCH * summ_numerator / (array_Ne_brutto[0][0] * summ_denominator);
    }
    else {

        for (ptrdiff_t i=0; i<n; i++) {

            if (std == "OST") {

                array_mCH[i][0] = 0.000485 * array_CCH[i][0] * array_Gexh[i][0]; // always is wet
            }
            else {

                array_mCH[i][0] = 0.000479 * array_CCH[i][0] * array_Gexh[i][0]; // always is wet
            }

            array_gCH[i][0] = array_mCH[i][0] / array_Ne_netto[i][0];

            summ_mCH += array_mCH[i][0] * array_w[i][0];
            summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
        }

        gCH = summ_mCH / summ_Ne_netto;
    }

    MakeCalculation_gCH_OK = true;

    return true;
}

bool CycleEmissions::calculate_gPT() {

    MakeCalculation_gPT_OK = false;

    mf = params->val_PTmass();

    Array_ror = new Double2DArray(NumberOfPoints, 1);
    array_ror = Array_ror->arrayPointer();

    Array_CPT = new Double2DArray(NumberOfPoints, 1);
    array_CPT = Array_CPT->arrayPointer();

    Array_qmedf = new Double2DArray(NumberOfPoints, 1);
    array_qmedf = Array_qmedf->arrayPointer();

    Array_msepi = new Double2DArray(NumberOfPoints, 1);
    array_msepi = Array_msepi->arrayPointer();

    Array_mPT = new Double2DArray(NumberOfPoints, 1);
    array_mPT = Array_mPT->arrayPointer();

    Array_gPT = new Double2DArray(NumberOfPoints, 1);
    array_gPT = Array_gPT->arrayPointer();

    QString std = params->val_Standard();

    if ( (params->val_PTcalc() != "no") && ( (std != "OST") && (std != "GOST") && (std != "EU0") &&
                                            (std != "C1") && (std != "D1") && (std != "D2") &&
                                            (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                                            (std != "F") && (std != "G1") && (std != "G2") ) ) {

        ptrdiff_t n = 0;
        if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

            n = NumberOfPoints - TCycleAddPointsNumber;
        }
        else {

            n = NumberOfPoints;
        }

        double L = config->val_L();
        double Rr = config->val_Rr();

        double summ_mPT = 0;
        double summ_Ne_netto = 0;

        for (ptrdiff_t i=0; i<n; i++) {

            if (smoke == 0) {

                array_KaPerc[i][0] = 100.0 * (1.0 - exp(- array_Ka1m[i][0] * L));
                array_FSN[i][0] = (6.6527E-017)           * pow(array_KaPerc[i][0], 10) +
                                  (-0.000000000000026602) * pow(array_KaPerc[i][0],  9) +
                                  (0.0000000000040987)    * pow(array_KaPerc[i][0],  8) +
                                  (-0.00000000026927)     * pow(array_KaPerc[i][0],  7) +
                                  (0.00000000040933)      * pow(array_KaPerc[i][0],  6) +
                                  (0.0000010658)          * pow(array_KaPerc[i][0],  5) +
                                  (-0.000069165)          * pow(array_KaPerc[i][0],  4) +
                                  (0.0020088)             * pow(array_KaPerc[i][0],  3) +
                                  (-0.028758)             * pow(array_KaPerc[i][0],  2) +
                                  (0.26502)               * pow(array_KaPerc[i][0],  1) +
                                  (0.0087517)             * pow(array_KaPerc[i][0],  0);
            }
            else if (smoke == 1) {

                array_FSN[i][0] = (6.6527E-017)           * pow(array_KaPerc[i][0], 10) +
                                  (-0.000000000000026602) * pow(array_KaPerc[i][0],  9) +
                                  (0.0000000000040987)    * pow(array_KaPerc[i][0],  8) +
                                  (-0.00000000026927)     * pow(array_KaPerc[i][0],  7) +
                                  (0.00000000040933)      * pow(array_KaPerc[i][0],  6) +
                                  (0.0000010658)          * pow(array_KaPerc[i][0],  5) +
                                  (-0.000069165)          * pow(array_KaPerc[i][0],  4) +
                                  (0.0020088)             * pow(array_KaPerc[i][0],  3) +
                                  (-0.028758)             * pow(array_KaPerc[i][0],  2) +
                                  (0.26502)               * pow(array_KaPerc[i][0],  1) +
                                  (0.0087517)             * pow(array_KaPerc[i][0],  0);
            }

            array_ror[i][0] = (array_Pb[i][0] + array_Pr[i][0]) * 1000.0 / Rr / (array_ts[i][0] + 273.0);
            array_CPT[i][0] = (-184.0 * array_FSN[i][0] - 727.5) * log10(1.0 - array_FSN[i][0] / 10.0);
            array_mPT[i][0] = array_CPT[i][0] * array_Gexh[i][0] / array_ror[i][0] / 1000.0;
            array_gPT[i][0] = array_mPT[i][0] / array_Ne_netto[i][0];

            summ_mPT += array_mPT[i][0] * array_w[i][0];
            summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
        }

        gPTs = summ_mPT / summ_Ne_netto;

        summ_mPT = 0;
        summ_Ne_netto = 0;

        if (params->val_PTcalc() == "ThroughPTmass") {

            if (mf == 0) {

                cout << "Enter mass of 1st clean filter, mg: "; cin >> mf1c;
                cout << "Enter mass of 1st dirty filter, mg: "; cin >> mf1d;
                cout << "Enter mass of 2nd clean filter, mg: "; cin >> mf2c;
                cout << "Enter mass of 2nd dirty filter, mg: "; cin >> mf2d;

                mf = (mf1d - mf1c) + (mf2d - mf2c);
            }

            if (qmdwVSrd) {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_rd[i][0] = array_qmdew[i][0] / (array_qmdew[i][0] - array_qmdw[i][0]);
                    array_qmedf[i][0] = array_Gexh[i][0] * array_rd[i][0];
                    qmedfl += array_qmedf[i][0] * array_w[i][0];
                    array_msepi[i][0] = array_qmdew[i][0] * array_tauf[i][0];
                    msep += array_msepi[i][0];

                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }
            else if (!qmdwVSrd) {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_qmdw[i][0] = (array_qmdew[i][0] * array_rd[i][0] - array_qmdew[i][0]) / array_rd[i][0];
                    array_qmedf[i][0] = array_Gexh[i][0] * array_rd[i][0];
                    qmedfl += array_qmedf[i][0] * array_w[i][0];
                    array_msepi[i][0] = array_qmdew[i][0] * array_tauf[i][0];
                    msep += array_msepi[i][0];

                    summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];
                }
            }

            mPT = mf * qmedfl / msep;
            gPT = mPT / summ_Ne_netto;
        }
    }

    MakeCalculation_gPT_OK = true;

    return true;
}

bool CycleEmissions::calculate_rEGR() {

    MakeCalculation_rEGR_OK = false;

    Array_rEGR = new Double2DArray(NumberOfPoints, 1);
    array_rEGR = Array_rEGR->arrayPointer();

    Array_alpha_res = new Double2DArray(NumberOfPoints, 1);
    array_alpha_res = Array_alpha_res->arrayPointer();

    double CCO2air = config->val_ConcCO2air();

    if (EGRcalc) {

        for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

            array_rEGR[i][0] = (array_CCO2in[i][0] - CCO2air) / (array_CCO2out[i][0] - CCO2air) * 100.0;
            array_alpha_res[i][0] = (array_alpha[i][0] - array_alpha[i][0] * array_rEGR[i][0] / 100.0 - array_rEGR[i][0] / 100.0) / (1.0 - 2.0 * array_rEGR[i][0] / 100.0);
        }

    }

    MakeCalculation_rEGR_OK = true;

    return true;
}

bool CycleEmissions::calculate_Means() {

    double summ_Gfuel    = 0;
    double summ_Ne_netto = 0;

    double summ_t0 = 0;
    double summ_B0 = 0;
    double summ_Ra = 0;

    QString std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    for (ptrdiff_t i=0; i<n; i++) {

        summ_Gfuel    += array_Gfuel[i][0] * array_w[i][0];
        summ_Ne_netto += array_Ne_netto[i][0] * array_w[i][0];

        summ_t0 += array_t0[i][0];
        summ_B0 += array_B0[i][0];
        summ_Ra += array_Ra[i][0];
    }

    geMean = summ_Gfuel / summ_Ne_netto * 1000.0;

    t0Mean = summ_t0 / n;
    B0Mean = summ_B0 / n;
    RaMean = summ_Ra / n;

    return true;
}

bool CycleEmissions::compareAlpha() {

    CompareAlpha_OK = false;

    Array_diff_alpha = new Double2DArray(NumberOfPoints, 1);
    array_diff_alpha = Array_diff_alpha->arrayPointer();

    if (CheckMeas) {

        if (EGRcalc) {

            double ConcO2mix = 0;
            double ConcO2air = config->val_ConcO2air();

            if (ConcO2air < 1) {

                qDebug() << "libtoxic ERROR: CycleEmissions: preCalculate: Bad source data or calculation settings!";

                return false;
            }

            for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

                ConcO2mix = ConcO2air * (1.0 - array_rEGR[i][0] / 100.0) + array_CO2[i][0] * array_rEGR[i][0] / 100.0;
                array_alpha_O2[i][0] = (0.01 * ConcO2mix * (1.0 - 0.01 * array_CO2[i][0]) + 0.273274 * 0.01 * array_CO2[i][0]) /
                                       (0.01 * ConcO2mix - 0.01 * array_CO2[i][0]);
                array_diff_alpha[i][0] = (array_alpha_res[i][0] - array_alpha_O2[i][0]) / array_alpha_O2[i][0] * 100.0;
            }
        }
        else {

            for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

                array_diff_alpha[i][0] = (array_alpha[i][0] - array_alpha_O2[i][0]) / array_alpha_O2[i][0] * 100.0;
            }
        }

    }

    CompareAlpha_OK = true;

    return true;
}

QString CycleEmissions::createReports(bool createrepdir) {

    QString message = "";

    if (!createrepdir) {

        message += "\ngNOx = " + QString::number(gNOx) + " g/kWh\n";
        qDebug() << "\ngNOx =" << gNOx << "g/kWh";

        if (gCOcalc) {

            message += "gCO = " + QString::number(gCO) + " g/kWh\n";
            qDebug() << "gCO =" << gCO << "g/kWh";
        }

        if (gCHcalc) {

            message += "gCH = " + QString::number(gCH) + " g/kWh\n";
            qDebug() << "gCH =" << gCH << "g/kWh";
        }

        QString ptcalc = params->val_PTcalc();

        if (ptcalc != "no") {

            if (ptcalc == "ThroughPTmass") {

                message += "gPT = " + QString::number(gPT) + " g/kWh\n";
                qDebug() << "gPT =" << gPT << "g/kWh";
            }

            message += "gPTs = " + QString::number(gPTs) + " g/kWh\n";
            qDebug() << "gPTs =" << gPTs << "g/kWh";
        }

        //

        return message;
    }

    QString std = params->val_Standard();

    QString dirnameReports = config->val_dirnameReports();
    string csvdelimiter = (config->val_csvDelimiter()).toStdString();

    fullReportsPath = dirnameReports + "/" + std + "_" + QString::fromStdString(mytime);
    QDir reportdir;
    reportdir.mkdir(fullReportsPath);

    //

    string CheckoutDataFileName;
    CheckoutDataFileName = "CheckoutData_" + mytime + ".csv";

    QString checkoutdata = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(CheckoutDataFileName);
    ofstream fout1(checkoutdata.toAscii());

    if (!fout1) {

        message += "libtoxic ERROR: CycleEmissions: createReports: fout1 was not created!\n";
        qDebug() << "\nlibtoxic ERROR: CycleEmissions: createReports: fout1 was not created!";

        return message;
    }

    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Point[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "n[min-1]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Me_b[Nm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_b[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "N_fan[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "w[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "t0[oC]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "B0[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ra[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "dPn[mmH2O]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gair[kg/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gfuel[kg/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CNOx[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "gNOx[g/kWh]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CCO[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CCH[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CCO2in[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CCO2out[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CO2[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[m-1]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "FSN[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Pr[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "ts[oC]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "tauf[s]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "qmdw[g/s]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "qmdew[g/s]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "rd[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_n[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Me_n[Nm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "alpha[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "alpha_O2[-]" << csvdelimiter;

    if ( (std == "C1") || (std == "D1") || (std == "D2") ||
         (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
         (std == "F") || (std == "G1") || (std == "G2") ) {

        fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gexh[m3/h]" << csvdelimiter;
        fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gexhd[m3/h]" << csvdelimiter;
    }
    else {

        fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gexh[kg/h]" << csvdelimiter;
        fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gexhd[kg/h]" << csvdelimiter;
    }

    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Pb[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Pa[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ha[g/kg]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gaird[kg/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Kw2[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ffh[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Kf[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Kwr[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Khd[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "mNOx[g/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "mCO[g/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "gCO[g/kWh]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "mCH[g/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "gCH[g/kWh]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "ror[kg/m3]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "CPT[mg/m3]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "mPT[g/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "gPT[g/kWh]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "qmedf[kg/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "msepi[g]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "rEGR[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "alpha_res[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "diff_alpha[%]" << csvdelimiter << endl;

    ptrdiff_t prec = Precision + 1;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(0) << (i + 1) << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(0) << array_n[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Me_brutto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ne_brutto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_N_fan[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_w[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_t0[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_B0[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ra[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_dPn[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gair[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gfuel[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CNOx[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gNOx[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCH[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO2in[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO2out[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CO2[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ka1m[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_KaPerc[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_FSN[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pr[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_ts[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_tauf[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmdw[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmdew[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_rd[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ne_netto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Me_netto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha_O2[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gexh[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gexhd[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pb[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pa[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ha[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gaird[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kw2[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ffh[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kf[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kwr[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Khd[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mNOx[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mCO[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gCO[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mCH[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gCH[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_ror[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CPT[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mPT[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gPT[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmedf[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_msepi[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_rEGR[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha_res[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_diff_alpha[i][0] << csvdelimiter << endl;
    }

    fout1.close();

    message += "libtoxic: Additional file \"" + QString::fromStdString(CheckoutDataFileName) + "\" rewrited.\n";
    qDebug() << "\nlibtoxic: Additional file" << QString::fromStdString(CheckoutDataFileName) << "rewrited.";

    //

    string SourceDataFileName;

    if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
         (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
         (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
         (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

        SourceDataFileName = "SourceData96_" + mytime + ".csv";
    }
    else if (std == "OST") {

        SourceDataFileName = "SourceDataOST_" + mytime + ".csv";
    }
    else if (std == "GOST") {

        SourceDataFileName = "SourceDataGOST_" + mytime + ".csv";
    }
    else if ( (std == "C1") || (std == "D1") || (std == "D2") ||
              (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
              (std == "F") || (std == "G1") || (std == "G2") ) {

        SourceDataFileName = "SourceDataGOST51249_" + mytime + ".csv";
    }
    else {

        SourceDataFileName = "SourceData49_" + mytime + ".csv";
    }

    QString srcdata = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(SourceDataFileName);
    ofstream fout4(srcdata.toAscii());

    if (!fout4) {

        message += "libtoxic ERROR: CycleEmissions: createReports: fout4 was not created!\n";
        qDebug() << "libtoxic ERROR: CycleEmissions: createReports: fout4 was not created!";

        return message;
    }

    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Point[-]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "n[min-1]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Me_b[Nm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_b[kW]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "N_fan[kW]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "w[-]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "t0[oC]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "B0[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ra[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "dPn[mmH2O]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Gair[kg/h]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Gfuel[kg/h]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CNOx[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "gNOx[g/kWh]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CCO[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CCH[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CCO2in[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CCO2out[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "CO2[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[m-1]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "FSN[-]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Pr[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "ts[oC]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "tauf[s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "qmdw[g/s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "qmdew[g/s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "rd[-]" << csvdelimiter << endl;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        for (ptrdiff_t j=0; j<PointsFileColumnsNumber; j++) {

            fout4 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+1) << array_DataForCalc[i+1][j] << csvdelimiter;
        }

        fout4 << endl;
    }

    fout4.close();

    message += "libtoxic: SourceData file \"" + QString::fromStdString(SourceDataFileName) + "\" created.\n";
    qDebug() << "libtoxic: SourceData file" << QString::fromStdString(SourceDataFileName) << "created.";

    //

    if (std != "FreeCalc") {

        if ( (params->val_PTcalc() == "ThroughPTmass") && ( (std != "OST") && (std != "GOST") && (std != "EU0") &&
                                                           (std != "C1") && (std != "D1") && (std != "D2") &&
                                                           (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                                                           (std != "F") && (std != "G1") && (std != "G2") ) ) {

            string ReportFileNamePT;

            if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
                 (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
                 (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
                 (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

                ReportFileNamePT = "Report96_PT_" + mytime + ".txt";
            }
            else {

                ReportFileNamePT = "Report49_PT_" + mytime + ".txt";
            }

            QString reppt = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(ReportFileNamePT);
            ofstream fout6(reppt.toAscii());

            if (!fout6) {

                message += "libtoxic ERROR: CycleEmissions: createReports: fout6 was not created!\n";
                qDebug() << "libtoxic ERROR: CycleEmissions: createReports: fout6 was not created!";

                return message;
            }

            fout6 << "\tr49 distribution version " << r49version.toStdString();
            fout6 << "\t\tReport on cycle. " << std.toStdString() << "\t\tDateTime: " << mytime << endl << endl;

            fout6 << "Engine                 : ...\n";
            fout6 << fixed << setprecision(Precision) << "Environment parameters : t0_mean = "
                    << t0Mean <<  " oC; B0_mean = "
                    << B0Mean << " kPa; Ra_mean = "
                    << RaMean << " %\n";

            fout6 << "Calculation comments   : ";

            if (!GairVals) {

                fout6 << "Gair meas by nozzle (Dn = " << config->val_Dn() << "); ";
            }
            else {

                fout6 << "direct Gair meas; ";
            }

            if (params->val_NOxSample() == "dry") {

                fout6 << "NOxSample type is dry; ";
            }
            else {

                fout6 << "NOxSample type is wet; ";
            }

            if (params->val_PTcalc() == "ThroughPTmass") {

                fout6 << fixed << setprecision(Precision+1) << "PT calc method is PT mass based (mf = " << mf << " mg);\n";
            }
            else if (params->val_PTcalc() == "no") {

                fout6 << "PT was not calculated;\n";
            }
            else {

                fout6 << "PT calc method is smoke meas based;\n";
            }

            if (CheckMeas) {

                fout6 << fixed << setprecision(Precision) << "                         concentrations meas checked (conc O2air = " << config->val_ConcO2air() << " %)";
            }
            else {

                fout6 << "                         concentrations meas were not checked";
            }

            fout6 << "\n\n";

            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "n[min-1]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ne_n[kW]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gair[kg/h]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gfuel[kg/h]";

            if (EGRcalc) {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha_res[-]";
            }
            else {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha[-]";
            }

            if (smoke == 0) {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ka[m-1]";
            }
            else if (smoke == 1) {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ka[%]";
            }
            else if (smoke == 2) {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "FSN[-]";
            }

            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "tauf[s]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "qmdw[g/s]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "qmdew[g/s]" << endl;

            ptrdiff_t n = 0;
            if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

                n = NumberOfPoints - TCycleAddPointsNumber;
            }
            else {

                n = NumberOfPoints;
            }

            for (ptrdiff_t i=0; i<n; i++) {

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << array_n[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[i][0];

                if (EGRcalc) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha_res[i][0];
                }
                else {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[i][0];
                }

                if (smoke == 0) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_Ka1m[i][0];
                }
                else if (smoke == 1) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_KaPerc[i][0];
                }
                else if (smoke == 2) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_FSN[i][0];
                }

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_tauf[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_qmdw[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_qmdew[i][0] << endl;
            }

            fout6 << endl;

            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "rd[-]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "qmedf[kg/h]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "msepi[g]" << endl;

            for (ptrdiff_t i=0; i<n; i++) {

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_rd[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_qmedf[i][0];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_msepi[i][0] << endl;
            }

            fout6 << endl;

            fout6 << right << setw(WidthOfColumn-1+2) << setfill(' ') << " ";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limitation";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "CalcResult";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Conclusion\n";

            double gPTLimit = 0;
            gPTLimit = val_PTLimit(std);

            fout6 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gPT[g/kWh]";

            fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gPTLimit;
            fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gPT;

            if ( (gPT < gPTLimit) || (gPT == gPTLimit) ) {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK\n";
            }
            else {

                fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed\n";
            }

            fout6 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gPTs[g/kWh]";
            fout6 << fixed << right << setw(WidthOfColumn+WidthOfColumn-2) << setfill(' ') << setprecision(Precision+1) << gPTs << endl << endl;

            fout6.close();

            message += "libtoxic: Report file \"" + QString::fromStdString(ReportFileNamePT) + "\" created.\n";
            qDebug() << "libtoxic: Report file" << QString::fromStdString(ReportFileNamePT) << "created.";
        }

        //

        string ReportFileNameGAS;

        if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
             (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
             (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
             (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

            ReportFileNameGAS = "Report96_GAS_" + mytime + ".txt";
        }
        else if (std == "OST") {

            ReportFileNameGAS = "ReportOST_GAS_" + mytime + ".txt";
        }
        else if (std == "GOST") {

            ReportFileNameGAS = "ReportGOST_GAS_" + mytime + ".txt";
        }
        else if ( (std == "C1") || (std == "D1") || (std == "D2") ||
                  (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                  (std == "F") || (std == "G1") || (std == "G2") ) {

            ReportFileNameGAS = "ReportGOST51249_GAS_" + mytime + ".txt";
        }
        else {

            ReportFileNameGAS = "Report49_GAS_" + mytime + ".txt";
        }

        QString repgas = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(ReportFileNameGAS);
        ofstream fout5(repgas.toAscii());

        if (!fout5) {

            message += "libtoxic ERROR: CycleEmissions: createReports: fout5 was not created!\n";
            qDebug() << "libtoxic ERROR: CycleEmissions: createReports: fout5 was not created!";

            return message;
        }

        fout5 << "\tr49 distribution version " << r49version.toStdString();
        fout5 << "\t\tReport on cycle. " << std.toStdString() << "\t\tDateTime: " << mytime << endl << endl;

        fout5 << "Engine                 : ...\n";
        fout5 << fixed << setprecision(Precision) << "Environment parameters : t0_mean = "
                << t0Mean <<  " oC; B0_mean = "
                << B0Mean << " kPa; Ra_mean = "
                << RaMean << " %\n";

        fout5 << "Calculation comments   : ";

        if (!GairVals) {

            fout5 << "Gair meas by nozzle (Dn = " << config->val_Dn() << "); ";
        }
        else {

            fout5 << "direct Gair meas; ";
        }

        if (params->val_NOxSample() == "dry") {

            fout5 << "NOxSample type is dry; ";
        }
        else {

            fout5 << "NOxSample type is wet; ";
        }

        if (params->val_PTcalc() == "ThroughPTmass") {

            fout5 << fixed << setprecision(Precision+1) << "PT calc method is PT mass based (mf = " << mf << " mg);\n";
        }
        else if (params->val_PTcalc() == "no") {

            fout5 << "PT was not calculated;\n";
        }
        else {

            fout5 << "PT calc method is smoke meas based;\n";
        }

        if (CheckMeas) {

            fout5 << fixed << setprecision(Precision) << "                         concentrations meas checked (conc O2air = " << config->val_ConcO2air() << " %)";
        }
        else {

            fout5 << "                         concentrations meas were not checked";
        }

        fout5 << "\n\n";

        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "n[min-1]";
        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ne_n[kW]";
        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gair[kg/h]";
        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gfuel[kg/h]";

        if (EGRcalc) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha_res[-]";
        }
        else {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha[-]";
        }

        if (smoke == 0) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ka[m-1]";
        }
        else if (smoke == 1) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ka[%]";
        }
        else if (smoke == 2) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "FSN[-]";
        }

        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CNOx[ppm]";

        if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
             (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mNOx[g/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOx[g/kWh]";
        }

        fout5 << endl;

        ptrdiff_t n = 0;
        if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) && (params->val_AddPointsCalc() == "yes") ) {

            n = NumberOfPoints - TCycleAddPointsNumber;
        }
        else {

            n = NumberOfPoints;
        }

        for (ptrdiff_t i=0; i<n; i++) {

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << array_n[i][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[i][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[i][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[i][0];

            if (EGRcalc) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha_res[i][0];
            }
            else {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[i][0];
            }

            if (smoke == 0) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_Ka1m[i][0];
            }
            else if (smoke == 1) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_KaPerc[i][0];
            }
            else if (smoke == 2) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_FSN[i][0];
            }

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CNOx[i][0];

            if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                 (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mNOx[i][0];
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gNOx[i][0];
            }

            fout5 << endl;
        }

        fout5 << endl;

        if ( (!EGRcalc) && (!CheckMeas) ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CCO[ppm]";
            }

            if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                 (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                if (gCOcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCO[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gCO[g/kWh]";
                }
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CCH[ppm]";
            }

            if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                 (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                if (gCHcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCH[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gCH[g/kWh]";
                }
            }

            if (params->val_PTcalc() == "ThroughSmoke") {

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mPTs[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gPTs[g/kWh]";
                }
            }

            fout5 << endl;

            for (ptrdiff_t i=0; i<n; i++) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (i + 1);

                if (gCOcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCO[i][0];
                }

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    if (gCOcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCO[i][0];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gCO[i][0];
                    }
                }

                if (gCHcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCH[i][0];
                }

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    if (gCHcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCH[i][0];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gCH[i][0];
                    }
                }

                if (params->val_PTcalc() == "ThroughSmoke") {

                    if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                         (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mPT[i][0];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_gPT[i][0];
                    }
                }

                fout5 << endl;
            }
        }
        else {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CCO[ppm]";
            }

            if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                 (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                if (gCOcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCO[g/h]";
                }
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CCH[ppm]";
            }

            if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                 (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                if (gCHcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCH[g/h]";
                }
            }

            if (params->val_PTcalc() == "ThroughSmoke") {

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mPTs[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gPTs[g/kWh]";
                }
            }

            if (EGRcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "rEGR[%]";
            }

            if (CheckMeas) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "diff_alpha[%]";
            }

            fout5 << endl;

            for (ptrdiff_t i=0; i<n; i++) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (i + 1);

                if (gCOcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCO[i][0];
                }

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    if (gCOcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCO[i][0];
                    }
                }

                if (gCHcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCH[i][0];
                }

                if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                     (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                    if (gCHcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCH[i][0];
                    }
                }

                if (params->val_PTcalc() == "ThroughSmoke") {

                    if ( (std != "C1") && (std != "D1") && (std != "D2") && (std != "E1") && (std != "E2") &&
                         (std != "E3") && (std != "E5") && (std != "F") && (std != "G1") && (std != "G2") ) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mPT[i][0];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_gPT[i][0];
                    }
                }

                if (EGRcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_rEGR[i][0];
                }

                if (CheckMeas) {

                    fout5 << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_diff_alpha[i][0];
                }

                fout5 << endl;
            }
        }

        fout5 << endl;

        if ( ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) &&
             (params->val_AddPointsCalc() == "yes") ) {

            fout5 << "Additional points:\n\n";

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "n[min-1]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ne_n[kW]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gair[kg/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gfuel[kg/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha[-]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOm[g/kWh]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOc[g/kWh]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "diff[%]" << endl;

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber - 2);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 3][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 3][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 3][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 3][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 3][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx1m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx1c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx1 << endl;

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber - 1);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 2][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 2][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 2][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 2][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 2][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx2m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx2c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx2 << endl;

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 1][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 1][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 1][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 1][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 1][0];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx3m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx3c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx3 << endl;

            fout5 << endl;
        }

        fout5 << "Specific emissions:\n\n";

        fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << " ";

        if ( (std == "C1") || (std == "D1") || (std == "D2") ||
             (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
             (std == "F") || (std == "G1") || (std == "G2") ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new)";
        }
        else if (std == "GOST") {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old1)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old2)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new1)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new2)";
        }
        else {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limitation";
        }

        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CalcResult";

        if ( (std != "C1") && (std != "D1") && (std != "D2") &&
             (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
             (std != "F") && (std != "G1") && (std != "G2") && (std != "GOST") ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Conclusion";
        }

        fout5 << endl;

        double gNOxLimit   = 0;
        double gNOxLimit1  = 0;
        double gNOxLimit2  = 0;
        double gNOxLimit3  = 0;
        double gNOxLimit4  = 0;
        double gCOLimit    = 0;
        double gCOLimit1   = 0;
        double gCOLimit2   = 0;
        double gCOLimit3   = 0;
        double gCOLimit4   = 0;
        double gCHLimit    = 0;
        double gCHLimit1   = 0;
        double gCHLimit2   = 0;
        double gCHLimit3   = 0;
        double gCHLimit4   = 0;
        double gPTLimit    = 0;
        double gNOxCHLimit = 0;

        if ( (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
             (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

            gNOxCHLimit = val_NOxCHLimit(std);
            gCOLimit = val_COLimit(std);
            gPTLimit = val_PTLimit(std);

            fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gNOx+gCH[g/kWh]";
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxCHLimit;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOx+gCH;

            if ( ( (gNOx+gCH) < gNOxCHLimit) || ( (gNOx+gCH) == gNOxCHLimit) ) {

                fout5 << right << setw(WidthOfColumn) << setfill(' ') << "OK\n";
            }
            else {

                fout5 << right << setw(WidthOfColumn) << setfill(' ') << "failed\n";
            }

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gCO[g/kWh]";
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCO;

                if ( (gCO < gCOLimit) || (gCO == gCOLimit) ) {

                    fout5 << right << setw(WidthOfColumn) << setfill(' ') << "OK\n";
                }
                else {

                    fout5 << right << setw(WidthOfColumn) << setfill(' ') << "failed\n";
                }
            }
        }
        else {

            if ( (std == "C1") || (std == "D1") || (std == "D2") ||
                 (std == "F") || (std == "G1") || (std == "G2") ) {

                QString std1 = std + "old";

                gNOxLimit1 = val_NOxLimit(std1);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                QString std2 = std + "new";

                gNOxLimit2 = val_NOxLimit(std2);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);
            }
            else if ( (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ) {

                QString std1 = std + "old";

                gNOxLimit1 = val_NOxLimit(std1, array_n[0][0]);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                QString std2 = std + "new";

                gNOxLimit2 = val_NOxLimit(std2, array_n[0][0]);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);
            }
            else if (std == "GOST") {

                QString std1 = std + "ou";

                gNOxLimit1 = val_NOxLimit(std1);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                QString std2 = std + "ol";

                gNOxLimit2 = val_NOxLimit(std2);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);

                QString std3 = std + "nu";

                gNOxLimit3 = val_NOxLimit(std3);
                gCOLimit3 = val_COLimit(std3);
                gCHLimit3 = val_CHLimit(std3);

                QString std4 = std + "nl";

                gNOxLimit4 = val_NOxLimit(std4);
                gCOLimit4 = val_COLimit(std4);
                gCHLimit4 = val_CHLimit(std4);
            }
            else {

                gNOxLimit = val_NOxLimit(std);
                gCOLimit = val_COLimit(std);
                gCHLimit = val_CHLimit(std);
                gPTLimit = val_PTLimit(std);
            }

            fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gNOx[g/kWh]";

            if ( (std == "C1") || (std == "D1") || (std == "D2") ||
                 (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                 (std == "F") || (std == "G1") || (std == "G2") ) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit1;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit2;
            }
            else if (std == "GOST") {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit1;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit2;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit3;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit4;
            }
            else {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit;
            }

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOx;

            if ( (std != "GOST") && (std != "C1") && (std != "D1") && (std != "D2") &&
                 (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                 (std != "F") && (std != "G1") && (std != "G2") ) {

                if ( (gNOx < gNOxLimit) || (gNOx == gNOxLimit) ) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                }
                else {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                }
            }

            fout5 << endl;

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gCO[g/kWh]";

                if ( (std == "C1") || (std == "D1") || (std == "D2") ||
                     (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                     (std == "F") || (std == "G1") || (std == "G2") ) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit2;
                }
                else if (std == "GOST") {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit2;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit3;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit4;
                }
                else {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit;
                }

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCO;

                if ( (std != "GOST") && (std != "C1") && (std != "D1") && (std != "D2") &&
                     (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                     (std != "F") && (std != "G1") && (std != "G2") ) {

                    if ( (gCO < gCOLimit) || (gCO == gCOLimit) ) {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                    }
                    else {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                    }
                }

                fout5 << endl;
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gCH[g/kWh]";

                if ( (std == "C1") || (std == "D1") || (std == "D2") ||
                     (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                     (std == "F") || (std == "G1") || (std == "G2") ) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit2;
                }
                else if (std == "GOST") {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit2;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit3;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit4;
                }
                else {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit;
                }

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCH;

                if ( (std != "GOST") && (std != "C1") && (std != "D1") && (std != "D2") &&
                     (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                     (std != "F") && (std != "G1") && (std != "G2") ) {

                    if ( (gCH < gCHLimit) || (gCH == gCHLimit) ) {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                    }
                    else {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                    }
                }

                fout5 << endl;
            }
        }

        if ( (params->val_PTcalc() != "no") && ( (std != "GOST") && (std != "OST") && (std != "EU0") && (std != "C1") &&
                                                (std != "D1") && (std != "D2") &&
                                                (std != "E1") && (std != "E2") && (std != "E3") && (std != "E5") &&
                                                (std != "F") && (std != "G1") && (std != "G2") ) ) {

            if (params->val_PTcalc() == "ThroughPTmass") {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gPT[g/kWh]";

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gPTLimit;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gPT;

                if ( (gPT < gPTLimit) || (gPT == gPTLimit) ) {

                    fout5 << right << setw(WidthOfColumn) << setfill(' ') << "OK\n";
                }
                else {

                    fout5 << right << setw(WidthOfColumn) << setfill(' ') << "failed\n";
                }
            }

            fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gPTs[g/kWh]";
            fout5 << fixed << right << setw(WidthOfColumn+WidthOfColumn-2) << setfill(' ') << setprecision(Precision+1) << gPTs << endl;
        }

        fout5 << endl;

        fout5 << fixed << setprecision(Precision) << "Mean specific fuel consumption: " << geMean << " g/kWh\n";

        fout5.close();

        message += "libtoxic: Report file \"" + QString::fromStdString(ReportFileNameGAS) + "\" created.\n";
        qDebug() << "libtoxic: Report file" << QString::fromStdString(ReportFileNameGAS) << "created.";
    }

    //

    string CalcConfigFileName = mytime + ".conf";

    QString calcparam = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(CalcConfigFileName);
    ofstream fout7(calcparam.toAscii());

    if (!fout7) {

        message += "libtoxic ERROR: CycleEmissions: createReports: fout7 was not created!\n";
        qDebug() << "libtoxic ERROR: CycleEmissions: createReports: fout7 was not created!";

        return message;
    }

    string paramValDelim = parameterValueDelimiter.toStdString();

    fout7 << "task"           << paramValDelim << params->val_Task().toStdString()           << endl;
    fout7 << "Vh"             << paramValDelim << params->val_Vh()                           << endl;
    fout7 << "standard"       << paramValDelim << params->val_Standard().toStdString()       << endl;
    fout7 << "FuelType"       << paramValDelim << params->val_FuelType().toStdString()       << endl;
    fout7 << "NOxSample"      << paramValDelim << params->val_NOxSample().toStdString()      << endl;
    fout7 << "PTcalc"         << paramValDelim << params->val_PTcalc().toStdString()         << endl;
    fout7 << "PTmass"         << paramValDelim << params->val_PTmass()                       << endl;
    fout7 << "AddPointsCalc"  << paramValDelim << params->val_AddPointsCalc().toStdString()  << endl;
    fout7 << "CalcConfigFile" << paramValDelim << params->val_CalcConfigFile().toStdString() << endl;

    fout7.close();

    message += "libtoxic: Calculation config file \"" + QString::fromStdString(CalcConfigFileName) + "\" created.\n";
    qDebug() << "libtoxic: Calculation config file" << QString::fromStdString(CalcConfigFileName) << "created.";

    //

    if (std != "FreeCalc") {

        message += "\ngNOx = " + QString::number(gNOx) + " g/kWh\n";
        qDebug() << "\ngNOx =" << gNOx << "g/kWh";

        if (gCOcalc) {

            message += "gCO = " + QString::number(gCO) + " g/kWh\n";
            qDebug() << "gCO =" << gCO << "g/kWh";
        }

        if (gCHcalc) {

            message += "gCH = " + QString::number(gCH) + " g/kWh\n";
            qDebug() << "gCH =" << gCH << "g/kWh";
        }

        QString ptcalc = params->val_PTcalc();

        if (ptcalc != "no") {

            if (ptcalc == "ThroughPTmass") {

                message += "gPT = " + QString::number(gPT) + " g/kWh\n";
                qDebug() << "gPT =" << gPT << "g/kWh";
            }

            message += "gPTs = " + QString::number(gPTs) + " g/kWh\n";
            qDebug() << "gPTs =" << gPTs << "g/kWh";
        }
    }

    //

    return message;
}

QString CycleEmissions::lastReportsDir() {

    return fullReportsPath;
}
