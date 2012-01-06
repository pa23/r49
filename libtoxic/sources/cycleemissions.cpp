/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: cycleemissions.cpp

    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>

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
#include "emissionlimits.h"
#include "precalc.h"
#include "r49.h"
#include "toxicerror.h"

#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include <cmath>

CycleEmissions::CycleEmissions(const QSharedPointer<LibtoxicParameters> &prms,
                               const QSharedPointer<CommonParameters> &cfg) :
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

        try {

            params->readCalcConfigFile(params->val_CalcConfigFile());
        }
        catch(ToxicError &toxerr) {

            throw;
        }
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

void CycleEmissions::readCSV(const QVector< QVector<double> > &data) {

    if ( data.isEmpty() ) {

        QString filenamePoints = config->val_filenamePoints();

        QSharedPointer<csvRead>
                readerDataForCalc(
                    new csvRead(filenamePoints, " ", STRSNUMBERFORCOLUMNCAPTION)
                    );

        try {

            readerDataForCalc->readFile();
        }
        catch(ToxicError &toxerr) {

            throw;
        }

        array_DataForCalc = readerDataForCalc->csvData();

        if ( array_DataForCalc.isEmpty() ) {

            throw ToxicError("No data to calculate!");
        }

        if ( array_DataForCalc[0].size() != POINTSFILECOLUMNSNUMBER ) {

            throw ToxicError("Incorrect source data!");
        }
    }
    else {

        array_DataForCalc = data;
    }

    NumberOfPoints = array_DataForCalc.size();

    ptrdiff_t std = params->val_Standard();
    ptrdiff_t addpc = params->val_AddPointsCalc();

    if (
            (
                (
                    (std == STD_EU6) || (std == STD_EU5) ||
                    (std == STD_EU4) || (std == STD_EU3)
                    ) &&
                (
                    (addpc == ADDPOINTSCALC_YES)
                    ) &&
                (
                    (NumberOfPoints != TCYCLEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_EU6) || (std == STD_EU5) ||
                    (std == STD_EU4) || (std == STD_EU3)
                    ) &&
                (
                    (addpc == ADDPOINTSCALC_NO)
                    ) &&
                (
                    (NumberOfPoints != (TCYCLEPOINTSNUMBER-
                                        TCYCLEADDPOINTSNUMBER))
                    )
                ) ||
            (
                (
                    (std == STD_EU2) || (std == STD_EU1) ||
                    (std == STD_EU0) || (std == STD_OST) ||
                    (std == STD_GOST)
                    ) &&
                (
                    (NumberOfPoints != (TCYCLEPOINTSNUMBER-
                                        TCYCLEADDPOINTSNUMBER))
                    )
                ) ||
            (
                (
                    (std == STD_R96E8) || (std == STD_R96F8) ||
                    (std == STD_R96G8) || (std == STD_R96D8) ||
                    (std == STD_R96H8) || (std == STD_R96I8) ||
                    (std == STD_R96J8) || (std == STD_R96K8)
                    ) &&
                (
                    (NumberOfPoints != ECYCLEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_R96E5) || (std == STD_R96F5) ||
                    (std == STD_R96G5) || (std == STD_R96D5) ||
                    (std == STD_R96H5) || (std == STD_R96I5) ||
                    (std == STD_R96J5) || (std == STD_R96K5)
                    ) &&
                (
                    (NumberOfPoints != FCYCLEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_C1)
                    ) &&
                (
                    (NumberOfPoints != GC1CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_D1)
                    ) &&
                (
                    (NumberOfPoints != GD1CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_D2)
                    ) &&
                (
                    (NumberOfPoints != GD2CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_E1)
                    ) &&
                (
                    (NumberOfPoints != GE1CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_E2)
                    ) &&
                (
                    (NumberOfPoints != GE2CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_E3)
                    ) &&
                (
                    (NumberOfPoints != GE3CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_E5)
                    ) &&
                (
                    (NumberOfPoints != GE5CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_F)
                    ) &&
                (
                    (NumberOfPoints != GFCYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_G1)
                    ) &&
                (
                    (NumberOfPoints != GG1CYLCEPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (std == STD_G2)
                    ) &&
                (
                    (NumberOfPoints != GG2CYLCEPOINTSNUMBER)
                    )
                )
            ) {

        throw ToxicError("Wrong source data or calculation settings! "
                         "Check the number of points of source data and "
                         "calculation parameters.");
    }

    array_n.clear();         array_n.resize(NumberOfPoints);
    array_Me_brutto.clear(); array_Me_brutto.resize(NumberOfPoints);
    array_Ne_brutto.clear(); array_Ne_brutto.resize(NumberOfPoints);
    array_N_fan.clear();     array_N_fan.resize(NumberOfPoints);
    array_w.clear();         array_w.resize(NumberOfPoints);
    array_t0.clear();        array_t0.resize(NumberOfPoints);
    array_B0.clear();        array_B0.resize(NumberOfPoints);
    array_Ra.clear();        array_Ra.resize(NumberOfPoints);
    array_dPn.clear();       array_dPn.resize(NumberOfPoints);
    array_Gair.clear();      array_Gair.resize(NumberOfPoints);
    array_Gfuel.clear();     array_Gfuel.resize(NumberOfPoints);
    array_CNOx.clear();      array_CNOx.resize(NumberOfPoints);
    array_gNOx.clear();      array_gNOx.resize(NumberOfPoints);
    array_CCO.clear();       array_CCO.resize(NumberOfPoints);
    array_CCH.clear();       array_CCH.resize(NumberOfPoints);
    array_CCO2in.clear();    array_CCO2in.resize(NumberOfPoints);
    array_CCO2out.clear();   array_CCO2out.resize(NumberOfPoints);
    array_CO2.clear();       array_CO2.resize(NumberOfPoints);
    array_Ka1m.clear();      array_Ka1m.resize(NumberOfPoints);
    array_KaPerc.clear();    array_KaPerc.resize(NumberOfPoints);
    array_FSN.clear();       array_FSN.resize(NumberOfPoints);
    array_Pr.clear();        array_Pr.resize(NumberOfPoints);
    array_ts.clear();        array_ts.resize(NumberOfPoints);
    array_tauf.clear();      array_tauf.resize(NumberOfPoints);
    array_qmdw.clear();      array_qmdw.resize(NumberOfPoints);
    array_qmdew.clear();     array_qmdew.resize(NumberOfPoints);
    array_rd.clear();        array_rd.resize(NumberOfPoints);

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

    mytime = QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss");

    //

    if ( !nonZeroArray(array_n) ) {

        throw ToxicError("Array \"n\" must contain nonzero data!");
    }

    if ( nonZeroArray(array_Me_brutto) ) {

        NenCalcMethod = true;
    }
    else if ( nonZeroArray(array_Ne_brutto) ) {

        NenCalcMethod = false;
    }
    else {

        throw ToxicError("Arrays \"Me_b\" or \"Ne_b\" "
                         "must contain nonzero data!");
    }

    if ( !nonZeroArray(array_t0) ) {

        throw ToxicError("Array \"t0\" must contain nonzero data!");
    }

    if ( !nonZeroArray(array_B0) ) {

        throw ToxicError("Array \"B0\" must contain nonzero data!");
    }

    if ( !nonZeroArray(array_Ra) ) {

        throw ToxicError("Array \"Ra\" must contain nonzero data!");
    }

    if ( nonZeroArray(array_Gair) ) {

        GairVals = true;
    }
    else if ( nonZeroArray(array_dPn) ) {

        GairVals = false;
    }
    else {

        throw ToxicError("Arrays \"Gair\" or \"dPn\" "
                         "must contain nonzero data!");
    }

    if ( !nonZeroArray(array_Gfuel) ) {

        throw ToxicError("Array \"Gfuel\" must contain nonzero data!");
    }

    if ( nonZeroArray(array_CNOx) ) {

        NOxCalcMethod = true;
    }
    else if ( nonZeroArray(array_gNOx) ) {

        NOxCalcMethod = false;
    }
    else {

        throw ToxicError("Arrays \"C_NOx\" or \"gNOx\" "
                         "must contain nonzero data!");
    }

    if ( nonZeroArray(array_CCO) ) {

        gCOcalc = true;
    }
    else {

        gCOcalc = false;
    }

    if ( nonZeroArray(array_CCH) ) {

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

    if ( nonZeroArray(array_CO2) ) {

        CheckMeas = true;
    }
    else {

        CheckMeas = false;
    }

    if ( params->val_PTcalc() != PTCALC_NO ) {

        if ( !nonZeroArray(array_Pr) || !nonZeroArray(array_ts) ||
             ( !nonZeroArray(array_Ka1m) &&
               !nonZeroArray(array_KaPerc) &&
               !nonZeroArray(array_FSN) ) ) {

            throw ToxicError("Arrays \"Pr\", \"ts\" and any of the arrays "
                             "\"Ka1m\", \"KaPerc\", \"FSN\" "
                             "must contain nonzero data!");
        }

        if ( params->val_PTcalc() == PTCALC_THROUGHPTMASS ) {

            if ( !nonZeroArray(array_tauf) || !nonZeroArray(array_qmdew) ||
                 ( !nonZeroArray(array_qmdw) &&
                   !nonZeroArray(array_rd) ) ) {

                throw ToxicError("Arrays \"tauf\", \"qmdew\" "
                                 "and any of the arrays "
                                 "\"qmdw\", \"rd\" "
                                 "must contain nonzero data!");
            }

            if ( nonZeroArray(array_qmdw) ) {

                qmdwVSrd = true;
            }
            else if ( nonZeroArray(array_rd) ) {

                qmdwVSrd = false;
            }
        }
    }

    if ( nonZeroArray(array_FSN) ) {

        smoke = 2;
    }
    else if ( nonZeroArray(array_Ka1m) ) {

        smoke = 0;
    }
    else if ( nonZeroArray(array_KaPerc) ) {

        smoke = 1;
    }
    else {

        smoke = -1;
    }
}

void CycleEmissions::calculate() {

    ptrdiff_t std = params->val_Standard();

    try {

        preCalculate();
        calculate_gNOx();

        if ( ( (std == STD_EU6) || (std == STD_EU5) ||
               (std == STD_EU4) || (std == STD_EU3) ) &&
             (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) &&
             (NumberOfPoints == TCYCLEPOINTSNUMBER) ) {

            calculateAdditionalPoints();
        }

        calculate_gCO();
        calculate_gCH();
        calculate_gPT();
        calculate_rEGR();

        if ( std != STD_FREECALC ) {

            calculate_Means();
        }

        compareAlpha();
    }
    catch(ToxicError &toxerr) {

        throw;
    }
}

void CycleEmissions::preCalculate() {

    array_Ne_netto.clear(); array_Ne_netto.resize(NumberOfPoints);
    array_Me_netto.clear(); array_Me_netto.resize(NumberOfPoints);
    array_alpha.clear();    array_alpha.resize(NumberOfPoints);
    array_alpha_O2.clear(); array_alpha_O2.resize(NumberOfPoints);
    array_Gexh.clear();     array_Gexh.resize(NumberOfPoints);
    array_Gexhd.clear();    array_Gexhd.resize(NumberOfPoints);
    array_Pb.clear();       array_Pb.resize(NumberOfPoints);
    array_Pa.clear();       array_Pa.resize(NumberOfPoints);
    array_Ha.clear();       array_Ha.resize(NumberOfPoints);
    array_Gaird.clear();    array_Gaird.resize(NumberOfPoints);
    array_Kw2.clear();      array_Kw2.resize(NumberOfPoints);
    array_Ffh.clear();      array_Ffh.resize(NumberOfPoints);
    array_Kf.clear();       array_Kf.resize(NumberOfPoints);
    array_Kwr.clear();      array_Kwr.resize(NumberOfPoints);
    array_Khd.clear();      array_Khd.resize(NumberOfPoints);
    array_fa.clear();       array_fa.resize(NumberOfPoints);
    array_ge.clear();       array_ge.resize(NumberOfPoints);

    ptrdiff_t std = params->val_Standard();

    double L0    = config->val_L0();
    double WH    = config->val_WH();
    double WO2   = config->val_WO2();
    double WN    = config->val_WN();
    double roAir = config->val_roAir();

    double Ffw = 0;
    double Ffd = 0;

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
         (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        ptrdiff_t FuelType = params->val_FuelType();

        if ( FuelType == FUELTYPE_DIESEL ) {

            Ffw = 0.75;
            Ffd = -0.77;
        }
        else if ( FuelType == FUELTYPE_MOTOR ) {

            Ffw = 0.72;
            Ffd = -0.74;
        }
        else if ( FuelType == FUELTYPE_MAZUT ) {

            Ffw = 0.69;
            Ffd = -0.71;
        }
        else {

            throw ToxicError("Wrong fuel type!");
        }
    }

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        if ( NenCalcMethod ) {

            array_Ne_brutto[i] = array_Me_brutto[i] * array_n[i] / 9550.0;
        }
        else if ( !NenCalcMethod ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
             (std == STD_EU3) ||
             (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) ||
             (std == STD_FREECALC) ) {

            array_Ne_netto[i] = array_Ne_brutto[i] - array_N_fan[i];
        }
        else {

            array_Ne_netto[i] = array_Ne_brutto[i];
        }

        array_Me_netto[i] = array_Ne_netto[i] * 9550.0 / array_n[i];

        if ( !GairVals ) {

            double Dn = config->val_Dn();

            if ( Dn < 1 ) {

                throw ToxicError("Wrong \"Dn\" parameter!");
            }

            array_Gair[i] = Gair(Dn, array_B0[i], array_t0[i],
                                 array_dPn[i]);
        }

        array_alpha[i] = array_Gair[i] / (array_Gfuel[i] * L0);

        if ( CheckMeas ) {

            double ConcO2air = config->val_ConcO2air();

            if ( ConcO2air < 1 ) {

                throw ToxicError("Wrong \"ConcO2air\" parameter!");
            }

            if ( !EGRcalc ) {

                array_alpha_O2[i] = (0.01 * ConcO2air *
                                     (1.0 - 0.01 * array_CO2[i]) +
                                     0.0683185 * 0.01 * array_CO2[i]) /
                        (0.01 * ConcO2air - 0.01 * array_CO2[i]);
            }
        }

        if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
             (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
             (std == STD_E5) ||
             (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

            array_Gexh[i] = array_Gair[i] / roAir + Ffw * array_Gfuel[i];
            array_Gexhd[i] = array_Gair[i] / roAir + Ffd * array_Gfuel[i];
        }
        else {

            array_Gexh[i] = array_Gair[i] + array_Gfuel[i];
        }

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
             (std != STD_E5) &&
             (std != STD_F ) && (std != STD_G1) && (std != STD_G2) ) {

            array_Pb[i] = array_B0[i];
            array_Pa[i] = val_Pa(array_t0[i]);

            array_Ha[i] = 6.22 * array_Ra[i] * array_Pa[i] /
                    (array_Pb[i] - array_Pa[i] * array_Ra[i] * 0.01);

            if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
                 (std == STD_EU3) || (std == STD_FREECALC) ||
                 (std == STD_R96E8) || (std == STD_R96F8) ||
                 (std == STD_R96G8) || (std == STD_R96D8) ||
                 (std == STD_R96E5) || (std == STD_R96F5) ||
                 (std == STD_R96G5) || (std == STD_R96D5) ||
                 (std == STD_R96H8) || (std == STD_R96I8) ||
                 (std == STD_R96J8) || (std == STD_R96K8) ||
                 (std == STD_R96H5) || (std == STD_R96I5) ||
                 (std == STD_R96J5) || (std == STD_R96K5) ) {

                array_Gaird[i] = (1.0 - array_Ha[i] / 1000.0) * array_Gair[i];
            }
            else if ( (std == STD_EU2) || (std == STD_EU1) ||
                      (std == STD_EU0) || (std == STD_OST) ||
                      (std == STD_GOST) ) {

                array_Gaird[i] = array_Gair[i] / (1.0 + array_Ha[i] / 1000.0);
            }
            else {

                throw ToxicError("Wrong calculation settings!");
            }

            if ( std != STD_OST ) {

                array_Kw2[i] = 1.608 * array_Ha[i] /
                        (1000.0 + 1.608 * array_Ha[i]);

                array_Ffh[i] = 1.969 / (1.0 + array_Gfuel[i] / array_Gair[i]);
                array_Kf[i] = 0.055594 * WH + 0.0080021 * WN + 0.0070046 * WO2;
            }

            if ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
                 (std == STD_EU3) || (std == STD_FREECALC) ) {

                array_Kwr[i] = (1.0 - (1.2442 * array_Ha[i] + 111.19 * WH *
                                       array_Gfuel[i] / array_Gaird[i]) /
                                (773.4 + 1.2442 * array_Ha[i] +
                                 array_Gfuel[i] / array_Gaird[i] *
                                 array_Kf[i] * 1000.0)) * 1.008;

                array_Khd[i] = 1.0 / (1.0 - 0.0182 * (array_Ha[i] - 10.71) +
                                      0.0045 * (array_t0[i] + 273.0 - 298.0));
            }
            else if ( (std == STD_R96E8) || (std == STD_R96F8) ||
                      (std == STD_R96G8) || (std == STD_R96D8) ||
                      (std == STD_R96E5) || (std == STD_R96F5) ||
                      (std == STD_R96G5) || (std == STD_R96D5) ||
                      (std == STD_R96H8) || (std == STD_R96I8) ||
                      (std == STD_R96J8) || (std == STD_R96K8) ||
                      (std == STD_R96H5) || (std == STD_R96I5) ||
                      (std == STD_R96J5) || (std == STD_R96K5) ) {

                array_Kwr[i] = (1.0 - array_Ffh[i] * array_Gfuel[i] /
                                array_Gaird[i]) - array_Kw2[i];

                array_Khd[i] = 1.0 / (1.0 + (0.309 * array_Gfuel[i] /
                                             array_Gaird[i] - 0.0266) *
                                      (array_Ha[i] - 10.71) +
                                      (-0.209 * array_Gfuel[i] /
                                       array_Gaird[i] + 0.00954) *
                                      (array_t0[i] + 273.0 - 298.0));
            }
            else if ( (std == STD_EU2) || (std == STD_EU1) ||
                      (std == STD_EU0) ) {

                array_Kwr[i] = 1.0 - 1.85 * array_Gfuel[i] / array_Gaird[i];

                array_Khd[i] = 1.0 / (1.0 + (0.044 * array_Gfuel[i] /
                                             array_Gaird[i] - 0.0038) *
                                      (7.0 * array_Ha[i] - 75.0) +
                                      (-0.116 * array_Gfuel[i] /
                                       array_Gaird[i] + 0.0053) * 1.8 *
                                      (array_t0[i] + 273.0 - 302.0));
            }
            else if ( std == STD_OST ) {

                array_Kwr[i] = 1.0 - 1.8 * array_Gfuel[i] / array_Gair[i];
            }
            else if ( std == STD_GOST ) {

                array_Kwr[i] = 1.0 - 1.85 * array_Gfuel[i] / array_Gair[i];

                array_Khd[i] = 1.0 / (1.0 + (0.044 * array_Gfuel[i] /
                                             array_Gair[i] - 0.0038) *
                                      (7.0 * array_Ha[i] - 75.0) +
                                      (-0.116 * array_Gfuel[i] / array_Gair[i] +
                                       0.0053) * 1.8 *
                                      (array_t0[i] + 273.0 - 302.0));
            }
            else {

                throw ToxicError("Wrong calculation settings!");
            }

            if ( params->val_ChargingType() == CHARGINGTYPE_NO ) {

                array_fa[i] = ( 99.0 / (array_Pb[i] - array_Ra[i] *
                                        array_Pa[i] * 0.01) ) *
                        pow( (array_t0[i] + 273.0) / 298, 0.7 );
            }
            else if ( params->val_ChargingType() ==
                      CHARGINGTYPE_GASTURBINE ) {

                array_fa[i] = pow( 99.0 / (array_Pb[i] - array_Ra[i] *
                                           array_Pa[i] * 0.01), 0.7 ) *
                        pow( (array_t0[i] + 273.0) / 298, 1.5 );
            }
            else {

                throw ToxicError("Wrong charging type!");
            }
        }

        array_ge[i] = array_Gfuel[i] / array_Ne_netto[i] * 1000.0;
    }
}

