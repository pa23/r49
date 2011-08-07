/*
    cycleemissions.cpp

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

#include <QSharedPointer>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QDir>

using std::string;
using std::cout;
using std::cin;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

CycleEmissions::CycleEmissions(QSharedPointer<LibtoxicParameters> prms, QSharedPointer<CommonParameters> cfg) :
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

    if (params.data()->val_CalcConfigFile() != "_._") {

        params.data()->readCalcConfigFile(params.data()->val_CalcConfigFile());
    }
}

CycleEmissions::~CycleEmissions() {
}

CycleEmissions::CycleEmissions(const CycleEmissions &orig) {

    (void)(&orig);
}

CycleEmissions &CycleEmissions::operator =(const CycleEmissions &x) {

    (void)(&x);
    return *this;
}

bool CycleEmissions::calculate() {

    ptrdiff_t std = params.data()->val_Standard();

    if (!preCalculate()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if (!calculate_gNOx()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) &&
           (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) && (NumberOfPoints == TCyclePointsNumber) ) {

        if (!calculateAdditionalPoints()) {

            qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

            return false;
        }
    }

    if (!calculate_gCO()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if (!calculate_gCH()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if (!calculate_gPT()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if (!calculate_rEGR()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    if (std != STD_FREECALC) {

        if (!calculate_Means()) {

            qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

            return false;
        }
    }

    if (!compareAlpha()) {

        qDebug() << Q_FUNC_INFO << ":::" << "returns false!";

        return false;
    }

    return true;
}

bool CycleEmissions::readCSV(QVector< QVector<double> > data) {

    if ( data.isEmpty() ) {

        QString filenamePoints = config.data()->val_filenamePoints();
        QString csvdelimiter = config.data()->val_csvDelimiter();

        QSharedPointer<csvRead> readerDataForCalc(new csvRead(filenamePoints, csvdelimiter, StrsNumberForColumnCaption));

        array_DataForCalc = readerDataForCalc.data()->csvData();

        if (array_DataForCalc.isEmpty()) {

            qDebug() << Q_FUNC_INFO << ":::" << "Incorrect source data!";

            return false;
        }

        if ( array_DataForCalc.at(0).size() != PointsFileColumnsNumber ) {

            qDebug() << Q_FUNC_INFO << ":::" << "Incorrect source data!";

            return false;
        }
    }
    else {

        array_DataForCalc = data;
    }

    NumberOfPoints = array_DataForCalc.size();

    ptrdiff_t std = params.data()->val_Standard();
    ptrdiff_t addpc = params.data()->val_AddPointsCalc();

    if (
            (
                    (
                            (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3)
                    ) &&
                    (
                            (addpc == ADDPOINTSCALC_YES)
                    ) &&
                    (
                            (NumberOfPoints != TCyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3)
                    ) &&
                    (
                            (addpc == ADDPOINTSCALC_NO)
                    ) &&
                    (
                            (NumberOfPoints != (TCyclePointsNumber-TCycleAddPointsNumber))
                    )
            ) ||
            (
                    (
                            (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) || (std == STD_OST) || (std == STD_GOST)
                    ) &&
                    (
                            (NumberOfPoints != (TCyclePointsNumber-TCycleAddPointsNumber))
                    )
            ) ||
            (
                    (
                            (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) || (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8)
                    ) &&
                    (
                            (NumberOfPoints != ECyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) || (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5)
                    ) &&
                    (
                            (NumberOfPoints != FCyclePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_C1)
                    ) &&
                    (
                            (NumberOfPoints != GC1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_D1)
                    ) &&
                    (
                            (NumberOfPoints != GD1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_D2)
                    ) &&
                    (
                            (NumberOfPoints != GD2CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_E1)
                    ) &&
                    (
                            (NumberOfPoints != GE1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_E2)
                    ) &&
                    (
                            (NumberOfPoints != GE2CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_E3)
                    ) &&
                    (
                            (NumberOfPoints != GE3CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_E5)
                    ) &&
                    (
                            (NumberOfPoints != GE5CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_F)
                    ) &&
                    (
                            (NumberOfPoints != GFCylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_G1)
                    ) &&
                    (
                            (NumberOfPoints != GG1CylcePointsNumber)
                    )
            ) ||
            (
                    (
                            (std == STD_G2)
                    ) &&
                    (
                            (NumberOfPoints != GG2CylcePointsNumber)
                    )
            )
    ) {

        qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

        return false;
    }

    array_n.resize(NumberOfPoints);
    array_Me_brutto.resize(NumberOfPoints);
    array_Ne_brutto.resize(NumberOfPoints);
    array_N_fan.resize(NumberOfPoints);
    array_w.resize(NumberOfPoints);
    array_t0.resize(NumberOfPoints);
    array_B0.resize(NumberOfPoints);
    array_Ra.resize(NumberOfPoints);
    array_dPn.resize(NumberOfPoints);
    array_Gair.resize(NumberOfPoints);
    array_Gfuel.resize(NumberOfPoints);
    array_CNOx.resize(NumberOfPoints);
    array_gNOx.resize(NumberOfPoints);
    array_CCO.resize(NumberOfPoints);
    array_CCH.resize(NumberOfPoints);
    array_CCO2in.resize(NumberOfPoints);
    array_CCO2out.resize(NumberOfPoints);
    array_CO2.resize(NumberOfPoints);
    array_Ka1m.resize(NumberOfPoints);
    array_KaPerc.resize(NumberOfPoints);
    array_FSN.resize(NumberOfPoints);
    array_Pr.resize(NumberOfPoints);
    array_ts.resize(NumberOfPoints);
    array_tauf.resize(NumberOfPoints);
    array_qmdw.resize(NumberOfPoints);
    array_qmdew.resize(NumberOfPoints);
    array_rd.resize(NumberOfPoints);

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        array_n        [i] = array_DataForCalc[i][ 1];
        array_Me_brutto[i] = array_DataForCalc[i][ 2];
        array_Ne_brutto[i] = array_DataForCalc[i][ 3];
        array_N_fan    [i] = array_DataForCalc[i][ 4];
        array_w        [i] = array_DataForCalc[i][ 5];
        array_t0       [i] = array_DataForCalc[i][ 6];
        array_B0       [i] = array_DataForCalc[i][ 7];
        array_Ra       [i] = array_DataForCalc[i][ 8];
        array_dPn      [i] = array_DataForCalc[i][ 9];
        array_Gair     [i] = array_DataForCalc[i][10];
        array_Gfuel    [i] = array_DataForCalc[i][11];
        array_CNOx     [i] = array_DataForCalc[i][12];
        array_gNOx     [i] = array_DataForCalc[i][13];
        array_CCO      [i] = array_DataForCalc[i][14];
        array_CCH      [i] = array_DataForCalc[i][15];
        array_CCO2in   [i] = array_DataForCalc[i][16];
        array_CCO2out  [i] = array_DataForCalc[i][17];
        array_CO2      [i] = array_DataForCalc[i][18];
        array_Ka1m     [i] = array_DataForCalc[i][19];
        array_KaPerc   [i] = array_DataForCalc[i][20];
        array_FSN      [i] = array_DataForCalc[i][21];
        array_Pr       [i] = array_DataForCalc[i][22];
        array_ts       [i] = array_DataForCalc[i][23];
        array_tauf     [i] = array_DataForCalc[i][24];
        array_qmdw     [i] = array_DataForCalc[i][25];
        array_qmdew    [i] = array_DataForCalc[i][26];
        array_rd       [i] = array_DataForCalc[i][27];
    }

    mytime = dateTimeNow();

    //

    if (params.data()->val_Standard() != STD_FREECALC) {

        if (!nonZeroArray(array_n)) {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (n)!";
            return false;
        }

        if (nonZeroArray(array_Me_brutto)) {

            NenCalcMethod = true;
        }
        else if (nonZeroArray(array_Ne_brutto)) {

            NenCalcMethod = false;
        }
        else {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (Me_b or Ne_b)!";
            return false;
        }

        if (!nonZeroArray(array_t0)) {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (t0)!";
            return false;
        }

        if (!nonZeroArray(array_B0)) {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (B0)!";
            return false;
        }

        if (!nonZeroArray(array_Ra)) {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (Ra)!";
            return false;
        }

        if (nonZeroArray(array_Gair)) {

            GairVals = true;
        }
        else if (nonZeroArray(array_dPn)) {

            GairVals = false;
        }
        else {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (Gair or dPn)!";
            return false;
        }

        if (!nonZeroArray(array_Gfuel)) {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (Gfuel)!";
            return false;
        }

        if (nonZeroArray(array_CNOx)) {

            NOxCalcMethod = true;
        }
        else if (nonZeroArray(array_gNOx)) {

            NOxCalcMethod = false;
        }
        else {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings (C_NOx or gNOx)!";
            return false;
        }

        if (nonZeroArray(array_CCO)) {

            gCOcalc = true;
        }
        else {

            gCOcalc = false;
        }

        if (nonZeroArray(array_CCH)) {

            gCHcalc = true;
        }
        else {

            gCHcalc = false;
        }

        if ( nonZeroArray(array_CCO2in) && nonZeroArray(array_CCO2out) ) {

            EGRcalc = true;
        }
        else {

            EGRcalc = false;
        }

        if (nonZeroArray(array_CO2)) {

            CheckMeas = true;
        }
        else {

            CheckMeas = false;
        }

        if (params.data()->val_PTcalc() != PTCALC_NO) {

            if ( !nonZeroArray(array_Pr) || !nonZeroArray(array_ts) ||
                 ( !nonZeroArray(array_Ka1m)   &&
                   !nonZeroArray(array_KaPerc) &&
                   !nonZeroArray(array_FSN)       ) ) {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";
                return false;
            }

            if (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) {

                if ( !nonZeroArray(array_tauf) || !nonZeroArray(array_qmdew) ||
                     ( !nonZeroArray(array_qmdw) &&
                       !nonZeroArray(array_rd)      ) ) {

                    qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";
                    return false;
                }

                if (nonZeroArray(array_qmdw)) {

                    qmdwVSrd = true;
                }
                else if (nonZeroArray(array_rd)) {

                    qmdwVSrd = false;
                }
            }
        }

        if (nonZeroArray(array_FSN)) {

            smoke = 2;
        }
        else if (nonZeroArray(array_Ka1m)) {

            smoke = 0;
        }
        else if (nonZeroArray(array_KaPerc)) {

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

    array_Ne_netto.resize(NumberOfPoints);
    array_Me_netto.resize(NumberOfPoints);
    array_alpha.resize(NumberOfPoints);
    array_alpha_O2.resize(NumberOfPoints);
    array_Gexh.resize(NumberOfPoints);
    array_Gexhd.resize(NumberOfPoints);
    array_Pb.resize(NumberOfPoints);
    array_Pa.resize(NumberOfPoints);
    array_Ha.resize(NumberOfPoints);
    array_Gaird.resize(NumberOfPoints);
    array_Kw2.resize(NumberOfPoints);
    array_Ffh.resize(NumberOfPoints);
    array_Kf.resize(NumberOfPoints);
    array_Kwr.resize(NumberOfPoints);
    array_Khd.resize(NumberOfPoints);

    ptrdiff_t std = params.data()->val_Standard();

    double L0    = config.data()->val_L0();
    double WH    = config.data()->val_WH();
    double WO2   = config.data()->val_WO2();
    double WN    = config.data()->val_WN();
    double roAir = config.data()->val_roAir();

    double Ffw = 0;
    double Ffd = 0;

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        ptrdiff_t FuelType = params.data()->val_FuelType();

        if (FuelType == FUELTYPE_DIESEL) {

            Ffw = 0.75;
            Ffd = -0.77;
        }
        else if (FuelType == FUELTYPE_MOTOR) {

            Ffw = 0.72;
            Ffd = -0.74;
        }
        else if (FuelType == FUELTYPE_MAZUT) {

            Ffw = 0.69;
            Ffd = -0.71;
        }
        else {

            qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

            return false;
        }
    }

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        if (NenCalcMethod) {

            array_Ne_brutto[i] = array_Me_brutto[i] * array_n[i] / 9550.0;
        }
        else if (!NenCalcMethod) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ||
             (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) || (std == STD_FREECALC) ) {

            array_Ne_netto[i] = array_Ne_brutto[i] - array_N_fan[i];
        }
        else {

            array_Ne_netto[i] = array_Ne_brutto[i];
        }

        array_Me_netto[i] = array_Ne_netto[i] * 9550.0 / array_n[i];

        if (!GairVals) {

            double Dn = config.data()->val_Dn();

            if (Dn < 1) {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

                return false;
            }

            array_Gair[i] = 0.0084591 * pow(Dn, 2) * sqrt((1.019716213 * array_dPn[i] * 7.500616827 * array_B0[i])/(array_t0[i] + 273.0));
        }

        array_alpha[i] = array_Gair[i] / array_Gfuel[i] / L0;

        if (CheckMeas) {

            double ConcO2air = config.data()->val_ConcO2air();

            if (ConcO2air < 1) {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

                return false;
            }

            if (!EGRcalc) {

                array_alpha_O2[i] = (0.01 * ConcO2air * (1.0 - 0.01 * array_CO2[i]) + 0.273274 * 0.01 * array_CO2[i]) /
                                    (0.01 * ConcO2air - 0.01 * array_CO2[i]);
            }
        }

        if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
             (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
             (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

            array_Gexh[i] = array_Gair[i] / roAir + Ffw * array_Gfuel[i];
            array_Gexhd[i] = array_Gair[i] / roAir + Ffd * array_Gfuel[i];
        }
        else {

            array_Gexh[i] = array_Gair[i] + array_Gfuel[i];
        }

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
             (std != STD_F ) && (std != STD_G1) && (std != STD_G2) ) {

            array_Pb[i] = array_B0[i];
            array_Pa[i] = val_Pa(array_t0[i]);
            array_Ha[i] = 6.22 * array_Ra[i] * array_Pa[i] / (array_Pb[i] - array_Pa[i] * array_Ra[i] * 0.01);

            if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) || (std == STD_FREECALC) ||
                 (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) ||
                 (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) ||
                 (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
                 (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

                array_Gaird[i] = (1.0 - array_Ha[i] / 1000.0) * array_Gair[i];
            }
            else if ( (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) || (std == STD_OST) || (std == STD_GOST) ) {

                array_Gaird[i] = array_Gair[i] / (1.0 + array_Ha[i] / 1000.0);
            }
            else {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

                return false;
            }

            if (std != STD_OST) {

                array_Kw2[i] = 1.608 * array_Ha[i] / (1000.0 + 1.608 * array_Ha[i]);
                array_Ffh[i] = 1.969 / (1.0 + array_Gfuel[i] / array_Gair[i]);
                array_Kf[i] = 0.055594 * WH + 0.0080021 * WN + 0.0070046 * WO2;
            }

            if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) || (std == STD_FREECALC) ) {

                array_Kwr[i] = (1.0 - (1.2442 * array_Ha[i] + 111.19 * WH * array_Gfuel[i] / array_Gaird[i]) /
                                      (773.4 + 1.2442 * array_Ha[i] + array_Gfuel[i] / array_Gaird[i] * array_Kf[i] * 1000.0)) * 1.008;
                array_Khd[i] = 1.0 / (1.0 - 0.0182 * (array_Ha[i] - 10.71) + 0.0045 * (array_t0[i] + 273.0 - 298.0));
            }
            else if ( (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) ||
                      (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) ||
                      (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
                      (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

                array_Kwr[i] = (1.0 - array_Ffh[i] * array_Gfuel[i] / array_Gaird[i]) - array_Kw2[i];
                array_Khd[i] = 1.0 / (1.0 + (0.309 * array_Gfuel[i] / array_Gaird[i] - 0.0266) * (array_Ha[i] - 10.71) +
                                     (-0.209 * array_Gfuel[i] / array_Gaird[i] + 0.00954) * (array_t0[i] + 273.0 - 298.0));
            }
            else if ( (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) ) {

                array_Kwr[i] = 1.0 - 1.85 * array_Gfuel[i] / array_Gaird[i];
                array_Khd[i] = 1.0 / (1.0 + (0.044 * array_Gfuel[i] / array_Gaird[i] - 0.0038) * (7.0 * array_Ha[i] - 75.0) +
                                            (-0.116 * array_Gfuel[i] / array_Gaird[i] + 0.0053) * 1.8 * (array_t0[i] + 273.0 - 302.0));
            }
            else if (std == STD_OST) {

                array_Kwr[i] = 1.0 - 1.8 * array_Gfuel[i] / array_Gair[i];
            }
            else if (std == STD_GOST) {

                array_Kwr[i] = 1.0 - 1.85 * array_Gfuel[i] / array_Gair[i];
                array_Khd[i] = 1.0 / (1.0 + (0.044 * array_Gfuel[i] / array_Gair[i] - 0.0038) * (7.0 * array_Ha[i] - 75.0) +
                                            (-0.116 * array_Gfuel[i] / array_Gair[i] + 0.0053) * 1.8 * (array_t0[i] + 273.0 - 302.0));
            }
            else {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

                return false;
            }
        }
    }

    return true;
}

bool CycleEmissions::calculate_gNOx() {

    array_mNOx.resize(NumberOfPoints);

    double muNO2 = config.data()->val_muNO2();

    double summ_mNOx = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    ptrdiff_t std = params.data()->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

            for (ptrdiff_t i=0; i<n; i++) {

                summ_numerator += array_CNOx[i] / 10000.0 * array_Gexhd[i] * array_w[i];
                summ_denominator += array_Ne_brutto[i] / array_Ne_brutto[0] * array_w[i];
            }
        }
        else {

            for (ptrdiff_t i=0; i<n; i++) {

                summ_numerator += array_CNOx[i] / 10000.0 * array_Gexh[i] * array_w[i];
                summ_denominator += array_Ne_brutto[i] / array_Ne_brutto[0] * array_w[i];
            }
        }

        gNOx = 0.446 * muNO2 * summ_numerator / (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        if (NOxCalcMethod) {

            if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

                for (ptrdiff_t i=0; i<n; i++) {

                    if (std == STD_OST) {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] * array_Gexh[i];
                    }
                    else {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] * array_Khd[i] * array_Gexh[i];
                    }

                    array_gNOx[i] = array_mNOx[i] / array_Ne_netto[i];

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else {

                for (ptrdiff_t i=0; i<n; i++) {

                    if (std == STD_OST) {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Gexh[i];
                    }
                    else {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Khd[i] * array_Gexh[i];
                    }

                    array_gNOx[i] = array_mNOx[i] / array_Ne_netto[i];

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
        }
        else if (!NOxCalcMethod) {

            if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_mNOx[i] = array_gNOx[i] * array_Ne_netto[i];

                    if (std == STD_OST) {

                        array_CNOx[i] = array_mNOx[i] / (0.001587 * array_Kwr[i] * array_Gexh[i]);
                    }
                    else {

                        array_CNOx[i] = array_mNOx[i] / (0.001587 * array_Kwr[i] * array_Khd[i] * array_Gexh[i]);
                    }

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_mNOx[i] = array_gNOx[i] * array_Ne_netto[i];

                    if (std == STD_OST) {

                        array_CNOx[i] = array_mNOx[i] / (0.001587 * array_Gexh[i]);
                    }
                    else {

                        array_CNOx[i] = array_mNOx[i] / (0.001587 * array_Khd[i] * array_Gexh[i]);
                    }

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
        }

        gNOx = summ_mNOx / summ_Ne_netto;
    }

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

    ptrdiff_t std = params.data()->val_Standard();

    for (ptrdiff_t i=(NumberOfPoints - TCycleAddPointsNumber); i<NumberOfPoints; i++) {

        if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

            if (std == STD_OST) {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] * array_Gexh[i];
            }
            else {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] * array_Khd[i] * array_Gexh[i];
            }
        }
        else {

            if (std == STD_OST) {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Gexh[i];
            }
            else {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Khd[i] * array_Gexh[i];
            }
        }

        array_gNOx[i] = array_mNOx[i] / array_Ne_netto[i];
    }

    gNOx1m = array_gNOx[NumberOfPoints - 3];
    gNOx2m = array_gNOx[NumberOfPoints - 2];
    gNOx3m = array_gNOx[NumberOfPoints - 1];

    double A = array_n[1];
    double B = array_n[2];
    double C = array_n[9];

    double nz1 = array_n[NumberOfPoints - 3];
    double nz2 = array_n[NumberOfPoints - 2];
    double nz3 = array_n[NumberOfPoints - 1];

    double nrt1 = B;
    double nrt2 = A;
    double nrt3 = B;

    double nsu1 = C;
    double nsu2 = B;
    double nsu3 = C;

    double Nz1 = array_Ne_netto[NumberOfPoints - 3];
    double Nz2 = array_Ne_netto[NumberOfPoints - 2];
    double Nz3 = array_Ne_netto[NumberOfPoints - 1];

    double Nt1 = array_Ne_netto[7];
    double Nt2 = array_Ne_netto[5];
    double Nt3 = array_Ne_netto[2];

    double Nu1 = array_Ne_netto[ 9];
    double Nu2 = array_Ne_netto[ 3];
    double Nu3 = array_Ne_netto[12];

    double Nr1 = array_Ne_netto[3];
    double Nr2 = array_Ne_netto[4];
    double Nr3 = array_Ne_netto[8];

    double Ns1 = array_Ne_netto[11];
    double Ns2 = array_Ne_netto[ 2];
    double Ns3 = array_Ne_netto[10];

    double Ntu1 = Nt1 + (Nu1 - Nt1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double Ntu2 = Nt2 + (Nu2 - Nt2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double Ntu3 = Nt3 + (Nu3 - Nt3) * (nz3 - nrt3) / (nsu3 - nrt3);

    double Nrs1 = Nr1 + (Ns1 - Nr1) * (nz1 - nrt1) / (nsu1 - nrt1);
    double Nrs2 = Nr2 + (Ns2 - Nr2) * (nz2 - nrt2) / (nsu2 - nrt2);
    double Nrs3 = Nr3 + (Ns3 - Nr3) * (nz3 - nrt3) / (nsu3 - nrt3);

    double gNOt1 = array_gNOx[7];
    double gNOt2 = array_gNOx[5];
    double gNOt3 = array_gNOx[2];

    double gNOu1 = array_gNOx[ 9];
    double gNOu2 = array_gNOx[ 3];
    double gNOu3 = array_gNOx[12];

    double gNOr1 = array_gNOx[3];
    double gNOr2 = array_gNOx[4];
    double gNOr3 = array_gNOx[8];

    double gNOs1 = array_gNOx[11];
    double gNOs2 = array_gNOx[ 2];
    double gNOs3 = array_gNOx[10];

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

    array_mCO.resize(NumberOfPoints);
    array_gCO.resize(NumberOfPoints);

    double muCO = config.data()->val_muCO();

    double summ_mCO = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    ptrdiff_t std = params.data()->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        for (ptrdiff_t i=0; i<n; i++) {

            summ_numerator += array_CCO[i] / 10000.0 * array_Gexhd[i] * array_w[i];
            summ_denominator += array_Ne_brutto[i] / array_Ne_brutto[0] * array_w[i];
        }

        gCO = 0.446 * muCO * summ_numerator / (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        for (ptrdiff_t i=0; i<n; i++) {

            array_mCO[i] = 0.000966 * array_CCO[i] * array_Kwr[i] * array_Gexh[i]; // always is dry
            array_gCO[i] = array_mCO[i] / array_Ne_netto[i];

            summ_mCO += array_mCO[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gCO = summ_mCO / summ_Ne_netto;
    }

    return true;
}

bool CycleEmissions::calculate_gCH() {

    array_mCH.resize(NumberOfPoints);
    array_gCH.resize(NumberOfPoints);

    double muCH = config.data()->val_muCH();

    double summ_mCH = 0;
    double summ_Ne_netto = 0;

    double summ_numerator = 0;
    double summ_denominator = 0;

    ptrdiff_t std = params.data()->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        for (ptrdiff_t i=0; i<n; i++) {

            summ_numerator += array_CCH[i] / 10000.0 * array_Gexh[i] * array_w[i];
            summ_denominator += array_Ne_brutto[i] / array_Ne_brutto[0] * array_w[i];
        }

        gCH = 0.446 * muCH * summ_numerator / (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        for (ptrdiff_t i=0; i<n; i++) {

            if (std == STD_OST) {

                array_mCH[i] = 0.000485 * array_CCH[i] * array_Gexh[i]; // always is wet
            }
            else {

                array_mCH[i] = 0.000479 * array_CCH[i] * array_Gexh[i]; // always is wet
            }

            array_gCH[i] = array_mCH[i] / array_Ne_netto[i];

            summ_mCH += array_mCH[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gCH = summ_mCH / summ_Ne_netto;
    }

    return true;
}

bool CycleEmissions::calculate_gPT() {

    mf = params.data()->val_PTmass();

    array_ror.resize(NumberOfPoints);
    array_CPT.resize(NumberOfPoints);
    array_qmedf.resize(NumberOfPoints);
    array_msepi.resize(NumberOfPoints);
    array_mPT.resize(NumberOfPoints);
    array_gPT.resize(NumberOfPoints);

    ptrdiff_t std = params.data()->val_Standard();

    if ( (params.data()->val_PTcalc() != PTCALC_NO) && ( (std != STD_OST) && (std != STD_GOST) && (std != STD_EU0) &&
                                                         (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                                                         (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                                                         (std != STD_F ) && (std != STD_G1) && (std != STD_G2) ) ) {

        ptrdiff_t n = 0;
        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            n = NumberOfPoints - TCycleAddPointsNumber;
        }
        else {

            n = NumberOfPoints;
        }

        double L = config.data()->val_L();
        double Rr = config.data()->val_Rr();

        double summ_mPT = 0;
        double summ_Ne_netto = 0;

        for (ptrdiff_t i=0; i<n; i++) {

            if (smoke == 0) {

                array_KaPerc[i] = 100.0 * (1.0 - exp(- array_Ka1m[i] * L));
                array_FSN[i] = (6.6527E-017)           * pow(array_KaPerc[i], 10) +
                               (-0.000000000000026602) * pow(array_KaPerc[i],  9) +
                               (0.0000000000040987)    * pow(array_KaPerc[i],  8) +
                               (-0.00000000026927)     * pow(array_KaPerc[i],  7) +
                               (0.00000000040933)      * pow(array_KaPerc[i],  6) +
                               (0.0000010658)          * pow(array_KaPerc[i],  5) +
                               (-0.000069165)          * pow(array_KaPerc[i],  4) +
                               (0.0020088)             * pow(array_KaPerc[i],  3) +
                               (-0.028758)             * pow(array_KaPerc[i],  2) +
                               (0.26502)               * pow(array_KaPerc[i],  1) +
                               (0.0087517)             * pow(array_KaPerc[i],  0);
            }
            else if (smoke == 1) {

                array_FSN[i] = (6.6527E-017)           * pow(array_KaPerc[i], 10) +
                               (-0.000000000000026602) * pow(array_KaPerc[i],  9) +
                               (0.0000000000040987)    * pow(array_KaPerc[i],  8) +
                               (-0.00000000026927)     * pow(array_KaPerc[i],  7) +
                               (0.00000000040933)      * pow(array_KaPerc[i],  6) +
                               (0.0000010658)          * pow(array_KaPerc[i],  5) +
                               (-0.000069165)          * pow(array_KaPerc[i],  4) +
                               (0.0020088)             * pow(array_KaPerc[i],  3) +
                               (-0.028758)             * pow(array_KaPerc[i],  2) +
                               (0.26502)               * pow(array_KaPerc[i],  1) +
                               (0.0087517)             * pow(array_KaPerc[i],  0);
            }

            array_ror[i] = (array_Pb[i] + array_Pr[i]) * 1000.0 / Rr / (array_ts[i] + 273.0);
            array_CPT[i] = (-184.0 * array_FSN[i] - 727.5) * log10(1.0 - array_FSN[i] / 10.0);
            array_mPT[i] = array_CPT[i] * array_Gexh[i] / array_ror[i] / 1000.0;
            array_gPT[i] = array_mPT[i] / array_Ne_netto[i];

            summ_mPT += array_mPT[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gPTs = summ_mPT / summ_Ne_netto;

        summ_mPT = 0;
        summ_Ne_netto = 0;

        if (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) {

            if (mf == 0) {

                cout << "Enter mass of 1st clean filter, mg: "; cin >> mf1c;
                cout << "Enter mass of 1st dirty filter, mg: "; cin >> mf1d;
                cout << "Enter mass of 2nd clean filter, mg: "; cin >> mf2c;
                cout << "Enter mass of 2nd dirty filter, mg: "; cin >> mf2d;

                mf = (mf1d - mf1c) + (mf2d - mf2c);
            }

            if (qmdwVSrd) {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_rd[i] = array_qmdew[i] / (array_qmdew[i] - array_qmdw[i]);
                    array_qmedf[i] = array_Gexh[i] * array_rd[i];
                    qmedfl += array_qmedf[i] * array_w[i];
                    array_msepi[i] = array_qmdew[i] * array_tauf[i];
                    msep += array_msepi[i];

                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else if (!qmdwVSrd) {

                for (ptrdiff_t i=0; i<n; i++) {

                    array_qmdw[i] = (array_qmdew[i] * array_rd[i] - array_qmdew[i]) / array_rd[i];
                    array_qmedf[i] = array_Gexh[i] * array_rd[i];
                    qmedfl += array_qmedf[i] * array_w[i];
                    array_msepi[i] = array_qmdew[i] * array_tauf[i];
                    msep += array_msepi[i];

                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }

            mPT = mf * qmedfl / msep;
            gPT = mPT / summ_Ne_netto;
        }
    }

    return true;
}

bool CycleEmissions::calculate_rEGR() {

    array_rEGR.resize(NumberOfPoints);
    array_alpha_res.resize(NumberOfPoints);

    double CCO2air = config.data()->val_ConcCO2air();

    if (EGRcalc) {

        for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

            array_rEGR[i] = (array_CCO2in[i] - CCO2air) / (array_CCO2out[i] - CCO2air) * 100.0;
            array_alpha_res[i] = (array_alpha[i] - array_alpha[i] * array_rEGR[i] / 100.0 - array_rEGR[i] / 100.0) / (1.0 - 2.0 * array_rEGR[i] / 100.0);
        }

    }

    return true;
}

bool CycleEmissions::calculate_Means() {

    double summ_Gfuel    = 0;
    double summ_Ne_netto = 0;

    double summ_t0 = 0;
    double summ_B0 = 0;
    double summ_Ra = 0;

    ptrdiff_t std = params.data()->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCycleAddPointsNumber;
    }
    else {

        n = NumberOfPoints;
    }

    for (ptrdiff_t i=0; i<n; i++) {

        summ_Gfuel    += array_Gfuel[i] * array_w[i];
        summ_Ne_netto += array_Ne_netto[i] * array_w[i];

        summ_t0 += array_t0[i];
        summ_B0 += array_B0[i];
        summ_Ra += array_Ra[i];
    }

    geMean = summ_Gfuel / summ_Ne_netto * 1000.0;

    t0Mean = summ_t0 / n;
    B0Mean = summ_B0 / n;
    RaMean = summ_Ra / n;

    return true;
}

bool CycleEmissions::compareAlpha() {

    array_diff_alpha.resize(NumberOfPoints);

    if (CheckMeas) {

        if (EGRcalc) {

            double ConcO2mix = 0;
            double ConcO2air = config.data()->val_ConcO2air();

            if (ConcO2air < 1) {

                qDebug() << Q_FUNC_INFO << ":::" << "Bad source data or calculation settings!";

                return false;
            }

            for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

                ConcO2mix = ConcO2air * (1.0 - array_rEGR[i] / 100.0) + array_CO2[i] * array_rEGR[i] / 100.0;
                array_alpha_O2[i] = (0.01 * ConcO2mix * (1.0 - 0.01 * array_CO2[i]) + 0.273274 * 0.01 * array_CO2[i]) /
                                    (0.01 * ConcO2mix - 0.01 * array_CO2[i]);
                array_diff_alpha[i] = (array_alpha_res[i] - array_alpha_O2[i]) / array_alpha_O2[i] * 100.0;
            }
        }
        else {

            for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

                array_diff_alpha[i] = (array_alpha[i] - array_alpha_O2[i]) / array_alpha_O2[i] * 100.0;
            }
        }

    }

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

        ptrdiff_t ptcalc = params.data()->val_PTcalc();

        if (ptcalc != PTCALC_NO) {

            if (ptcalc == PTCALC_THROUGHPTMASS) {

                message += "gPT = " + QString::number(gPT) + " g/kWh\n";
                qDebug() << "gPT =" << gPT << "g/kWh";
            }

            message += "gPTs = " + QString::number(gPTs) + " g/kWh\n";
            qDebug() << "gPTs =" << gPTs << "g/kWh";
        }

        //

        return message;
    }

    ptrdiff_t std = params.data()->val_Standard();

    QString dirnameReports = config.data()->val_dirnameReports();
    string csvdelimiter = (config.data()->val_csvDelimiter()).toStdString();

    fullReportsPath = dirnameReports + "/" + params.data()->defStandardName(std) + "_" + QString::fromStdString(mytime);
    QDir reportdir;
    reportdir.mkdir(fullReportsPath);

    //

    string CheckoutDataFileName;
    CheckoutDataFileName = "CheckoutData_" + mytime + ".csv";

    QString checkoutdata = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(CheckoutDataFileName);
    ofstream fout1(checkoutdata.toAscii());

    if (!fout1) {

        message += QString::fromAscii(Q_FUNC_INFO) + ":::" + "fout1 was not created!\n";
        qDebug() << Q_FUNC_INFO << ":::" << "fout1 was not created!";

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
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_NOx[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "gNOx[g/kWh]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_CH[ppm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO2in[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO2out[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "C_O2[%]" << csvdelimiter;
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

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

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
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "diff_alpha[%]" << csvdelimiter << "\n";

    ptrdiff_t prec = Precision + 1;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(0) << (i + 1) << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(0) << array_n[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Me_brutto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ne_brutto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_N_fan[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_w[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_t0[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_B0[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ra[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_dPn[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gair[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gfuel[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CNOx[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gNOx[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCH[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO2in[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CCO2out[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CO2[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ka1m[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_KaPerc[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_FSN[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pr[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_ts[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_tauf[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmdw[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmdew[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_rd[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ne_netto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Me_netto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha_O2[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gexh[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gexhd[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pb[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Pa[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ha[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Gaird[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kw2[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Ffh[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kf[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Kwr[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_Khd[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mNOx[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mCO[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gCO[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mCH[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gCH[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_ror[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_CPT[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_mPT[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_gPT[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_qmedf[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_msepi[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_rEGR[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_alpha_res[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(prec) << array_diff_alpha[i] << csvdelimiter << "\n";
    }

    fout1.close();

    message += "libtoxic: Additional file \"" + QString::fromStdString(CheckoutDataFileName) + "\" rewrited.\n";
    qDebug() << "\nlibtoxic: Additional file" << QString::fromStdString(CheckoutDataFileName) << "rewrited.";

    //

    string SourceDataFileName;

    if ( (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) ||
         (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) ||
         (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
         (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

        SourceDataFileName = "SourceData96_" + mytime + ".csv";
    }
    else if (std == STD_OST) {

        SourceDataFileName = "SourceDataOST_" + mytime + ".csv";
    }
    else if (std == STD_GOST) {

        SourceDataFileName = "SourceDataGOST_" + mytime + ".csv";
    }
    else if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
              (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
              (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        SourceDataFileName = "SourceDataGOST51249_" + mytime + ".csv";
    }
    else {

        SourceDataFileName = "SourceData49_" + mytime + ".csv";
    }

    QString srcdata = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(SourceDataFileName);
    ofstream fout4(srcdata.toAscii());

    if (!fout4) {

        message += QString::fromAscii(Q_FUNC_INFO) + ":::" + "fout4 was not created!\n";
        qDebug() << Q_FUNC_INFO << ":::" << "fout4 was not created!";

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
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_NOx[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "gNOx[g/kWh]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_CH[ppm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO2in[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_CO2out[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "C_O2[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[m-1]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ka[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "FSN[-]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Pr[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "ts[oC]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "tauf[s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "qmdw[g/s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "qmdew[g/s]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "rd[-]" << csvdelimiter << "\n";

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        for (ptrdiff_t j=0; j<PointsFileColumnsNumber; j++) {

            fout4 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+1) << array_DataForCalc[i][j] << csvdelimiter;
        }

        fout4 << "\n";
    }

    fout4.close();

    message += "libtoxic: SourceData file \"" + QString::fromStdString(SourceDataFileName) + "\" created.\n";
    qDebug() << "libtoxic: SourceData file" << QString::fromStdString(SourceDataFileName) << "created.";

    //

    if (std != STD_FREECALC) {

        if ( (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) && ( (std != STD_OST) && (std != STD_GOST) && (std != STD_EU0) &&
                                                                        (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                                                                        (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                                                                        (std != STD_F ) && (std != STD_G1) && (std != STD_G2) ) ) {

            string ReportFileNamePT;

            if ( (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) ||
                 (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) ||
                 (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
                 (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

                ReportFileNamePT = "Report96_PT_" + mytime + ".txt";
            }
            else {

                ReportFileNamePT = "Report49_PT_" + mytime + ".txt";
            }

            QString reppt = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(ReportFileNamePT);
            ofstream fout6(reppt.toAscii());

            if (!fout6) {

                message += QString::fromAscii(Q_FUNC_INFO) + ":::" + "fout6 was not created!\n";
                qDebug() << Q_FUNC_INFO << ":::" << "fout6 was not created!";

                return message;
            }

            fout6 << "\tr49 distribution version " << r49version.toStdString();
            fout6 << "\t\tReport on cycle. " << params.data()->defStandardName(std).toStdString() << "\t\tDateTime: " << mytime << "\n\n";

            fout6 << "Engine                 : ...\n";
            fout6 << fixed << setprecision(Precision) << "Environment parameters : t0_mean = "
                    << t0Mean <<  " oC; B0_mean = "
                    << B0Mean << " kPa; Ra_mean = "
                    << RaMean << " %\n";

            fout6 << "Calculation comments   : ";

            if (!GairVals) {

                fout6 << "Gair meas by nozzle (Dn = " << config.data()->val_Dn() << "); ";
            }
            else {

                fout6 << "direct Gair meas; ";
            }

            if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

                fout6 << "NOxSample type is dry; ";
            }
            else {

                fout6 << "NOxSample type is wet; ";
            }

            if (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) {

                fout6 << fixed << setprecision(Precision+1) << "PT calc method is PT mass based (mf = " << mf << " mg);\n";
            }
            else if (params.data()->val_PTcalc() == PTCALC_NO) {

                fout6 << "PT was not calculated;\n";
            }
            else {

                fout6 << "PT calc method is smoke meas based;\n";
            }

            if (CheckMeas) {

                fout6 << fixed << setprecision(Precision) << "                         concentrations meas checked (conc O2air = " << config.data()->val_ConcO2air() << " %)";
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
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "qmdew[g/s]" << "\n";

            ptrdiff_t n = 0;
            if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

                n = NumberOfPoints - TCycleAddPointsNumber;
            }
            else {

                n = NumberOfPoints;
            }

            for (ptrdiff_t i=0; i<n; i++) {

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << array_n[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[i];

                if (EGRcalc) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha_res[i];
                }
                else {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[i];
                }

                if (smoke == 0) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_Ka1m[i];
                }
                else if (smoke == 1) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_KaPerc[i];
                }
                else if (smoke == 2) {

                    fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_FSN[i];
                }

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_tauf[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_qmdw[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_qmdew[i] << "\n";
            }

            fout6 << "\n";

            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "rd[-]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "qmedf[kg/h]";
            fout6 << right << setw(WidthOfColumn-1) << setfill(' ') << "msepi[g]" << "\n";

            for (ptrdiff_t i=0; i<n; i++) {

                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_rd[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_qmedf[i];
                fout6 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_msepi[i] << "\n";
            }

            fout6 << "\n";

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
            fout6 << fixed << right << setw(WidthOfColumn+WidthOfColumn-2) << setfill(' ') << setprecision(Precision+1) << gPTs << "\n\n";

            fout6.close();

            message += "libtoxic: Report file \"" + QString::fromStdString(ReportFileNamePT) + "\" created.\n";
            qDebug() << "libtoxic: Report file" << QString::fromStdString(ReportFileNamePT) << "created.";
        }

        //

        string ReportFileNameGAS;

        if ( (std == STD_R96E8) || (std == STD_R96F8) || (std == STD_R96G8) || (std == STD_R96D8) ||
             (std == STD_R96E5) || (std == STD_R96F5) || (std == STD_R96G5) || (std == STD_R96D5) ||
             (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
             (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

            ReportFileNameGAS = "Report96_GAS_" + mytime + ".txt";
        }
        else if (std == STD_OST) {

            ReportFileNameGAS = "ReportOST_GAS_" + mytime + ".txt";
        }
        else if (std == STD_GOST) {

            ReportFileNameGAS = "ReportGOST_GAS_" + mytime + ".txt";
        }
        else if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                  (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
                  (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            ReportFileNameGAS = "ReportGOST51249_GAS_" + mytime + ".txt";
        }
        else {

            ReportFileNameGAS = "Report49_GAS_" + mytime + ".txt";
        }

        QString repgas = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(ReportFileNameGAS);
        ofstream fout5(repgas.toAscii());

        if (!fout5) {

            message += QString::fromAscii(Q_FUNC_INFO) + ":::" + "fout5 was not created!\n";
            qDebug() << Q_FUNC_INFO << ":::" << "fout5 was not created!";

            return message;
        }

        fout5 << "\tr49 distribution version " << r49version.toStdString();
        fout5 << "\t\tReport on cycle. " << params.data()->defStandardName(std).toStdString() << "\t\tDateTime: " << mytime << "\n\n";

        fout5 << "Engine                 : ...\n";
        fout5 << fixed << setprecision(Precision) << "Environment parameters : t0_mean = "
                << t0Mean <<  " oC; B0_mean = "
                << B0Mean << " kPa; Ra_mean = "
                << RaMean << " %\n";

        fout5 << "Calculation comments   : ";

        if (!GairVals) {

            fout5 << "Gair meas by nozzle (Dn = " << config.data()->val_Dn() << "); ";
        }
        else {

            fout5 << "direct Gair meas; ";
        }

        if (params.data()->val_NOxSample() == NOXSAMPLE_DRY) {

            fout5 << "NOxSample type is dry; ";
        }
        else {

            fout5 << "NOxSample type is wet; ";
        }

        if (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) {

            fout5 << fixed << setprecision(Precision+1) << "PT calc method is PT mass based (mf = " << mf << " mg);\n";
        }
        else if (params.data()->val_PTcalc() == PTCALC_NO) {

            fout5 << "PT was not calculated;\n";
        }
        else {

            fout5 << "PT calc method is smoke meas based;\n";
        }

        if (CheckMeas) {

            fout5 << fixed << setprecision(Precision) << "                         concentrations meas checked (conc O2air = " << config.data()->val_ConcO2air() << " %)";
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

        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "C_NOx[ppm]";

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
             (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mNOx[g/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOx[g/kWh]";
        }

        fout5 << "\n";

        ptrdiff_t n = 0;
        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) && (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            n = NumberOfPoints - TCycleAddPointsNumber;
        }
        else {

            n = NumberOfPoints;
        }

        for (ptrdiff_t i=0; i<n; i++) {

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << (i + 1);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(0) << array_n[i];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[i];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[i];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[i];

            if (EGRcalc) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha_res[i];
            }
            else {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[i];
            }

            if (smoke == 0) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_Ka1m[i];
            }
            else if (smoke == 1) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_KaPerc[i];
            }
            else if (smoke == 2) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_FSN[i];
            }

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CNOx[i];

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mNOx[i];
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gNOx[i];
            }

            fout5 << "\n";
        }

        fout5 << "\n";

        if ( (!EGRcalc) && (!CheckMeas) ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "C_CO[ppm]";
            }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                if (gCOcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCO[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gCO[g/kWh]";
                }
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "C_CH[ppm]";
            }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                if (gCHcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCH[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gCH[g/kWh]";
                }
            }

            if (params.data()->val_PTcalc() == PTCALC_THROUGHSMOKE) {

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mPTs[g/h]";
                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gPTs[g/kWh]";
                }
            }

            fout5 << "\n";

            for (ptrdiff_t i=0; i<n; i++) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (i + 1);

                if (gCOcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCO[i];
                }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if (gCOcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCO[i];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gCO[i];
                    }
                }

                if (gCHcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCH[i];
                }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if (gCHcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCH[i];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_gCH[i];
                    }
                }

                if (params.data()->val_PTcalc() == PTCALC_THROUGHSMOKE) {

                    if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                         (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mPT[i];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_gPT[i];
                    }
                }

                fout5 << "\n";
            }
        }
        else {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "C_CO[ppm]";
            }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                if (gCOcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCO[g/h]";
                }
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "C_CH[ppm]";
            }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                if (gCHcalc) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "mCH[g/h]";
                }
            }

            if (params.data()->val_PTcalc() == PTCALC_THROUGHSMOKE) {

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

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

            fout5 << "\n";

            for (ptrdiff_t i=0; i<n; i++) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (i + 1);

                if (gCOcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCO[i];
                }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if (gCOcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCO[i];
                    }
                }

                if (gCHcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_CCH[i];
                }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if (gCHcalc) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mCH[i];
                    }
                }

                if (params.data()->val_PTcalc() == PTCALC_THROUGHSMOKE) {

                    if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) && (std != STD_E1) && (std != STD_E2) &&
                         (std != STD_E3) && (std != STD_E5) && (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_mPT[i];
                        fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << array_gPT[i];
                    }
                }

                if (EGRcalc) {

                    fout5 << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_rEGR[i];
                }

                if (CheckMeas) {

                    fout5 << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_diff_alpha[i];
                }

                fout5 << "\n";
            }
        }

        fout5 << "\n";

        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) || (std == STD_EU3) ) &&
             (params.data()->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            fout5 << "Additional points:\n\n";

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Point[-]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "n[min-1]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Ne_n[kW]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gair[kg/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Gfuel[kg/h]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "alpha[-]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOm[g/kWh]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "gNOc[g/kWh]";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "diff[%]\n";

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber - 2);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 3];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 3];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 3];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 3];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 3];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx1m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx1c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx1 << "\n";

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber - 1);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 2];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 2];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 2];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 2];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 2];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx2m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx2c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx2 << "\n";

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << (TCyclePointsNumber);
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_n[TCyclePointsNumber - 1];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Ne_netto[TCyclePointsNumber - 1];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gair[TCyclePointsNumber - 1];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_Gfuel[TCyclePointsNumber - 1];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << array_alpha[TCyclePointsNumber - 1];
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx3m;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << gNOx3c;
            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision) << diffNOx3 << "\n";

            fout5 << "\n";
        }

        fout5 << "Specific emissions:\n\n";

        fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << " ";

        if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
             (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
             (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new)";
        }
        else if (std == STD_GOST) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old1)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (old2)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new1)";
            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limit (new2)";
        }
        else {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Limitation";
        }

        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "CalcResult";

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
             (std != STD_F) && (std != STD_G1) && (std != STD_G2) && (std != STD_GOST) ) {

            fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "Conclusion";
        }

        fout5 << "\n";

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

        if ( (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) || (std == STD_R96K8) ||
             (std == STD_R96H5) || (std == STD_R96I5) || (std == STD_R96J5) || (std == STD_R96K5) ) {

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

            if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                 (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                ptrdiff_t std1 = std + 20; // old

                gNOxLimit1 = val_NOxLimit(std1);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                ptrdiff_t std2 = std + 10; // new

                gNOxLimit2 = val_NOxLimit(std2);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);
            }
            else if ( (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ) {

                ptrdiff_t std1 = std + 20; // old

                gNOxLimit1 = val_NOxLimit(std1, array_n[0]);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                ptrdiff_t std2 = std + 10; // new

                gNOxLimit2 = val_NOxLimit(std2, array_n[0]);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);
            }
            else if (std == STD_GOST) {

                ptrdiff_t std1 = std + 30; // ou

                gNOxLimit1 = val_NOxLimit(std1);
                gCOLimit1 = val_COLimit(std1);
                gCHLimit1 = val_CHLimit(std1);

                ptrdiff_t std2 = std + 40; // ol

                gNOxLimit2 = val_NOxLimit(std2);
                gCOLimit2 = val_COLimit(std2);
                gCHLimit2 = val_CHLimit(std2);

                ptrdiff_t std3 = std + 10; // nu

                gNOxLimit3 = val_NOxLimit(std3);
                gCOLimit3 = val_COLimit(std3);
                gCHLimit3 = val_CHLimit(std3);

                ptrdiff_t std4 = std + 20; // nl

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

            if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                 (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
                 (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit1;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit2;
            }
            else if (std == STD_GOST) {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit1;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit2;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit3;
                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit4;
            }
            else {

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOxLimit;
            }

            fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gNOx;

            if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                 (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                if ( (gNOx < gNOxLimit) || (gNOx == gNOxLimit) ) {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                }
                else {

                    fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                }
            }

            fout5 << "\n";

            if (gCOcalc) {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gCO[g/kWh]";

                if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                     (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
                     (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit2;
                }
                else if (std == STD_GOST) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit2;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit3;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit4;
                }
                else {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCOLimit;
                }

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCO;

                if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                     (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if ( (gCO < gCOLimit) || (gCO == gCOLimit) ) {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                    }
                    else {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                    }
                }

                fout5 << "\n";
            }

            if (gCHcalc) {

                fout5 << right << setw(WidthOfColumn-1+2) << setfill(' ') << "gCH[g/kWh]";

                if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                     (std == STD_E1) || (std == STD_E2) || (std == STD_E3) || (std == STD_E5) ||
                     (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit2;
                }
                else if (std == STD_GOST) {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit1;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit2;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit3;
                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit4;
                }
                else {

                    fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCHLimit;
                }

                fout5 << fixed << right << setw(WidthOfColumn-1) << setfill(' ') << setprecision(Precision+1) << gCH;

                if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                     (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) {

                    if ( (gCH < gCHLimit) || (gCH == gCHLimit) ) {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "OK";
                    }
                    else {

                        fout5 << right << setw(WidthOfColumn-1) << setfill(' ') << "failed";
                    }
                }

                fout5 << "\n";
            }
        }

        if ( (params.data()->val_PTcalc() != PTCALC_NO) && ( (std != STD_GOST) && (std != STD_OST) && (std != STD_EU0) && (std != STD_C1) &&
                                                             (std != STD_D1) && (std != STD_D2) &&
                                                             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) && (std != STD_E5) &&
                                                             (std != STD_F) && (std != STD_G1) && (std != STD_G2) ) ) {

            if (params.data()->val_PTcalc() == PTCALC_THROUGHPTMASS) {

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
            fout5 << fixed << right << setw(WidthOfColumn+WidthOfColumn-2) << setfill(' ') << setprecision(Precision+1) << gPTs << "\n";
        }

        fout5 << "\n";

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

        message += QString::fromAscii(Q_FUNC_INFO) + ":::" + "fout7 was not created!\n";
        qDebug() << Q_FUNC_INFO << ":::" << "fout7 was not created!";

        return message;
    }

    string paramValDelim = parameterValueDelimiter.toStdString();

    fout7 << "task"           << paramValDelim << params.data()->val_Task()                         << "\n";
    fout7 << "Vh"             << paramValDelim << params.data()->val_Vh()                           << "\n";
    fout7 << "standard"       << paramValDelim << params.data()->val_Standard()                     << "\n";
    fout7 << "FuelType"       << paramValDelim << params.data()->val_FuelType()                     << "\n";
    fout7 << "NOxSample"      << paramValDelim << params.data()->val_NOxSample()                    << "\n";
    fout7 << "PTcalc"         << paramValDelim << params.data()->val_PTcalc()                       << "\n";
    fout7 << "PTmass"         << paramValDelim << params.data()->val_PTmass()                       << "\n";
    fout7 << "AddPointsCalc"  << paramValDelim << params.data()->val_AddPointsCalc()                << "\n";
    fout7 << "CalcConfigFile" << paramValDelim << params.data()->val_CalcConfigFile().toStdString() << "\n";

    fout7.close();

    message += "libtoxic: Calculation config file \"" + QString::fromStdString(CalcConfigFileName) + "\" created.\n";
    qDebug() << "libtoxic: Calculation config file" << QString::fromStdString(CalcConfigFileName) << "created.";

    //

    if (std != STD_FREECALC) {

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

        ptrdiff_t ptcalc = params.data()->val_PTcalc();

        if (ptcalc != PTCALC_NO) {

            if (ptcalc == PTCALC_THROUGHPTMASS) {

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

QString CycleEmissions::lastReportsDir() const {

    return fullReportsPath;
}