void CycleEmissions::calculate_gNOx() {

    array_mNOx.clear(); array_mNOx.resize(NumberOfPoints);

    double muNO2 = config->val_muNO2();

    ptrdiff_t std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
           (std == STD_EU3) ) &&
         (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
         (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        double summ_numerator = 0;
        double summ_denominator = 0;

        if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

            for ( ptrdiff_t i=0; i<n; i++ ) {

                summ_numerator += array_CNOx[i] /
                        10000.0 * array_Gexhd[i] * array_w[i];

                summ_denominator += array_Ne_brutto[i] /
                        array_Ne_brutto[0] * array_w[i];
            }
        }
        else {

            for ( ptrdiff_t i=0; i<n; i++ ) {

                summ_numerator += array_CNOx[i] /
                        10000.0 * array_Gexh[i] * array_w[i];

                summ_denominator += array_Ne_brutto[i] /
                        array_Ne_brutto[0] * array_w[i];
            }
        }

        gNOx = 0.446 * muNO2 * summ_numerator /
                (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        double summ_mNOx = 0;
        double summ_Ne_netto = 0;

        if ( NOxCalcMethod ) {

            if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    if ( std == STD_OST ) {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] *
                                array_Kwr[i] * array_Gexh[i];
                    }
                    else {

                        array_mNOx[i] = 0.001587 * array_CNOx[i] *
                                array_Kwr[i] * array_Khd[i] * array_Gexh[i];
                    }

                    array_gNOx[i] = array_mNOx[i] / array_Ne_netto[i];

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    if ( std == STD_OST ) {

                        array_mNOx[i] = 0.001587 *
                                array_CNOx[i] * array_Gexh[i];
                    }
                    else {

                        array_mNOx[i] = 0.001587 *
                                array_CNOx[i] * array_Khd[i] * array_Gexh[i];
                    }

                    array_gNOx[i] = array_mNOx[i] / array_Ne_netto[i];

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
        }
        else if ( !NOxCalcMethod ) {

            if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    array_mNOx[i] = array_gNOx[i] * array_Ne_netto[i];

                    if ( std == STD_OST ) {

                        array_CNOx[i] = array_mNOx[i] /
                                (0.001587 * array_Kwr[i] * array_Gexh[i]);
                    }
                    else {

                        array_CNOx[i] = array_mNOx[i] /
                                (0.001587 * array_Kwr[i] * array_Khd[i] *
                                 array_Gexh[i]);
                    }

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    array_mNOx[i] = array_gNOx[i] * array_Ne_netto[i];

                    if ( std == STD_OST ) {

                        array_CNOx[i] = array_mNOx[i] /
                                (0.001587 * array_Gexh[i]);
                    }
                    else {

                        array_CNOx[i] = array_mNOx[i] /
                                (0.001587 * array_Khd[i] * array_Gexh[i]);
                    }

                    summ_mNOx += array_mNOx[i] * array_w[i];
                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
        }

        gNOx = summ_mNOx / summ_Ne_netto;
    }
}

void CycleEmissions::calculateAdditionalPoints() {

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

    ptrdiff_t std = params->val_Standard();

    for ( ptrdiff_t i=(NumberOfPoints - TCYCLEADDPOINTSNUMBER);
          i<NumberOfPoints; i++ ) {

        if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

            if ( std == STD_OST ) {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] *
                        array_Gexh[i];
            }
            else {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Kwr[i] *
                        array_Khd[i] * array_Gexh[i];
            }
        }
        else {

            if ( std == STD_OST ) {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Gexh[i];
            }
            else {

                array_mNOx[i] = 0.001587 * array_CNOx[i] * array_Khd[i] *
                        array_Gexh[i];
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
}

void CycleEmissions::calculate_gCO() {

    array_mCO.clear(); array_mCO.resize(NumberOfPoints);
    array_gCO.clear(); array_gCO.resize(NumberOfPoints);

    double muCO = config->val_muCO();

    ptrdiff_t std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
           (std == STD_EU3) ) &&
         (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
         (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        double summ_numerator = 0;
        double summ_denominator = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            summ_numerator += array_CCO[i] /
                    10000.0 * array_Gexhd[i] * array_w[i];

            summ_denominator += array_Ne_brutto[i] /
                    array_Ne_brutto[0] * array_w[i];
        }

        gCO = 0.446 * muCO * summ_numerator /
                (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        double summ_mCO = 0;
        double summ_Ne_netto = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            array_mCO[i] = 0.000966 * array_CCO[i] * array_Kwr[i] *
                    array_Gexh[i]; // always is dry

            array_gCO[i] = array_mCO[i] / array_Ne_netto[i];

            summ_mCO += array_mCO[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gCO = summ_mCO / summ_Ne_netto;
    }
}

void CycleEmissions::calculate_gCH() {

    array_mCH.clear(); array_mCH.resize(NumberOfPoints);
    array_gCH.clear(); array_gCH.resize(NumberOfPoints);

    double muCH = config->val_muCH();

    ptrdiff_t std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
           (std == STD_EU3) ) &&
         (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
    }
    else {

        n = NumberOfPoints;
    }

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
         (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        double summ_numerator = 0;
        double summ_denominator = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            summ_numerator += array_CCH[i] /
                    10000.0 * array_Gexh[i] * array_w[i];

            summ_denominator += array_Ne_brutto[i] /
                    array_Ne_brutto[0] * array_w[i];
        }

        gCH = 0.446 * muCH * summ_numerator /
                (array_Ne_brutto[0] * summ_denominator);
    }
    else {

        double summ_mCH = 0;
        double summ_Ne_netto = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            if ( std == STD_OST ) {

                array_mCH[i] = 0.000485 * array_CCH[i] *
                        array_Gexh[i]; // always is wet
            }
            else {

                array_mCH[i] = 0.000479 * array_CCH[i] *
                        array_Gexh[i]; // always is wet
            }

            array_gCH[i] = array_mCH[i] / array_Ne_netto[i];

            summ_mCH += array_mCH[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gCH = summ_mCH / summ_Ne_netto;
    }
}

void CycleEmissions::calculate_gPT() {

    mf = params->val_PTmass();

    array_ror.clear();   array_ror.resize(NumberOfPoints);
    array_CPT.clear();   array_CPT.resize(NumberOfPoints);
    array_qmedf.clear(); array_qmedf.resize(NumberOfPoints);
    array_msepi.clear(); array_msepi.resize(NumberOfPoints);
    array_mPT.clear();   array_mPT.resize(NumberOfPoints);
    array_gPT.clear();   array_gPT.resize(NumberOfPoints);

    ptrdiff_t std = params->val_Standard();

    if ( (params->val_PTcalc() != PTCALC_NO) &&
         ( (std != STD_OST) && (std != STD_GOST) && (std != STD_EU0) &&
           (std != STD_C1)  && (std != STD_D1)   && (std != STD_D2)  &&
           (std != STD_E1)  && (std != STD_E2)   && (std != STD_E3)  &&
           (std != STD_E5)  &&
           (std != STD_F )  && (std != STD_G1)   && (std != STD_G2) ) ) {

        ptrdiff_t n = 0;
        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
               (std == STD_EU3) ) &&
             (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
        }
        else {

            n = NumberOfPoints;
        }

        double L = config->val_L();
        double Rr = config->val_Rr();

        double summ_mPT = 0;
        double summ_Ne_netto = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            if ( smoke == 0 ) {

                array_KaPerc[i] = Ka1m2KaPerc(array_Ka1m[i], L);
                array_FSN[i] = (6.6527E-017)          *pow(array_KaPerc[i],10)+
                        (-0.000000000000026602)*pow(array_KaPerc[i], 9)+
                        (0.0000000000040987)   *pow(array_KaPerc[i], 8)+
                        (-0.00000000026927)    *pow(array_KaPerc[i], 7)+
                        (0.00000000040933)     *pow(array_KaPerc[i], 6)+
                        (0.0000010658)         *pow(array_KaPerc[i], 5)+
                        (-0.000069165)         *pow(array_KaPerc[i], 4)+
                        (0.0020088)            *pow(array_KaPerc[i], 3)+
                        (-0.028758)            *pow(array_KaPerc[i], 2)+
                        (0.26502)              *pow(array_KaPerc[i], 1)+
                        (0.0087517)            *pow(array_KaPerc[i], 0);
            }
            else if (smoke == 1) {

                array_FSN[i] = (6.6527E-017)          * pow(array_KaPerc[i],10)+
                        (-0.000000000000026602)* pow(array_KaPerc[i], 9)+
                        (0.0000000000040987)   * pow(array_KaPerc[i], 8)+
                        (-0.00000000026927)    * pow(array_KaPerc[i], 7)+
                        (0.00000000040933)     * pow(array_KaPerc[i], 6)+
                        (0.0000010658)         * pow(array_KaPerc[i], 5)+
                        (-0.000069165)         * pow(array_KaPerc[i], 4)+
                        (0.0020088)            * pow(array_KaPerc[i], 3)+
                        (-0.028758)            * pow(array_KaPerc[i], 2)+
                        (0.26502)              * pow(array_KaPerc[i], 1)+
                        (0.0087517)            * pow(array_KaPerc[i], 0);
            }

            array_ror[i] = (array_Pb[i] + array_Pr[i]) * 1000.0 / Rr /
                    (array_ts[i] + 273.0);

            array_CPT[i] = (-184.0 * array_FSN[i] - 727.5) *
                    log10(1.0 - array_FSN[i] / 10.0);

            array_mPT[i] = array_CPT[i] * array_Gexh[i] / array_ror[i] / 1000.0;
            array_gPT[i] = array_mPT[i] / array_Ne_netto[i];

            summ_mPT += array_mPT[i] * array_w[i];
            summ_Ne_netto += array_Ne_netto[i] * array_w[i];
        }

        gPTs = summ_mPT / summ_Ne_netto;

        summ_mPT = 0;
        summ_Ne_netto = 0;

        if ( params->val_PTcalc() == PTCALC_THROUGHPTMASS ) {

            if ( mf == 0 ) {

                throw ToxicError("Wrong \"gPT\" parameter!");
            }

            if ( qmdwVSrd ) {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    array_rd[i] = array_qmdew[i] /
                            (array_qmdew[i] - array_qmdw[i]);

                    array_qmedf[i] = array_Gexh[i] * array_rd[i];
                    qmedfl += array_qmedf[i] * array_w[i];
                    array_msepi[i] = array_qmdew[i] * array_tauf[i];
                    msep += array_msepi[i];

                    summ_Ne_netto += array_Ne_netto[i] * array_w[i];
                }
            }
            else if ( !qmdwVSrd ) {

                for ( ptrdiff_t i=0; i<n; i++ ) {

                    array_qmdw[i] = (array_qmdew[i] * array_rd[i] -
                                     array_qmdew[i]) / array_rd[i];

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
}

void CycleEmissions::calculate_rEGR() {

    array_rEGR.clear();      array_rEGR.resize(NumberOfPoints);
    array_alpha_res.clear(); array_alpha_res.resize(NumberOfPoints);

    double CCO2air = config->val_ConcCO2air();

    if ( EGRcalc ) {

        for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

            array_rEGR[i] = (array_CCO2in[i] - CCO2air) /
                    (array_CCO2out[i] - CCO2air) * 100.0;

            array_alpha_res[i] = (array_alpha[i] - array_alpha[i] *
                                  array_rEGR[i] / 100.0 -
                                  array_rEGR[i] / 100.0) /
                    (1.0 - 2.0 * array_rEGR[i] / 100.0);
        }

    }
}

void CycleEmissions::calculate_Means() {

    double summ_Gfuel    = 0;
    double summ_Ne_netto = 0;

    double summ_t0 = 0;
    double summ_B0 = 0;
    double summ_Ra = 0;

    ptrdiff_t std = params->val_Standard();

    ptrdiff_t n = 0;
    if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
           (std == STD_EU3) ) &&
         (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

        n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
    }
    else {

        n = NumberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

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
}

void CycleEmissions::compareAlpha() {

    array_diff_alpha.clear(); array_diff_alpha.resize(NumberOfPoints);

    if ( CheckMeas ) {

        if ( EGRcalc ) {

            double ConcO2mix = 0;
            double ConcO2air = config->val_ConcO2air();

            if ( ConcO2air < 1 ) {

                throw ToxicError("Wrong \"ConcO2air\" parameter!");
            }

            for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

                ConcO2mix = ConcO2air * (1.0 - array_rEGR[i] / 100.0) +
                        array_CO2[i] * array_rEGR[i] / 100.0;

                array_alpha_O2[i] = (0.01 * ConcO2mix *
                                     (1.0 - 0.01 * array_CO2[i]) + 0.0683185 *
                                     0.01 * array_CO2[i]) /
                        (0.01 * ConcO2mix - 0.01 * array_CO2[i]);

                array_diff_alpha[i] = (array_alpha_res[i] - array_alpha_O2[i]) /
                        array_alpha_O2[i] * 100.0;
            }
        }
        else {

            for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

                array_diff_alpha[i] = (array_alpha[i] - array_alpha_O2[i]) /
                        array_alpha_O2[i] * 100.0;
            }
        }

    }
}

bool CycleEmissions::checkTestConditions() const {

    for ( ptrdiff_t i=0; i<array_fa.count(); i++ ) {

        if ( (array_fa[i] < 0.96) || (array_fa[i] > 1.06) ) {

            return false;
        }
    }

    return true;
}

QString CycleEmissions::createReports(const bool &createrepdir) {

    QString message = "";
    QString testcondres = "?";

    if ( checkTestConditions() ) {

        testcondres = "Check test conditions: OK.";
    }
    else {

        testcondres = "Check test conditions: FAILED.";
    }

    if ( !createrepdir ) {

        message += "\ngNOx = " + QString::number(gNOx) + " g/kWh\n";

        if ( gCOcalc ) {

            message += "gCO = " + QString::number(gCO) + " g/kWh\n";
        }

        if ( gCHcalc ) {

            message += "gCH = " + QString::number(gCH) + " g/kWh\n";
        }

        ptrdiff_t ptcalc = params->val_PTcalc();

        if ( ptcalc != PTCALC_NO ) {

            if ( ptcalc == PTCALC_THROUGHPTMASS ) {

                message += "gPT = " + QString::number(gPT) + " g/kWh\n";
            }

            message += "gPTs = " + QString::number(gPTs) + " g/kWh\n";
        }

        message += "\n" + testcondres + "\n";

        //

        return message;
    }

    ptrdiff_t std = params->val_Standard();

    QString dirnameReports = config->val_dirnameReports();

    fullReportsPath = dirnameReports + "/" +
            params->defStandardName(std) + "_" + mytime;

    QDir reportdir;
    reportdir.mkdir(fullReportsPath);

    //

    QString checkoutDataFileName = "CheckoutData_" + mytime + ".csv";

    QString checkoutdata = reportdir.relativeFilePath(fullReportsPath) +
            reportdir.separator() + checkoutDataFileName;

    QFile data1(checkoutdata);

    if ( !data1.open(QFile::WriteOnly) ) {

        throw ToxicError("Can not open file " + checkoutdata + "!");
    }

    QTextStream fout1(&data1);

    fout1 << right << qSetFieldWidth(WIDTHOFCOLUMN);

    fout1 << "Point[-]"   << "n[min-1]"    << "Me_b[Nm]"
          << "Ne_b[kW]"   << "N_fan[kW]"   << "w[-]"
          << "t0[oC]"     << "B0[kPa]"     << "Ra[%]"
          << "dPn[mmH2O]" << "Gair[kg/h]"  << "Gfuel[kg/h]"
          << "C_NOx[ppm]" << "gNOx[g/kWh]" << "C_CO[ppm]"
          << "C_CH[ppm]"  << "C_CO2in[%]"  << "C_CO2out[%]"
          << "C_O2[%]"    << "Ka[m-1]"     << "Ka[%]"
          << "FSN[-]"     << "Pr[kPa]"     << "ts[oC]"
          << "tauf[s]"    << "qmdw[g/s]"   << "qmdew[g/s]"
          << "rd[-]"      << "Ne_n[kW]"    << "Me_n[Nm]"
          << "alpha[-]"   << "alpha_O2[-]";

    if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
         (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
         (std == STD_E5) ||
         (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        fout1 << "Gexh[m3/h]" << "Gexhd[m3/h]";
    }
    else {

        fout1 << "Gexh[kg/h]" << "Gexhd[kg/h]";
    }

    fout1 << "Pb[kPa]"     << "Pa[kPa]"    << "Ha[g/kg]"
          << "Gaird[kg/h]" << "Kw2[-]"     << "Ffh[-]"
          << "Kf[-]"       << "Kwr[-]"     << "Khd[-]"
          << "fa[-]"       << "ge[g/kWh]"  << "mNOx[g/h]"
          << "mCO[g/h]"    << "gCO[g/kWh]" << "mCH[g/h]"
          << "gCH[g/kWh]"  << "ror[kg/m3]" << "CPT[mg/m3]"
          << "mPT[g/h]"    << "gPT[g/kWh]" << "qmedf[kg/h]"
          << "msepi[g]"    << "rEGR[%]"    << "alpha_res[-]"
          << "diff_alpha[%]";

    fout1.reset();

    fout1 << "\n";

    ptrdiff_t prec = PRECISION + 1;

    fout1 << fixed << right;

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        fout1 << qSetFieldWidth(WIDTHOFCOLUMN) << qSetRealNumberPrecision(0)
              << (i + 1) << array_n[i];

        fout1 << qSetRealNumberPrecision(prec)
              << array_Me_brutto[i] << array_Ne_brutto[i] << array_N_fan[i]
              << array_w[i]         << array_t0[i]        << array_B0[i]
              << array_Ra[i]        << array_dPn[i]       << array_Gair[i]
              << array_Gfuel[i]     << array_CNOx[i]      << array_gNOx[i]
              << array_CCO[i]       << array_CCH[i]       << array_CCO2in[i]
              << array_CCO2out[i]   << array_CO2[i]       << array_Ka1m[i]
              << array_KaPerc[i]    << array_FSN[i]       << array_Pr[i]
              << array_ts[i]        << array_tauf[i]      << array_qmdw[i]
              << array_qmdew[i]     << array_rd[i]        << array_Ne_netto[i]
              << array_Me_netto[i]  << array_alpha[i]     << array_alpha_O2[i]
              << array_Gexh[i]      << array_Gexhd[i]     << array_Pb[i]
              << array_Pa[i]        << array_Ha[i]        << array_Gaird[i]
              << array_Kw2[i]       << array_Ffh[i]       << array_Kf[i]
              << array_Kwr[i]       << array_Khd[i]       << array_fa[i]
              << array_ge[i]        << array_mNOx[i]      << array_mCO[i]
              << array_gCO[i]       << array_mCH[i]       << array_gCH[i]
              << array_ror[i]       << array_CPT[i]       << array_mPT[i]
              << array_gPT[i]       << array_qmedf[i]     << array_msepi[i]
              << array_rEGR[i]      << array_alpha_res[i] << array_diff_alpha[i]
              << qSetFieldWidth(0) << "\n";
    }

    data1.close();

    message += "libtoxic: Additional file \"" +
            checkoutDataFileName + "\" created.\n";

    //

    QString sourceDataFileName;

    if ( (std == STD_R96E8) || (std == STD_R96F8) ||
         (std == STD_R96G8) || (std == STD_R96D8) ||
         (std == STD_R96E5) || (std == STD_R96F5) ||
         (std == STD_R96G5) || (std == STD_R96D5) ||
         (std == STD_R96H8) || (std == STD_R96I8) ||
         (std == STD_R96J8) || (std == STD_R96K8) ||
         (std == STD_R96H5) || (std == STD_R96I5) ||
         (std == STD_R96J5) || (std == STD_R96K5) ) {

        sourceDataFileName = "SourceData96_" + mytime + ".csv";
    }
    else if ( std == STD_OST ) {

        sourceDataFileName = "SourceDataOST_" + mytime + ".csv";
    }
    else if ( std == STD_GOST ) {

        sourceDataFileName = "SourceDataGOST_" + mytime + ".csv";
    }
    else if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
              (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
              (std == STD_E5) ||
              (std == STD_F ) || (std == STD_G1) || (std == STD_G2) ) {

        sourceDataFileName = "SourceDataGOST51249_" + mytime + ".csv";
    }
    else {

        sourceDataFileName = "SourceData49_" + mytime + ".csv";
    }

    QString srcdata = reportdir.relativeFilePath(fullReportsPath) +
            reportdir.separator() + sourceDataFileName;

    QFile data4(srcdata);

    if ( !data4.open(QFile::WriteOnly) ) {

        throw ToxicError("Can not open file " + srcdata + "!");
    }

    QTextStream fout4(&data4);

    fout4 << right << qSetFieldWidth(WIDTHOFCOLUMN);

    fout4 << "Point[-]"   << "n[min-1]"    << "Me_b[Nm]"
          << "Ne_b[kW]"   << "N_fan[kW]"   << "w[-]"
          << "t0[oC]"     << "B0[kPa]"     << "Ra[%]"
          << "dPn[mmH2O]" << "Gair[kg/h]"  << "Gfuel[kg/h]"
          << "C_NOx[ppm]" << "gNOx[g/kWh]" << "C_CO[ppm]"
          << "C_CH[ppm]"  << "C_CO2in[%]"  << "C_CO2out[%]"
          << "C_O2[%]"    << "Ka[m-1]"     << "Ka[%]"
          << "FSN[-]"     << "Pr[kPa]"     << "ts[oC]"
          << "tauf[s]"    << "qmdw[g/s]"   << "qmdew[g/s]"
          << "rd[-]"
          << qSetFieldWidth(0) << "\n";

    fout4 << fixed << qSetFieldWidth(WIDTHOFCOLUMN)
          << qSetRealNumberPrecision(PRECISION+1);

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        fout4 << qSetFieldWidth(WIDTHOFCOLUMN);

        for ( ptrdiff_t j=0; j<POINTSFILECOLUMNSNUMBER; j++ ) {

            fout4 << array_DataForCalc[i][j];
        }

        fout4 << qSetFieldWidth(0) << "\n";
    }

    data4.close();

    message += "libtoxic: SourceData file \"" +
            sourceDataFileName + "\" created.\n";

    //

    if ( std != STD_FREECALC ) {

        if ( (params->val_PTcalc() == PTCALC_THROUGHPTMASS) &&
             ( (std != STD_OST) && (std != STD_GOST) && (std != STD_EU0) &&
               (std != STD_C1)  && (std != STD_D1)   && (std != STD_D2)  &&
               (std != STD_E1)  && (std != STD_E2)   && (std != STD_E3)  &&
               (std != STD_E5)  &&
               (std != STD_F )  && (std != STD_G1)   && (std != STD_G2) ) ) {

            QString reportFileNamePT;

            if ( (std == STD_R96E8) || (std == STD_R96F8) ||
                 (std == STD_R96G8) || (std == STD_R96D8) ||
                 (std == STD_R96E5) || (std == STD_R96F5) ||
                 (std == STD_R96G5) || (std == STD_R96D5) ||
                 (std == STD_R96H8) || (std == STD_R96I8) ||
                 (std == STD_R96J8) || (std == STD_R96K8) ||
                 (std == STD_R96H5) || (std == STD_R96I5) ||
                 (std == STD_R96J5) || (std == STD_R96K5) ) {

                reportFileNamePT = "Report96_PT_" + mytime + ".txt";
            }
            else {

                reportFileNamePT = "Report49_PT_" + mytime + ".txt";
            }

            QString reppt = reportdir.relativeFilePath(fullReportsPath) +
                    reportdir.separator() + reportFileNamePT;

            QFile data6(reppt);

            if ( !data6.open(QFile::WriteOnly) ) {

                throw ToxicError("Can not open file " + reppt + "!");
            }

            QTextStream fout6(&data6);

            fout6 << "\tr49 distribution version " << r49version;
            fout6 << "\t\tReport on cycle. "
                  << params->defStandardName(std) << "\t\tDateTime: "
                  << mytime << "\n\n";

            fout6 << "Engine                 : ...\n";
            fout6 << fixed << qSetRealNumberPrecision(PRECISION)
                  << "Environment parameters : t0_mean = " << t0Mean
                  << " oC; B0_mean = " << B0Mean << " kPa; Ra_mean = "
                  << RaMean << " %; " << testcondres << "\n";

            fout6 << "Calculation comments   : ";

            if ( !GairVals ) {

                fout6 << "Gair meas by nozzle (Dn = "
                      << config->val_Dn() << "); ";
            }
            else {

                fout6 << "direct Gair meas; ";
            }

            if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

                fout6 << "NOxSample type is dry; ";
            }
            else {

                fout6 << "NOxSample type is wet; ";
            }

            if ( params->val_PTcalc() == PTCALC_THROUGHPTMASS ) {

                fout6 << qSetRealNumberPrecision(PRECISION+1)
                      << "PT calc method is PT mass based (mf = " << mf
                      << " mg);\n";
            }
            else if ( params->val_PTcalc() == PTCALC_NO ) {

                fout6 << "PT was not calculated;\n";
            }
            else {

                fout6 << "PT calc method is smoke meas based;\n";
            }

            if ( CheckMeas ) {

                fout6 << qSetRealNumberPrecision(PRECISION)
                      << "                         "
                      << "concentrations meas checked (conc O2air = "
                      << config->val_ConcO2air() << " %)";
            }
            else {

                fout6 << "                         "
                      << "concentrations meas were not checked";
            }

            fout6 << "\n\n";

            fout6 << right << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Point[-]"   << "n[min-1]" << "Ne_n[kW]"
                  << "Gair[kg/h]" << "Gfuel[kg/h]";

            if ( EGRcalc ) { fout6 << "alpha_res[-]"; }
            else           { fout6 << "alpha[-]";     }

            if      ( smoke == 0 ) { fout6 << "Ka[m-1]"; }
            else if ( smoke == 1 ) { fout6 << "Ka[%]";   }
            else if ( smoke == 2 ) { fout6 << "FSN[-]";  }

            fout6 << "tauf[s]" << "qmdw[g/s]" << "qmdew[g/s]"
                  << qSetFieldWidth(0) << "\n";

            ptrdiff_t n = 0;
            if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
                   (std == STD_EU3) ) &&
                 (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

                n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
            }
            else {

                n = NumberOfPoints;
            }

            for ( ptrdiff_t i=0; i<n; i++ ) {

                fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                      << qSetRealNumberPrecision(0)
                      << (i + 1) << array_n[i];

                fout6 << qSetRealNumberPrecision(PRECISION) <<
                         array_Ne_netto[i] << array_Gair[i] << array_Gfuel[i];

                if ( EGRcalc ) { fout6 << array_alpha_res[i]; }
                else           { fout6 << array_alpha[i];     }

                if ( smoke == 0 ) {

                    fout6 << qSetRealNumberPrecision(PRECISION+1)
                          << array_Ka1m[i];
                }
                else if ( smoke == 1 ) {

                    fout6 << qSetRealNumberPrecision(PRECISION+1)
                          << array_KaPerc[i];
                }
                else if ( smoke == 2 ) {

                    fout6 << qSetRealNumberPrecision(PRECISION+1)
                          << array_FSN[i];
                }

                fout6 << qSetRealNumberPrecision(PRECISION) << array_tauf[i];

                fout6 << qSetRealNumberPrecision(PRECISION+1)
                      << array_qmdw[i] << array_qmdew[i]
                      << qSetFieldWidth(0) << "\n";
            }

            fout6 << "\n";

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Point[-]" << "rd[-]" << "qmedf[kg/h]"
                  << "msepi[g]"
                  << qSetFieldWidth(0) << "\n";

            for ( ptrdiff_t i=0; i<n; i++ ) {

                fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                      << qSetRealNumberPrecision(0)
                      << (i + 1);

                fout6 << qSetRealNumberPrecision(PRECISION+1) << array_rd[i];
                fout6 << qSetRealNumberPrecision(PRECISION)   << array_qmedf[i];
                fout6 << qSetRealNumberPrecision(PRECISION+1) << array_msepi[i];

                fout6 << qSetFieldWidth(0) << "\n";
            }

            fout6 << "\n";

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << " ";

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Limitation" << "CalcResult" << "Conclusion"
                  << qSetFieldWidth(0) << "\n";

            double gPTLimit = 0;
            gPTLimit = val_PTLimit(std);

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gPT[g/kWh]";

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1) << gPTLimit << gPT;

            if ( (gPT < gPTLimit) || (gPT == gPTLimit) ) {

                fout6 << "OK" << qSetFieldWidth(0) << "\n";
            }
            else {

                fout6 << "failed" << qSetFieldWidth(0) << "\n";
            }

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gPTs[g/kWh]";

            fout6 << qSetFieldWidth(WIDTHOFCOLUMN+WIDTHOFCOLUMN-2)
                  << qSetRealNumberPrecision(PRECISION+1)
                  << gPTs << qSetFieldWidth(0) << "\n\n";

            data6.close();

            message += "libtoxic: Report file \"" +
                    reportFileNamePT + "\" created.\n";
        }

        //

        QString reportFileNameGAS;

        if ( (std == STD_R96E8) || (std == STD_R96F8) ||
             (std == STD_R96G8) || (std == STD_R96D8) ||
             (std == STD_R96E5) || (std == STD_R96F5) ||
             (std == STD_R96G5) || (std == STD_R96D5) ||
             (std == STD_R96H8) || (std == STD_R96I8) ||
             (std == STD_R96J8) || (std == STD_R96K8) ||
             (std == STD_R96H5) || (std == STD_R96I5) ||
             (std == STD_R96J5) || (std == STD_R96K5) ) {

            reportFileNameGAS = "Report96_GAS_" + mytime + ".txt";
        }
        else if (std == STD_OST) {

            reportFileNameGAS = "ReportOST_GAS_" + mytime + ".txt";
        }
        else if (std == STD_GOST) {

            reportFileNameGAS = "ReportGOST_GAS_" + mytime + ".txt";
        }
        else if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                  (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                  (std == STD_E5) ||
                  (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            reportFileNameGAS = "ReportGOST51249_GAS_" + mytime + ".txt";
        }
        else {

            reportFileNameGAS = "Report49_GAS_" + mytime + ".txt";
        }

        QString repgas = reportdir.relativeFilePath(fullReportsPath) +
                reportdir.separator() + reportFileNameGAS;

        QFile data5(repgas);

        if ( !data5.open(QFile::WriteOnly) ) {

            throw ToxicError("Can not open file " + repgas + "!");
        }

        QTextStream fout5(&data5);

        fout5 << "r49 distribution version " << r49version;
        fout5 << "     Report on cycle. "
              << params->defStandardName(std)
              << "     DateTime: " << mytime << "\n\n";

        fout5 << "Engine                 : ...\n";
        fout5 << fixed << qSetRealNumberPrecision(PRECISION)
              << "Environment parameters : t0_mean = " << t0Mean
              <<  " oC; B0_mean = " << B0Mean << " kPa; Ra_mean = "
               << RaMean << " %; " << testcondres << "\n";

        fout5 << "Calculation comments   : ";

        if ( !GairVals ) {

            fout5 << "Gair meas by nozzle (Dn = " << config->val_Dn()
                  << "); ";
        }
        else {

            fout5 << "direct Gair meas; ";
        }

        if ( params->val_NOxSample() == NOXSAMPLE_DRY ) {

            fout5 << "NOxSample type is dry; ";
        }
        else {

            fout5 << "NOxSample type is wet; ";
        }

        if ( params->val_PTcalc() == PTCALC_THROUGHPTMASS ) {

            fout5 << qSetRealNumberPrecision(PRECISION+1)
                  << "PT calc method is PT mass based (mf = " << mf
                  << " mg);\n";
        }
        else if ( params->val_PTcalc() == PTCALC_NO ) {

            fout5 << "PT was not calculated;\n";
        }
        else {

            fout5 << "PT calc method is smoke meas based;\n";
        }

        if ( CheckMeas ) {

            fout5 << qSetRealNumberPrecision(PRECISION)
                  << "                         "
                  << "concentrations meas checked (conc O2air = "
                  << config->val_ConcO2air() << " %)";
        }
        else {

            fout5 << "                         "
                  << "concentrations meas were not checked";
        }

        fout5 << "\n\n";

        fout5 << right << qSetFieldWidth(WIDTHOFCOLUMN-1)
              << "Point[-]" << "n[min-1]" << "Ne_n[kW]"
              << "Gair[kg/h]" << "Gfuel[kg/h]";

        if ( EGRcalc ) { fout5 << "alpha_res[-]"; }
        else           { fout5 << "alpha[-]";     }

        if      ( smoke == 0 ) { fout5 << "Ka[m-1]"; }
        else if ( smoke == 1 ) { fout5 << "Ka[%]";   }
        else if ( smoke == 2 ) { fout5 << "FSN[-]";  }

        fout5 << "C_NOx[ppm]";

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
             (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
             (std != STD_G2) ) {

            fout5 << "mNOx[g/h]" << "gNOx[g/kWh]";
        }

        fout5 << qSetFieldWidth(0) << "\n";

        ptrdiff_t n = 0;
        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
               (std == STD_EU3) ) &&
             (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            n = NumberOfPoints - TCYCLEADDPOINTSNUMBER;
        }
        else {

            n = NumberOfPoints;
        }

        for ( ptrdiff_t i=0; i<n; i++ ) {

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1);

            fout5 << qSetRealNumberPrecision(0) << (i + 1) << array_n[i];

            fout5 << qSetRealNumberPrecision(PRECISION)
                  << array_Ne_netto[i] << array_Gair[i] << array_Gfuel[i];

            if ( EGRcalc ) { fout5 << array_alpha_res[i]; }
            else           { fout5 << array_alpha[i];     }

            if ( smoke == 0 ) {

                fout5 << qSetRealNumberPrecision(PRECISION+1)
                      << array_Ka1m[i];
            }
            else if ( smoke == 1 ) {

                fout5 << qSetRealNumberPrecision(PRECISION+1)
                      << array_KaPerc[i];
            }
            else if ( smoke == 2 ) {

                fout5 << qSetRealNumberPrecision(PRECISION+1)
                      << array_FSN[i];
            }

            fout5 << qSetRealNumberPrecision(PRECISION) << array_CNOx[i];

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                 (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                 (std != STD_G2) ) {

                fout5 << array_mNOx[i] << array_gNOx[i];
            }

            fout5 << qSetFieldWidth(0) << "\n";
        }

        fout5 << "\n";

        fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1);

        if ( (!EGRcalc) && (!CheckMeas) ) {

            fout5 << "Point[-]";

            if ( gCOcalc ) { fout5 << "C_CO[ppm]"; }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                 (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                 (std != STD_G2) ) {

                if ( gCOcalc ) { fout5 << "mCO[g/h]" << "gCO[g/kWh]"; }
            }

            if ( gCHcalc ) { fout5 << "C_CH[ppm]"; }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                 (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                 (std != STD_G2) ) {

                if ( gCHcalc ) { fout5 << "mCH[g/h]" << "gCH[g/kWh]"; }
            }

            if ( params->val_PTcalc() == PTCALC_THROUGHSMOKE ) {

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    fout5 << "mPTs[g/h]" << "gPTs[g/kWh]";
                }
            }

            fout5 << "fa[-]"
                  << qSetFieldWidth(0) << "\n";

            for ( ptrdiff_t i=0; i<n; i++ ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1);

                fout5 << qSetRealNumberPrecision(PRECISION) << (i + 1);

                if ( gCOcalc ) { fout5 << array_CCO[i]; }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    if ( gCOcalc ) { fout5 << array_mCO[i] << array_gCO[i]; }
                }

                if ( gCHcalc ) { fout5 << array_CCH[i]; }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    if ( gCHcalc ) { fout5 << array_mCH[i] << array_gCH[i]; }
                }

                if ( params->val_PTcalc() == PTCALC_THROUGHSMOKE ) {

                    if ( (std != STD_C1) && (std != STD_D1) &&
                         (std != STD_D2) && (std != STD_E1) &&
                         (std != STD_E2) && (std != STD_E3) &&
                         (std != STD_E5) && (std != STD_F)  &&
                         (std != STD_G1) && (std != STD_G2) ) {

                        fout5 << array_mPT[i];

                        fout5 << qSetRealNumberPrecision(PRECISION+1)
                              << array_gPT[i];
                    }
                }

                fout5 << qSetRealNumberPrecision(PRECISION+1)
                      << array_fa[i]
                      << qSetFieldWidth(0) << "\n";
            }
        }
        else {

            fout5 << "Point[-]";

            if ( gCOcalc ) { fout5 << "C_CO[ppm]"; }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                 (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                 (std != STD_G2) ) {

                if ( gCOcalc ) { fout5 << "mCO[g/h]"; }
            }

            if ( gCHcalc ) { fout5 << "C_CH[ppm]"; }

            if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                 (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                 (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                 (std != STD_G2) ) {

                if ( gCHcalc ) { fout5 << "mCH[g/h]"; }
            }

            if ( params->val_PTcalc() == PTCALC_THROUGHSMOKE ) {

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    fout5 << "mPTs[g/h]" << "gPTs[g/kWh]";
                }
            }

            if ( EGRcalc   ) { fout5 << "rEGR[%]";       }
            if ( CheckMeas ) { fout5 << "diff_alpha[%]"; }

            fout5 << "fa[-]"
                  << qSetFieldWidth(0) << "\n";

            for ( ptrdiff_t i=0; i<n; i++ ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1);

                fout5 << qSetRealNumberPrecision(PRECISION) << (i + 1);

                if ( gCOcalc ) { fout5 << array_CCO[i]; }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    if ( gCOcalc ) { fout5 << array_mCO[i]; }
                }

                if ( gCHcalc ) { fout5 << array_CCH[i]; }

                if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
                     (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
                     (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
                     (std != STD_G2) ) {

                    if ( gCHcalc ) { fout5 << array_mCH[i]; }
                }

                if ( params->val_PTcalc() == PTCALC_THROUGHSMOKE ) {

                    if ( (std != STD_C1) && (std != STD_D1) &&
                         (std != STD_D2) && (std != STD_E1) &&
                         (std != STD_E2) && (std != STD_E3) &&
                         (std != STD_E5) && (std != STD_F)  &&
                         (std != STD_G1) && (std != STD_G2) ) {

                        fout5 << array_mPT[i];

                        fout5 << qSetRealNumberPrecision(PRECISION+1)
                              << array_gPT[i];
                    }
                }

                if ( EGRcalc ) {

                    fout5 << qSetRealNumberPrecision(PRECISION)
                          << array_rEGR[i];
                }

                if ( CheckMeas ) { fout5 << array_diff_alpha[i]; }

                fout5 << qSetRealNumberPrecision(PRECISION+1)
                      << array_fa[i]
                      << qSetFieldWidth(0) << "\n";
            }
        }

        fout5 << "\n";
        fout5 << qSetRealNumberPrecision(PRECISION);

        if ( ( (std == STD_EU6) || (std == STD_EU5) || (std == STD_EU4) ||
               (std == STD_EU3) ) &&
             (params->val_AddPointsCalc() == ADDPOINTSCALC_YES) ) {

            fout5 << "Additional points:\n\n";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Point[-]"    << "n[min-1]"    << "Ne_n[kW]"
                  << "Gair[kg/h]"  << "Gfuel[kg/h]" << "alpha[-]"
                  << "gNOm[g/kWh]" << "gNOc[g/kWh]" << "diff[%]"
                  << qSetFieldWidth(0) << "\n";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << (TCYCLEPOINTSNUMBER-2)
                  << array_n[TCYCLEPOINTSNUMBER-3]
                  << array_Ne_netto[TCYCLEPOINTSNUMBER-3]
                  << array_Gair[TCYCLEPOINTSNUMBER-3]
                  << array_Gfuel[TCYCLEPOINTSNUMBER-3]
                  << array_alpha[TCYCLEPOINTSNUMBER-3]
                  << gNOx1m << gNOx1c << diffNOx1
                  << qSetFieldWidth(0) << "\n";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << (TCYCLEPOINTSNUMBER-1)
                  << array_n[TCYCLEPOINTSNUMBER-2]
                  << array_Ne_netto[TCYCLEPOINTSNUMBER-2]
                  << array_Gair[TCYCLEPOINTSNUMBER-2]
                  << array_Gfuel[TCYCLEPOINTSNUMBER-2]
                  << array_alpha[TCYCLEPOINTSNUMBER-2]
                  << gNOx2m << gNOx2c << diffNOx2
                  << qSetFieldWidth(0) << "\n";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << (TCYCLEPOINTSNUMBER)
                  << array_n[TCYCLEPOINTSNUMBER-1]
                  << array_Ne_netto[TCYCLEPOINTSNUMBER-1]
                  << array_Gair[TCYCLEPOINTSNUMBER-1]
                  << array_Gfuel[TCYCLEPOINTSNUMBER-1]
                  << array_alpha[TCYCLEPOINTSNUMBER-1]
                  << gNOx3m << gNOx3c << diffNOx3
                  << qSetFieldWidth(0) << "\n";

            fout5 << "\n";
        }

        fout5 << "Specific emissions:\n\n";

        fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << " ";

        if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
             (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
             (std == STD_E5) ||
             (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Limit (old)" << "Limit (new)";
        }
        else if ( std == STD_GOST ) {

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Limit (old1)" << "Limit (old2)"
                  << "Limit (new1)" << "Limit (new2)";
        }
        else {

            fout5 << right << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << "Limitation";
        }

        fout5 << "CalcResult";

        if ( (std != STD_C1) && (std != STD_D1) && (std != STD_D2) &&
             (std != STD_E1) && (std != STD_E2) && (std != STD_E3) &&
             (std != STD_E5) && (std != STD_F)  && (std != STD_G1) &&
             (std != STD_G2) && (std != STD_GOST) ) {

            fout5 << "Conclusion";
        }

        fout5 << qSetFieldWidth(0) << "\n";

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

        if ( (std == STD_R96H8) || (std == STD_R96I8) || (std == STD_R96J8) ||
             (std == STD_R96K8) || (std == STD_R96H5) || (std == STD_R96I5) ||
             (std == STD_R96J5) || (std == STD_R96K5) ) {

            gNOxCHLimit = val_NOxCHLimit(std);
            gCOLimit = val_COLimit(std);
            gPTLimit = val_PTLimit(std);

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gNOx+gCH[g/kWh]";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << qSetRealNumberPrecision(PRECISION+1)
                  << gNOxCHLimit << gNOx+gCH;

            if ( ( (gNOx+gCH) < gNOxCHLimit) || ( (gNOx+gCH) == gNOxCHLimit) ) {

                fout5 << "OK"
                      << qSetFieldWidth(0) << "\n";
            }
            else {

                fout5 << "failed"
                      << qSetFieldWidth(0) << "\n";
            }

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gNOx[g/kWh]";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN+WIDTHOFCOLUMN-2)
                  << gNOx
                  << qSetFieldWidth(0) << "\n";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gCH[g/kWh]";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN+WIDTHOFCOLUMN-2)
                  << gCH
                  << qSetFieldWidth(0) << "\n";

            if ( gCOcalc ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gCO[g/kWh]";

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                      << gCOLimit << gCO;

                if ( (gCO < gCOLimit) || (gCO == gCOLimit) ) {

                    fout5 << "OK"
                          << qSetFieldWidth(0) << "\n";
                }
                else {

                    fout5 << "failed"
                          << qSetFieldWidth(0) << "\n";
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
            else if ( (std == STD_E1) || (std == STD_E2) ||
                      (std == STD_E3) || (std == STD_E5) ) {

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

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gNOx[g/kWh]";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                  << qSetRealNumberPrecision(PRECISION+1);

            if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                 (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                 (std == STD_E5) ||
                 (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                fout5 << gNOxLimit1 << gNOxLimit2;
            }
            else if ( std == STD_GOST ) {

                fout5 << gNOxLimit1 << gNOxLimit2 << gNOxLimit3 << gNOxLimit4;
            }
            else {

                fout5 << gNOxLimit;
            }

            fout5 << gNOx;

            if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) &&
                 (std != STD_D2)   && (std != STD_E1) && (std != STD_E2) &&
                 (std != STD_E3)   && (std != STD_E5) && (std != STD_F)  &&
                 (std != STD_G1)   && (std != STD_G2) ) {

                if ( (gNOx < gNOxLimit) || (gNOx == gNOxLimit) ) {

                    fout5 << "OK";
                }
                else {

                    fout5 << "failed";
                }
            }

            fout5 << qSetFieldWidth(0) << "\n";

            if ( gCOcalc ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gCO[g/kWh]";

                if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                     (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                     (std == STD_E5) ||
                     (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCOLimit1 << gCOLimit2;
                }
                else if ( std == STD_GOST ) {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCOLimit1 << gCOLimit2 << gCOLimit3 << gCOLimit4;
                }
                else {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCOLimit;
                }

                fout5 << gCO;

                if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) &&
                     (std != STD_D2)   && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3)   && (std != STD_E5) && (std != STD_F)  &&
                     (std != STD_G1)   && (std != STD_G2) ) {

                    if ( (gCO < gCOLimit) || (gCO == gCOLimit) ) {

                        fout5 << "OK";
                    }
                    else {

                        fout5 << "failed";
                    }
                }

                fout5 << qSetFieldWidth(0) << "\n";
            }

            if ( gCHcalc ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gCH[g/kWh]";

                if ( (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                     (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                     (std == STD_E5) ||
                     (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCHLimit1 << gCHLimit2;
                }
                else if ( std == STD_GOST ) {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCHLimit1 << gCHLimit2 << gCHLimit3 << gCHLimit4;
                }
                else {

                    fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                          << gCHLimit;
                }

                fout5 << gCH;

                if ( (std != STD_GOST) && (std != STD_C1) && (std != STD_D1) &&
                     (std != STD_D2)   && (std != STD_E1) && (std != STD_E2) &&
                     (std != STD_E3)   && (std != STD_E5) && (std != STD_F)  &&
                     (std != STD_G1)   && (std != STD_G2) ) {

                    if ( (gCH < gCHLimit) || (gCH == gCHLimit) ) {

                        fout5 << "OK";
                    }
                    else {

                        fout5 << "failed";
                    }
                }

                fout5 << qSetFieldWidth(0) << "\n";
            }
        }

        if ( (params->val_PTcalc() != PTCALC_NO) &&
             ( (std != STD_GOST) && (std != STD_OST) &&
               (std != STD_EU0)  && (std != STD_C1)  &&
               (std != STD_D1)   && (std != STD_D2)  &&
               (std != STD_E1)   && (std != STD_E2)  &&
               (std != STD_E3)   && (std != STD_E5)  &&
               (std != STD_F)    && (std != STD_G1)  &&
               (std != STD_G2) ) ) {

            if ( params->val_PTcalc() == PTCALC_THROUGHPTMASS ) {

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gPT[g/kWh]";

                fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1)
                      <<  gPTLimit << gPT;

                if ( (gPT < gPTLimit) || (gPT == gPTLimit) ) {

                    fout5 << "OK"
                          << qSetFieldWidth(0) << "\n";
                }
                else {

                    fout5 << "failed"
                          << qSetFieldWidth(0) << "\n";
                }
            }

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN-1+2) << "gPTs[g/kWh]";

            fout5 << qSetFieldWidth(WIDTHOFCOLUMN+WIDTHOFCOLUMN-2)
                  << gPTs
                  << qSetFieldWidth(0) << "\n";
        }

        fout5 << "\n";

        fout5 << qSetRealNumberPrecision(PRECISION)
              << "Mean specific fuel consumption: " << geMean << " g/kWh\n";

        data5.close();

        message += "libtoxic: Report file \"" +
                reportFileNameGAS + "\" created.\n";
    }

    //

    QString calcConfigFileName = mytime + ".conf";

    QString calcparam = reportdir.relativeFilePath(fullReportsPath) +
            reportdir.separator() + calcConfigFileName;

    QFile data7(calcparam);

    if ( !data7.open(QFile::WriteOnly) ) {

        throw ToxicError("Can not open file " + calcparam + "!");
    }

    QTextStream fout7(&data7);

    fout7 << "task"           << PARAMETERVALUEDELIMITER
          << params->val_Task()           << "\n"
          << "Vh"             << PARAMETERVALUEDELIMITER
          << params->val_Vh() <<             "\n"
          << "standard"       << PARAMETERVALUEDELIMITER
          << params->val_Standard()       << "\n"
          << "ChargingType"   << PARAMETERVALUEDELIMITER
          << params->val_ChargingType()   << "\n"
          << "FuelType"       << PARAMETERVALUEDELIMITER
          << params->val_FuelType()       << "\n"
          << "NOxSample"      << PARAMETERVALUEDELIMITER
          << params->val_NOxSample()      << "\n"
          << "PTcalc"         << PARAMETERVALUEDELIMITER
          << params->val_PTcalc()         << "\n"
          << "PTmass"         << PARAMETERVALUEDELIMITER
          << params->val_PTmass()         << "\n"
          << "AddPointsCalc"  << PARAMETERVALUEDELIMITER
          << params->val_AddPointsCalc()  << "\n"
          << "CalcConfigFile" << PARAMETERVALUEDELIMITER
          << params->val_CalcConfigFile() << "\n";

    data7.close();

    message += "libtoxic: Calculation config file \"" +
            calcConfigFileName + "\" created.\n";

    //

    if ( std != STD_FREECALC ) {

        message += "\ngNOx = " + QString::number(gNOx) + " g/kWh\n";

        if ( gCOcalc ) {

            message += "gCO = " + QString::number(gCO) + " g/kWh\n";
        }

        if ( gCHcalc ) {

            message += "gCH = " + QString::number(gCH) + " g/kWh\n";
        }

        ptrdiff_t ptcalc = params->val_PTcalc();

        if ( ptcalc != PTCALC_NO ) {

            if ( ptcalc == PTCALC_THROUGHPTMASS ) {

                message += "gPT = " + QString::number(gPT) + " g/kWh\n";
            }

            message += "gPTs = " + QString::number(gPTs) + " g/kWh\n";
        }

        message += "\n" + testcondres + "\n";
    }

    //

    return message;
}

QString CycleEmissions::lastReportsDir() const {

    return fullReportsPath;
}
