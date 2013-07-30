/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnR49R96.cpp

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

#include "txEmissionsOnR49R96.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"
#include "txError.h"
#include "txConstants.h"
#include "txAuxiliaryFunctions.h"
#include "txDataReader.h"
#include "txIdentification.h"
#include "txEmissionLimits.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <cmath>

#include <QDebug>

namespace toxic {

txEmissionsOnR49R96::txEmissionsOnR49R96(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts
        ) :
    m_NOxCalcMethod(NOXCALCMETHOD_DIRECT),
    m_gCOcalc(GCOCALC_YES),
    m_gCHcalc(GCHCALC_YES),
    m_gPTcalc(GPTCALC_NO),
    m_smoke(SMOKE_KA1M),
    m_qmdwORrd(QMDWORRD_QMDW),
    m_gNOx(0),
    m_gCO(0),
    m_gCH(0),
    m_gPT(0),
    m_gPTs(0),
    m_gNOx1m(0),
    m_gNOx2m(0),
    m_gNOx3m(0),
    m_gNOx1c(0),
    m_gNOx2c(0),
    m_gNOx3c(0),
    m_diffNOx1(0),
    m_diffNOx2(0),
    m_diffNOx3(0),
    m_geMean(0),
    m_t0Mean(0),
    m_B0Mean(0),
    m_RaMean(0) {

    m_commonParameters = commpars;
    m_calculationOptions = calcopts;

    if ( !m_calculationOptions->val_calcConfigFile().isEmpty() ) {

        m_calculationOptions->
                readCalcConfigFile(m_calculationOptions->val_calcConfigFile());
    }
}

txEmissionsOnR49R96::~txEmissionsOnR49R96() {
}

void txEmissionsOnR49R96::setSourceData(
        const QVector<QVector<double> > &srcdata
        ) {

    m_numberOfPoints = srcdata.size();

    checkSrcData();
    prepSrcArrays(srcdata);
}

void txEmissionsOnR49R96::setSourceData() {

    QSharedPointer<txDataReader> dataReader(new txDataReader());

    dataReader->readFile(m_commonParameters->val_srcFileNamePoints(), " ");
    QVector< QVector<double> > srcdata = dataReader->val_data();

    if ( srcdata.isEmpty() ) {
        throw txError("No data to calculate!");
    }

    //

    m_numberOfPoints = srcdata.size();

    checkSrcData();
    prepSrcArrays(srcdata);
}

void txEmissionsOnR49R96::calculate() {

    setupCalc();
    baseCalc();
    prelimCalc();

    const int currstd = m_calculationOptions->val_standard();

    if ( m_NOxCalcMethod != -1 ) {
        calc_gNOx();
    }

    if ( ( currstd == STD_EU6 || currstd == STD_EU5 ||
           currstd == STD_EU4 || currstd == STD_EU3 ) &&
         m_calculationOptions->val_addPointsCalc() == ADDPOINTSCALC_YES ) {
        calc_addPoints();
    }

    if ( m_gCOcalc == GCOCALC_YES ) {
        calc_gCO();
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        calc_gCH();
    }

    if ( m_gPTcalc == GPTCALC_YES ) {
        calc_gPT();
    }

    if ( m_EGRcalc == EGRCALC_YES ) {
        calc_rEGR();
    }

    if ( m_calculationOptions->val_standard() != STD_FREECALC ) {
        calc_Means();
    }

    if ( m_checkMeas == CHECKMEAS_YES ) {
        compareAlpha();
    }

    checkTestConditions();
}

QString txEmissionsOnR49R96::results() const {

    if ( m_calculationOptions->val_standard() == STD_FREECALC ) {
        return "\n" + m_testConditions + "\n";
    }

    QString res;

    if ( m_NOxCalcMethod != -1 ) {
        res += "\ngNOx = " + QString::number(m_gNOx) + " g/kWh\n";
    }

    if ( m_gCOcalc == GCOCALC_YES ) {
        res += "gCO = " + QString::number(m_gCO) + " g/kWh\n";
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        res += "gCH = " + QString::number(m_gCH) + " g/kWh\n";
    }

    const ptrdiff_t ptcalc = m_calculationOptions->val_PTcalc();

    if ( ptcalc == PTCALC_THROUGHSMOKE ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
        res += "gPTs = " + QString::number(m_gPTs) + " g/kWh\n";
    }

    if ( ptcalc == PTCALC_THROUGHPTMASS ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
        res += "gPT = " + QString::number(m_gPT) + " g/kWh\n";
    }

    res += "\n" + m_testConditions + "\n";

    return res;
}

QString txEmissionsOnR49R96::createReports() {

    m_fullReportsPath =
            m_commonParameters->val_reportsDirName()
            + QDir::separator()
            + m_calculationOptions->defStandardName(
                m_calculationOptions->val_standard()
                )
            + "_"
            + m_currTime;

    QDir reportDir;
    reportDir.mkdir(m_fullReportsPath);

    QString message = saveSourceData() + saveCheckoutData() + saveCalcOptions();

    const int currstd = m_calculationOptions->val_standard();

    if ( currstd != STD_FREECALC ) {
        message += saveReportGAS();
    }

    if ( m_gPTcalc == GPTCALC_YES &&
         (m_calculationOptions->val_PTcalc() == PTCALC_THROUGHPTMASS ||
          m_calculationOptions->val_PTcalc() == PTCALC_THROUGHSMOKEANDPTMASS) &&
         currstd != STD_FREECALC ) {
        message += saveReportPT();
    }

    return message;
}

void txEmissionsOnR49R96::checkSrcData() {

    const int currstd = m_calculationOptions->val_standard();
    const int addpointscalc = m_calculationOptions->val_addPointsCalc();

    if ( currstd < STD_EU6 ||
         (currstd > STD_R96K5 && currstd != STD_FREECALC) ) {
        throw txError("Wrong source data or calculation settings! "
                      "This library module is not suitable for the data at "
                      "the specified standard! "
                      "If you see this message, your program uses libtoxic "
                      "incorrectly!");
    }

    if (
            (
                (
                    (currstd == STD_EU6) || (currstd == STD_EU5) ||
                    (currstd == STD_EU4) || (currstd == STD_EU3)
                    ) &&
                (
                    (addpointscalc == ADDPOINTSCALC_YES)
                    ) &&
                (
                    (m_numberOfPoints != (ESCPOINTSNUMBER+ESCADDPOINTSNUMBER))
                    )
                ) ||
            (
                (
                    (currstd == STD_EU6) || (currstd == STD_EU5) ||
                    (currstd == STD_EU4) || (currstd == STD_EU3)
                    ) &&
                (
                    (addpointscalc == ADDPOINTSCALC_NO)
                    ) &&
                (
                    (m_numberOfPoints != ESCPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_EU2) || (currstd == STD_EU1) ||
                    (currstd == STD_EU0) || (currstd == STD_OST3700123481) ||
                    (currstd == STD_GOST17220597)
                    ) &&
                (
                    (m_numberOfPoints != ESCPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_R96E8) || (currstd == STD_R96F8) ||
                    (currstd == STD_R96G8) || (currstd == STD_R96D8) ||
                    (currstd == STD_R96H8) || (currstd == STD_R96I8) ||
                    (currstd == STD_R96J8) || (currstd == STD_R96K8)
                    ) &&
                (
                    (m_numberOfPoints != H8POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_R96E5) || (currstd == STD_R96F5) ||
                    (currstd == STD_R96G5) || (currstd == STD_R96D5) ||
                    (currstd == STD_R96H5) || (currstd == STD_R96I5) ||
                    (currstd == STD_R96J5) || (currstd == STD_R96K5)
                    ) &&
                (
                    (m_numberOfPoints != H5POINTSNUMBER)
                    )
                ) ) {

        throw txError("Wrong source data or calculation settings! "
                      "Check the number of points of source data and "
                      "calculation parameters.");
    }
}

void txEmissionsOnR49R96::setupCalc() {

    m_currTime = QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss");

    //

    if ( zeroArray(ma_n) ) {
        throw txError("Array \"n\" must contain nonzero data!");
    }

    if ( !zeroArray(ma_Me_brutto) ) {
        m_NeCalcMethod = NECALCMETHOD_THROUGHME;
    }
    else if ( !zeroArray(ma_Ne_brutto) ) {
        m_NeCalcMethod = NECALCMETHOD_THROUGHNE;
    }
    else {
        throw txError("Arrays \"Me_b\" or \"Ne_b\" "
                      "must contain nonzero data!");
    }

    if ( zeroArray(ma_t0) ) {
        throw txError("Array \"t0\" must contain nonzero data!");
    }

    if ( zeroArray(ma_B0) ) {
        throw txError("Array \"B0\" must contain nonzero data!");
    }

    if ( zeroArray(ma_Ra) ) {
        throw txError("Array \"Ra\" must contain nonzero data!");
    }

    if ( !zeroArray(ma_Gair) ) {
        m_GairVals = GAIRVALS_THROUGHMASSFLOW;
    }
    else if ( !zeroArray(ma_dPn) ) {
        m_GairVals = GAIRVALS_THROUGHNOZZLE;
    }
    else {
        throw txError("Arrays \"Gair\" or \"dPn\" "
                      "must contain nonzero data!");
    }

    if ( zeroArray(ma_Gfuel) ) {
        throw txError("Array \"Gfuel\" must contain nonzero data!");
    }

    if ( !zeroArray(ma_CNOx) ) {
        m_NOxCalcMethod = NOXCALCMETHOD_DIRECT;
    }
    else if ( !zeroArray(ma_gNOx) ) {
        m_NOxCalcMethod = NOXCALCMETHOD_REVERCE;
    }
    else {
        m_NOxCalcMethod = -1;
    }

    if ( zeroArray(ma_CCO) ) {
        m_gCOcalc = GCOCALC_NO;
    }
    else {
        m_gCOcalc = GCOCALC_YES;
    }

    if ( zeroArray(ma_CCH) ) {
        m_gCHcalc = GCHCALC_NO;
    }
    else {
        m_gCHcalc = GCHCALC_YES;
    }

    if ( !zeroArray(ma_CCO2in) && !zeroArray(ma_CCO2out) ) {
        m_EGRcalc = EGRCALC_YES;
    }
    else {
        m_EGRcalc = EGRCALC_NO;
    }

    if ( zeroArray(ma_CO2) ) {
        m_checkMeas = CHECKMEAS_NO;
    }
    else {
        m_checkMeas = CHECKMEAS_YES;
    }

    const int currstd = m_calculationOptions->val_standard();

    if ( m_calculationOptions->val_PTcalc() != PTCALC_NO &&
         currstd != STD_EU0 &&
         currstd != STD_OST3700123481 &&
         currstd != STD_GOST17220597 ) {

        if ( m_calculationOptions->val_PTcalc() == PTCALC_THROUGHSMOKE ||
             m_calculationOptions->val_PTcalc() == PTCALC_THROUGHSMOKEANDPTMASS ) {

            if ( zeroArray(ma_Pr) || zeroArray(ma_ts) ||
                 ( zeroArray(ma_Ka1m) && zeroArray(ma_KaPerc) && zeroArray(ma_FSN) ) ) {

                throw txError("Arrays \"Pr\", \"ts\" and any of the arrays "
                              "\"Ka1m\", \"KaPerc\", \"FSN\" "
                              "must contain nonzero data!");
            }
        }

        if ( m_calculationOptions->val_PTcalc() == PTCALC_THROUGHPTMASS ||
             m_calculationOptions->val_PTcalc() == PTCALC_THROUGHSMOKEANDPTMASS ) {

            if ( zeroArray(ma_tauf) || zeroArray(ma_qmdew) ||
                 ( zeroArray(ma_qmdw) && zeroArray(ma_rd) ) ) {

                throw txError("Arrays \"tauf\", \"qmdew\" "
                              "and any of the arrays "
                              "\"qmdw\", \"rd\" "
                              "must contain nonzero data!");
            }

            if ( !zeroArray(ma_qmdw) ) {
                m_qmdwORrd = QMDWORRD_QMDW;
            }
            else if ( !zeroArray(ma_rd) ) {
                m_qmdwORrd = QMDWORRD_RD;
            }
            else {
                throw txError("Arrays \"qmdw\" or \"rd\" must contain nonzero data!");
            }
        }

        m_gPTcalc = GPTCALC_YES;
    }
    else {
        m_gPTcalc = GPTCALC_NO;
    }

    if ( !zeroArray(ma_FSN) ) {
        m_smoke = SMOKE_FSN;
    }
    else if ( !zeroArray(ma_Ka1m) ) {
        m_smoke = SMOKE_KA1M;
    }
    else if ( !zeroArray(ma_KaPerc) ) {
        m_smoke = SMOKE_KAPERC;
    }
    else {
        m_smoke = -1;
    }
}

void txEmissionsOnR49R96::prelimCalc() {

    ma_Ne_netto.clear();  ma_Ne_netto.resize(m_numberOfPoints);
    ma_Me_netto.clear();  ma_Me_netto.resize(m_numberOfPoints);
    ma_Gexh.clear();      ma_Gexh.resize(m_numberOfPoints);
    ma_Ha.clear();        ma_Ha.resize(m_numberOfPoints);
    ma_Gaird.clear();     ma_Gaird.resize(m_numberOfPoints);
    ma_Kw2.clear();       ma_Kw2.resize(m_numberOfPoints);
    ma_Ffh.clear();       ma_Ffh.resize(m_numberOfPoints);
    ma_Kf.clear();        ma_Kf.resize(m_numberOfPoints);
    ma_Kwr.clear();       ma_Kwr.resize(m_numberOfPoints);
    ma_Khd.clear();       ma_Khd.resize(m_numberOfPoints);
    ma_ge_netto.clear();  ma_ge_netto.resize(m_numberOfPoints);

    const int currstd = m_calculationOptions->val_standard();
    const double WH = m_commonParameters->val_WH();
    const double WN = m_commonParameters->val_WN();
    const double WO2 = m_commonParameters->val_WO2();

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
             currstd == STD_EU3 || currstd == STD_EU2 || currstd == STD_EU1 ||
             currstd == STD_EU0 || currstd == STD_FREECALC ) {
            ma_Ne_netto[i] = ma_Ne_brutto[i] - ma_N_fan[i];
        }
        else {
            ma_Ne_netto[i] = ma_Ne_brutto[i];
        }
        ma_Me_netto[i] = ma_Ne_netto[i] * 9550.0 / ma_n[i];
        ma_Gexh[i] = ma_Gair[i] + ma_Gfuel[i];
        ma_Ha[i] = 6.22 * ma_Ra[i] * ma_Pa[i] /
                (ma_Pb[i] - ma_Pa[i] * ma_Ra[i] * 0.01);

        if ( currstd == STD_EU2 || currstd == STD_EU1 || currstd == STD_EU0 ||
             currstd == STD_OST3700123481 || currstd == STD_GOST17220597 ) {
            ma_Gaird[i] = ma_Gair[i] / (1.0 + ma_Ha[i] / 1000.0);
        }
        else {
            ma_Gaird[i] = (1.0 - ma_Ha[i] / 1000.0) * ma_Gair[i];
        }

        if ( currstd != STD_OST3700123481 ) {

            ma_Kw2[i] = 1.608 * ma_Ha[i] / (1000.0 + 1.608 * ma_Ha[i]);
            ma_Ffh[i] = 1.969 / (1.0 + ma_Gfuel[i] / ma_Gair[i]);
            ma_Kf[i] = 0.055594 * WH + 0.0080021 * WN + 0.0070046 * WO2;
        }

        if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
             currstd == STD_EU3 || currstd == STD_FREECALC ) {
            ma_Kwr[i] = (1.0 - (1.2442 * ma_Ha[i] + 111.19 * WH * ma_Gfuel[i] /
                                ma_Gaird[i]) / (773.4 + 1.2442 * ma_Ha[i] +
                                                ma_Gfuel[i] / ma_Gaird[i] *
                                                ma_Kf[i] * 1000.0)) * 1.008;
            ma_Khd[i] = 1.0 / (1.0 - 0.0182 * (ma_Ha[i] - 10.71) +
                               0.0045 * (ma_t0[i] + 273.0 - 298.0));
        }
        else if ( currstd == STD_EU2 || currstd == STD_EU1 ||
                  currstd == STD_EU0 || currstd == STD_GOST17220597 ) {
            ma_Kwr[i] = 1.0 - 1.85 * ma_Gfuel[i] / ma_Gaird[i];
            ma_Khd[i] = 1.0 / (1.0 + (0.044 * ma_Gfuel[i] / ma_Gaird[i] - 0.0038) *
                               (7.0 * ma_Ha[i] - 75.0) +
                               (-0.116 * ma_Gfuel[i] / ma_Gaird[i] + 0.0053) * 1.8 *
                               (ma_t0[i] + 273.0 - 302.0));
        }
        else if ( currstd == STD_OST3700123481 ) {
            ma_Kwr[i] = 1.0 - 1.8 * ma_Gfuel[i] / ma_Gair[i];
        }
        else {
            ma_Kwr[i] = (1.0 - ma_Ffh[i] * ma_Gfuel[i] / ma_Gaird[i]) - ma_Kw2[i];
            ma_Khd[i] = 1.0 / (1.0 + (0.309 * ma_Gfuel[i] /
                                      ma_Gaird[i] - 0.0266) * (ma_Ha[i] - 10.71) +
                               (-0.209 * ma_Gfuel[i] / ma_Gaird[i] + 0.00954) *
                               (ma_t0[i] + 273.0 - 298.0));
        }

        ma_ge_netto[i] = ma_Gfuel[i] / ma_Ne_netto[i] * 1000.0;
    }
}

void txEmissionsOnR49R96::calc_gNOx() {

    ma_mNOx.clear(); ma_mNOx.resize(m_numberOfPoints);

    const int currstd = m_calculationOptions->val_standard();
    const int noxsample = m_calculationOptions->val_NOxSample();
    double summ_mNOx = 0;
    double summ_Ne_netto = 0;

    if ( m_NOxCalcMethod == NOXCALCMETHOD_DIRECT ) {

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

            if ( noxsample == NOXSAMPLE_WET ) {

                if ( currstd == STD_OST3700123481 ) {
                    ma_mNOx[i] = 0.001587 * ma_CNOx[i] * ma_Gexh[i];
                }
                else {
                    ma_mNOx[i] = 0.001587 * ma_CNOx[i] * ma_Khd[i] * ma_Gexh[i];
                }
            }
            else if ( noxsample == NOXSAMPLE_DRY ) {

                if ( currstd == STD_OST3700123481 ) {
                    ma_mNOx[i] = 0.001587 * ma_CNOx[i] * ma_Kwr[i] * ma_Gexh[i];
                }
                else {
                    ma_mNOx[i] = 0.001587 * ma_CNOx[i] *
                            ma_Kwr[i] * ma_Khd[i] * ma_Gexh[i];
                }
            }
            else {
                throw txError("Incorrect NOx sample type!");
            }

            ma_gNOx[i] = ma_mNOx[i] / ma_Ne_netto[i];

            if ( i >= ESCPOINTSNUMBER  ) {
                continue;
            }

            summ_mNOx += ma_mNOx[i] * ma_w[i];
            summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
        }
    }
    else if ( m_NOxCalcMethod == NOXCALCMETHOD_REVERCE ) {

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

            ma_mNOx[i] = ma_gNOx[i] * ma_Ne_netto[i];

            if ( noxsample == NOXSAMPLE_WET ) {

                if ( currstd == STD_OST3700123481 ) {
                    ma_CNOx[i] = ma_mNOx[i] / (0.001587 * ma_Gexh[i]);
                }
                else {
                    ma_CNOx[i] = ma_mNOx[i] / (0.001587 * ma_Khd[i] * ma_Gexh[i]);
                }
            }
            else if ( noxsample == NOXSAMPLE_DRY ) {

                if ( currstd == STD_OST3700123481 ) {
                    ma_CNOx[i] = ma_mNOx[i] / (0.001587 * ma_Kwr[i] * ma_Gexh[i]);
                }
                else {
                    ma_CNOx[i] = ma_mNOx[i] /
                            (0.001587 * ma_Kwr[i] * ma_Khd[i] * ma_Gexh[i]);
                }
            }
            else {
                throw txError("Incorrect NOx sample type!");
            }

            if ( i >= ESCPOINTSNUMBER  ) {
                continue;
            }

            summ_mNOx += ma_mNOx[i] * ma_w[i];
            summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
        }
    }
    else {
        throw txError("Incorrect NOx calculation method!");
    }

    m_gNOx = summ_mNOx / summ_Ne_netto;
}

void txEmissionsOnR49R96::calc_addPoints() {

    /*
        Control Point (Z)

        power
            |
            |                U
       Ntu _|___________
            |
            |      T
            |           Z
            |
            |                S
       Nrs _|___________
            |
            |      R
            |
            |______|_________|______ speed
                   |         |
                  nrt       nsu
    */

    const int noxsample = m_calculationOptions->val_NOxSample();

    for ( ptrdiff_t i=(m_numberOfPoints-ESCADDPOINTSNUMBER);
          i<m_numberOfPoints;
          i++ ) {

        if ( noxsample == NOXSAMPLE_WET ) {
            ma_mNOx[i] = 0.001587 * ma_CNOx[i] * ma_Khd[i] * ma_Gexh[i];
        }
        else if ( noxsample == NOXSAMPLE_DRY ) {
            ma_mNOx[i] = 0.001587 * ma_CNOx[i] * ma_Kwr[i] * ma_Khd[i] * ma_Gexh[i];
        }

        ma_gNOx[i] = ma_mNOx[i] / ma_Ne_netto[i];
    }

    m_gNOx1m = ma_gNOx[m_numberOfPoints - ESCADDPOINTSNUMBER];
    m_gNOx2m = ma_gNOx[m_numberOfPoints - ESCADDPOINTSNUMBER + 1];
    m_gNOx3m = ma_gNOx[m_numberOfPoints - ESCADDPOINTSNUMBER + 2];

    const double A = (ma_n[1] + ma_n[ 4] + ma_n[ 5] + ma_n[ 6]) / 4.0;
    const double B = (ma_n[2] + ma_n[ 3] + ma_n[ 7] + ma_n[ 8]) / 4.0;
    const double C = (ma_n[9] + ma_n[10] + ma_n[11] + ma_n[12]) / 4.0;

    const double nz1 = ma_n[m_numberOfPoints - ESCADDPOINTSNUMBER];
    const double nz2 = ma_n[m_numberOfPoints - ESCADDPOINTSNUMBER + 1];
    const double nz3 = ma_n[m_numberOfPoints - ESCADDPOINTSNUMBER + 2];

    const double Nz1 = ma_Ne_netto[m_numberOfPoints - ESCADDPOINTSNUMBER];
    const double Nz2 = ma_Ne_netto[m_numberOfPoints - ESCADDPOINTSNUMBER + 1];
    const double Nz3 = ma_Ne_netto[m_numberOfPoints - ESCADDPOINTSNUMBER + 2];

    double nrt1 = 0; double nsu1 = 0;
    double nrt2 = 0; double nsu2 = 0;
    double nrt3 = 0; double nsu3 = 0;

    double Nt1 = 0; double Nu1 = 0; double Nr1 = 0; double Ns1 = 0;
    double Nt2 = 0; double Nu2 = 0; double Nr2 = 0; double Ns2 = 0;
    double Nt3 = 0; double Nu3 = 0; double Nr3 = 0; double Ns3 = 0;

    double gNOt1 = 0; double gNOu1 = 0; double gNOr1 = 0; double gNOs1 = 0;
    double gNOt2 = 0; double gNOu2 = 0; double gNOr2 = 0; double gNOs2 = 0;
    double gNOt3 = 0; double gNOu3 = 0; double gNOr3 = 0; double gNOs3 = 0;

    if ( nz1 > A && nz1 < B ) {

        nrt1 = A;
        nsu1 = B;

        if ( Nz1 > ma_Ne_netto[6] && Nz1 < ma_Ne_netto[2] ) {

            Nt1 = ma_Ne_netto[4]; gNOt1 = ma_gNOx[4];
            Nu1 = ma_Ne_netto[2]; gNOu1 = ma_gNOx[2];
            Nr1 = ma_Ne_netto[6]; gNOr1 = ma_gNOx[6];
            Ns1 = ma_Ne_netto[8]; gNOs1 = ma_gNOx[8];
        }
        else if ( Nz1 > ma_Ne_netto[4] && Nz1 < ma_Ne_netto[3] ) {

            Nt1 = ma_Ne_netto[5]; gNOt1 = ma_gNOx[5];
            Nu1 = ma_Ne_netto[3]; gNOu1 = ma_gNOx[3];
            Nr1 = ma_Ne_netto[4]; gNOr1 = ma_gNOx[4];
            Ns1 = ma_Ne_netto[2]; gNOs1 = ma_gNOx[2];
        }
        else if ( Nz1 > ma_Ne_netto[5] && Nz1 < ma_Ne_netto[7] ) {

            Nt1 = ma_Ne_netto[1]; gNOt1 = ma_gNOx[1];
            Nu1 = ma_Ne_netto[7]; gNOu1 = ma_gNOx[7];
            Nr1 = ma_Ne_netto[5]; gNOr1 = ma_gNOx[5];
            Ns1 = ma_Ne_netto[3]; gNOs1 = ma_gNOx[3];
        }
        else {

            throw txError("Incorrect power value of 1st additional point!");
        }
    }
    else if ( nz1 > B && nz1 < C ) {

        nrt1 = B;
        nsu1 = C;

        if ( Nz1 > ma_Ne_netto[8] && Nz1 < ma_Ne_netto[12] ) {

            Nt1 = ma_Ne_netto[2];  gNOt1 = ma_gNOx[2];
            Nu1 = ma_Ne_netto[12]; gNOu1 = ma_gNOx[12];
            Nr1 = ma_Ne_netto[8];  gNOr1 = ma_gNOx[8];
            Ns1 = ma_Ne_netto[10]; gNOs1 = ma_gNOx[10];
        }
        else if ( Nz1 > ma_Ne_netto[2] && Nz1 < ma_Ne_netto[11] ) {

            Nt1 = ma_Ne_netto[3];  gNOt1 = ma_gNOx[3];
            Nu1 = ma_Ne_netto[11]; gNOu1 = ma_gNOx[11];
            Nr1 = ma_Ne_netto[2];  gNOr1 = ma_gNOx[2];
            Ns1 = ma_Ne_netto[12]; gNOs1 = ma_gNOx[12];
        }
        else if ( Nz1 > ma_Ne_netto[3] && Nz1 < ma_Ne_netto[9] ) {

            Nt1 = ma_Ne_netto[7];  gNOt1 = ma_gNOx[7];
            Nu1 = ma_Ne_netto[9];  gNOu1 = ma_gNOx[9];
            Nr1 = ma_Ne_netto[3];  gNOr1 = ma_gNOx[3];
            Ns1 = ma_Ne_netto[11]; gNOs1 = ma_gNOx[11];
        }
        else {

            throw txError("Incorrect power value of 1st additional point!");
        }
    }
    else {

        throw txError("Incorrect speed value of 1st additional point!");
    }

    if ( nz2 > A && nz2 < B ) {

        nrt2 = A;
        nsu2 = B;

        if ( Nz2 > ma_Ne_netto[6] && Nz2 < ma_Ne_netto[2] ) {

            Nt2 = ma_Ne_netto[4]; gNOt2 = ma_gNOx[4];
            Nu2 = ma_Ne_netto[2]; gNOu2 = ma_gNOx[2];
            Nr2 = ma_Ne_netto[6]; gNOr2 = ma_gNOx[6];
            Ns2 = ma_Ne_netto[8]; gNOs2 = ma_gNOx[8];
        }
        else if ( Nz2 > ma_Ne_netto[4] && Nz2 < ma_Ne_netto[3] ) {

            Nt2 = ma_Ne_netto[5]; gNOt2 = ma_gNOx[5];
            Nu2 = ma_Ne_netto[3]; gNOu2 = ma_gNOx[3];
            Nr2 = ma_Ne_netto[4]; gNOr2 = ma_gNOx[4];
            Ns2 = ma_Ne_netto[2]; gNOs2 = ma_gNOx[2];
        }
        else if ( Nz2 > ma_Ne_netto[5] && Nz2 < ma_Ne_netto[7] ) {

            Nt2 = ma_Ne_netto[1]; gNOt2 = ma_gNOx[1];
            Nu2 = ma_Ne_netto[7]; gNOu2 = ma_gNOx[7];
            Nr2 = ma_Ne_netto[5]; gNOr2 = ma_gNOx[5];
            Ns2 = ma_Ne_netto[3]; gNOs2 = ma_gNOx[3];
        }
        else {

            throw txError("Incorrect power value of 2nd additional point!");
        }
    }
    else if ( nz2 > B && nz2 < C ) {

        nrt2 = B;
        nsu2 = C;

        if ( Nz2 > ma_Ne_netto[8] && Nz2 < ma_Ne_netto[12] ) {

            Nt2 = ma_Ne_netto[2];  gNOt2 = ma_gNOx[2];
            Nu2 = ma_Ne_netto[12]; gNOu2 = ma_gNOx[12];
            Nr2 = ma_Ne_netto[8];  gNOr2 = ma_gNOx[8];
            Ns2 = ma_Ne_netto[10]; gNOs2 = ma_gNOx[10];
        }
        else if ( Nz2 > ma_Ne_netto[2] && Nz2 < ma_Ne_netto[11] ) {

            Nt2 = ma_Ne_netto[3];  gNOt2 = ma_gNOx[3];
            Nu2 = ma_Ne_netto[11]; gNOu2 = ma_gNOx[11];
            Nr2 = ma_Ne_netto[2];  gNOr2 = ma_gNOx[2];
            Ns2 = ma_Ne_netto[12]; gNOs2 = ma_gNOx[12];
        }
        else if ( Nz2 > ma_Ne_netto[3] && Nz2 < ma_Ne_netto[9] ) {

            Nt2 = ma_Ne_netto[7];  gNOt2 = ma_gNOx[7];
            Nu2 = ma_Ne_netto[9];  gNOu2 = ma_gNOx[9];
            Nr2 = ma_Ne_netto[3];  gNOr2 = ma_gNOx[3];
            Ns2 = ma_Ne_netto[11]; gNOs2 = ma_gNOx[11];
        }
        else {

            throw txError("Incorrect power value of 2nd additional point!");
        }
    }
    else {

        throw txError("Incorrect speed value of 2nd additional point!");
    }

    if ( nz3 > A && nz3 < B ) {

        nrt3 = A;
        nsu3 = B;

        if ( Nz3 > ma_Ne_netto[6] && Nz3 < ma_Ne_netto[2] ) {

            Nt3 = ma_Ne_netto[4]; gNOt3 = ma_gNOx[4];
            Nu3 = ma_Ne_netto[2]; gNOu3 = ma_gNOx[2];
            Nr3 = ma_Ne_netto[6]; gNOr3 = ma_gNOx[6];
            Ns3 = ma_Ne_netto[8]; gNOs3 = ma_gNOx[8];
        }
        else if ( Nz3 > ma_Ne_netto[4] && Nz3 < ma_Ne_netto[3] ) {

            Nt3 = ma_Ne_netto[5]; gNOt3 = ma_gNOx[5];
            Nu3 = ma_Ne_netto[3]; gNOu3 = ma_gNOx[3];
            Nr3 = ma_Ne_netto[4]; gNOr3 = ma_gNOx[4];
            Ns3 = ma_Ne_netto[2]; gNOs3 = ma_gNOx[2];
        }
        else if ( Nz3 > ma_Ne_netto[5] && Nz3 < ma_Ne_netto[7] ) {

            Nt3 = ma_Ne_netto[1]; gNOt3 = ma_gNOx[1];
            Nu3 = ma_Ne_netto[7]; gNOu3 = ma_gNOx[7];
            Nr3 = ma_Ne_netto[5]; gNOr3 = ma_gNOx[5];
            Ns3 = ma_Ne_netto[3]; gNOs3 = ma_gNOx[3];
        }
        else {

            throw txError("Incorrect power value of 3rd additional point!");
        }
    }
    else if ( nz3 > B && nz3 < C ) {

        nrt3 = B;
        nsu3 = C;

        if ( Nz3 > ma_Ne_netto[8] && Nz3 < ma_Ne_netto[12] ) {

            Nt3 = ma_Ne_netto[2];  gNOt3 = ma_gNOx[2];
            Nu3 = ma_Ne_netto[12]; gNOu3 = ma_gNOx[12];
            Nr3 = ma_Ne_netto[8];  gNOr3 = ma_gNOx[8];
            Ns3 = ma_Ne_netto[10]; gNOs3 = ma_gNOx[10];
        }
        else if ( Nz3 > ma_Ne_netto[2] && Nz3 < ma_Ne_netto[11] ) {

            Nt3 = ma_Ne_netto[3];  gNOt3 = ma_gNOx[3];
            Nu3 = ma_Ne_netto[11]; gNOu3 = ma_gNOx[11];
            Nr3 = ma_Ne_netto[2];  gNOr3 = ma_gNOx[2];
            Ns3 = ma_Ne_netto[12]; gNOs3 = ma_gNOx[12];
        }
        else if ( Nz3 > ma_Ne_netto[3] && Nz3 < ma_Ne_netto[9] ) {

            Nt3 = ma_Ne_netto[7];  gNOt3 = ma_gNOx[7];
            Nu3 = ma_Ne_netto[9];  gNOu3 = ma_gNOx[9];
            Nr3 = ma_Ne_netto[3];  gNOr3 = ma_gNOx[3];
            Ns3 = ma_Ne_netto[11]; gNOs3 = ma_gNOx[11];
        }
        else {

            throw txError("Incorrect power value of 3rd additional point!");
        }
    }
    else {

        throw txError("Incorrect speed value of 3rd additional point!");
    }

    const double Ntu1 = Nt1 + (Nu1 - Nt1) * (nz1 - nrt1) / (nsu1 - nrt1);
    const double Ntu2 = Nt2 + (Nu2 - Nt2) * (nz2 - nrt2) / (nsu2 - nrt2);
    const double Ntu3 = Nt3 + (Nu3 - Nt3) * (nz3 - nrt3) / (nsu3 - nrt3);

    const double Nrs1 = Nr1 + (Ns1 - Nr1) * (nz1 - nrt1) / (nsu1 - nrt1);
    const double Nrs2 = Nr2 + (Ns2 - Nr2) * (nz2 - nrt2) / (nsu2 - nrt2);
    const double Nrs3 = Nr3 + (Ns3 - Nr3) * (nz3 - nrt3) / (nsu3 - nrt3);

    const double gNOtu1 = gNOt1 + (gNOu1 - gNOt1) * (nz1 - nrt1) / (nsu1 - nrt1);
    const double gNOtu2 = gNOt2 + (gNOu2 - gNOt2) * (nz2 - nrt2) / (nsu2 - nrt2);
    const double gNOtu3 = gNOt3 + (gNOu3 - gNOt3) * (nz3 - nrt3) / (nsu3 - nrt3);

    const double gNOrs1 = gNOr1 + (gNOs1 - gNOr1) * (nz1 - nrt1) / (nsu1 - nrt1);
    const double gNOrs2 = gNOr2 + (gNOs2 - gNOr2) * (nz2 - nrt2) / (nsu2 - nrt2);
    const double gNOrs3 = gNOr3 + (gNOs3 - gNOr3) * (nz3 - nrt3) / (nsu3 - nrt3);

    m_gNOx1c = gNOrs1 + (gNOtu1 - gNOrs1) * (Nz1 - Nrs1) / (Ntu1 - Nrs1);
    m_gNOx2c = gNOrs2 + (gNOtu2 - gNOrs2) * (Nz2 - Nrs2) / (Ntu2 - Nrs2);
    m_gNOx3c = gNOrs3 + (gNOtu3 - gNOrs3) * (Nz3 - Nrs3) / (Ntu3 - Nrs3);

    m_diffNOx1 = (m_gNOx1m - m_gNOx1c) / m_gNOx1c * 100.0;
    m_diffNOx2 = (m_gNOx2m - m_gNOx2c) / m_gNOx2c * 100.0;
    m_diffNOx3 = (m_gNOx3m - m_gNOx3c) / m_gNOx3c * 100.0;
}

void txEmissionsOnR49R96::calc_gCO() {

    ma_mCO.clear(); ma_mCO.resize(m_numberOfPoints);
    ma_gCO.clear(); ma_gCO.resize(m_numberOfPoints);

    double summ_mCO = 0;
    double summ_Ne_netto = 0;

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        ma_mCO[i] = 0.000966 * ma_CCO[i] * ma_Kwr[i] * ma_Gexh[i]; // always is dry
        ma_gCO[i] = ma_mCO[i] / ma_Ne_netto[i];
        summ_mCO += ma_mCO[i] * ma_w[i];
        summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
    }

    m_gCO = summ_mCO / summ_Ne_netto;
}

void txEmissionsOnR49R96::calc_gCH() {

    ma_mCH.clear(); ma_mCH.resize(m_numberOfPoints);
    ma_gCH.clear(); ma_gCH.resize(m_numberOfPoints);

    const int currstd = m_calculationOptions->val_standard();

    double summ_mCH = 0;
    double summ_Ne_netto = 0;

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        if ( currstd == STD_OST3700123481 ) {
            ma_mCH[i] = 0.000485 * ma_CCH[i] * ma_Gexh[i]; // always is wet
        }
        else {
            ma_mCH[i] = 0.000479 * ma_CCH[i] * ma_Gexh[i]; // always is wet
        }

        ma_gCH[i] = ma_mCH[i] / ma_Ne_netto[i];
        summ_mCH += ma_mCH[i] * ma_w[i];
        summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
    }

    m_gCH = summ_mCH / summ_Ne_netto;
}

void txEmissionsOnR49R96::calc_gPT() {

    ma_ror.clear();   ma_ror.resize(m_numberOfPoints);
    ma_CPT.clear();   ma_CPT.resize(m_numberOfPoints);
    ma_qmedf.clear(); ma_qmedf.resize(m_numberOfPoints);
    ma_msepi.clear(); ma_msepi.resize(m_numberOfPoints);
    ma_mPT.clear();   ma_mPT.resize(m_numberOfPoints);
    ma_gPT.clear();   ma_gPT.resize(m_numberOfPoints);

    const int ptcalc = m_calculationOptions->val_PTcalc();
    const double mf = m_calculationOptions->val_PTmass();
    const double L = m_commonParameters->val_L();
    const double Rr = m_commonParameters->val_Rr();
    double summ_Ne_netto = 0;

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    if ( ptcalc == PTCALC_THROUGHSMOKE ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {

        double summ_mPT = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            if ( m_smoke == SMOKE_KA1M ) {
                ma_KaPerc[i] = KaPerc(ma_Ka1m[i], L);
                ma_FSN[i] = FSN(ma_KaPerc[i]);
            }
            else if ( m_smoke == SMOKE_KAPERC ) {
                ma_FSN[i] = FSN(ma_KaPerc[i]);
            }

            ma_ror[i] = (ma_Pb[i] + ma_Pr[i]) * 1000.0 / Rr / (ma_ts[i] + 273.0);
            ma_CPT[i] = (-184.0 * ma_FSN[i] - 727.5) * log10(1.0 - ma_FSN[i] / 10.0);
            ma_mPT[i] = ma_CPT[i] * ma_Gexh[i] / ma_ror[i] / 1000.0;
            ma_gPT[i] = ma_mPT[i] / ma_Ne_netto[i];

            summ_mPT += ma_mPT[i] * ma_w[i];
            summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
        }

        m_gPTs = summ_mPT / summ_Ne_netto;

        summ_mPT = 0;
        summ_Ne_netto = 0;
    }

    if ( ptcalc == PTCALC_THROUGHPTMASS ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {

        double qmedfl = 0;
        double msep = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            if ( m_qmdwORrd == QMDWORRD_QMDW ) {
                ma_rd[i] = ma_qmdew[i] / (ma_qmdew[i] - ma_qmdw[i]);
            }
            else if ( m_qmdwORrd == QMDWORRD_RD ) {
                ma_qmdw[i] = (ma_qmdew[i] * ma_rd[i] - ma_qmdew[i]) / ma_rd[i];
            }

            ma_qmedf[i] = ma_Gexh[i] * ma_rd[i];
            qmedfl += ma_qmedf[i] * ma_w[i];
            ma_msepi[i] = ma_qmdew[i] * ma_tauf[i];
            msep += ma_msepi[i];
            summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
        }

        const double mPT = mf * qmedfl / msep;
        m_gPT = mPT / summ_Ne_netto;
    }
}

void txEmissionsOnR49R96::calc_Means() {

    double summ_Gfuel = 0;
    double summ_Ne_netto = 0;
    double summ_t0 = 0;
    double summ_B0 = 0;
    double summ_Ra = 0;

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        summ_Gfuel += ma_Gfuel[i] * ma_w[i];
        summ_Ne_netto += ma_Ne_netto[i] * ma_w[i];
        summ_t0 += ma_t0[i];
        summ_B0 += ma_B0[i];
        summ_Ra += ma_Ra[i];
    }

    m_geMean = summ_Gfuel / summ_Ne_netto * 1000.0;
    m_t0Mean = summ_t0 / n;
    m_B0Mean = summ_B0 / n;
    m_RaMean = summ_Ra / n;
}

QString txEmissionsOnR49R96::saveCheckoutData() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "CheckoutData_"
            + m_calculationOptions->defStandardName(
                m_calculationOptions->val_standard()
                )
            + "_"
            + m_currTime
            + ".dat";

    QFile choutData(fullPath);

    if ( !choutData.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&choutData);

    fout << right
         << qSetFieldWidth(COLUMNWIDTH);

    for ( ptrdiff_t i=0; i<SRCDATACAPTIONS_EMISSIONS.size(); i++ ) {
        fout << SRCDATACAPTIONS_EMISSIONS[i];
    }

    fout << "alpha[-]"     << "alpha_O2[-]" << "Pb[kPa]"
         << "Pa[kPa]"      << "fa[-]"       << "ge_b[g/kWh]"
         << "Ne_n[kW]"     << "Me_n[Nm]"    << "Gexh[kg/h]"
         << "Ha[g/kg]"     << "Gaird[kg/h]" << "Kw2[-]"
         << "Ffh[-]"       << "Kf[-]"       << "Kwr[-]"
         << "Khd[-]"       << "ge_n[g/kWh]";

    if ( m_NOxCalcMethod != -1 ) {
        fout << "mNOx[g/h]";
    }

    if ( m_gCOcalc == GCOCALC_YES ) {
        fout << "mCO[g/h]" << "gCO[g/kWh]";
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        fout << "mCH[g/h]" << "gCH[g/kWh]";
    }

    if ( m_gPTcalc == GPTCALC_YES ) {
        fout << "ror[kg/m3]"  << "CPT[mg/m3]" << "mPT[g/h]" << "gPT[g/kWh]"
             << "qmedf[kg/h]" << "msepi[g]";
    }

    if ( m_EGRcalc == EGRCALC_YES ) {
        fout << "rEGR[%]" << "alpha_res[-]";
    }

    if ( m_checkMeas == CHECKMEAS_YES ) {
        fout << "diff_alpha[%]";
    }

    fout << qSetFieldWidth(0)
         << "\n"
         << fixed
         << qSetFieldWidth(COLUMNWIDTH)
         << qSetRealNumberPrecision(PRECISION+1);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH);

        fout << i + 1;
        fout << ma_n[i]         << ma_Me_brutto[i] << ma_Ne_brutto[i]
             << ma_N_fan[i]     << ma_w[i]         << ma_t0[i]
             << ma_B0[i]        << ma_Ra[i]        << ma_dPn[i]
             << ma_Gair[i]      << ma_Gfuel[i]     << ma_CNOx[i]
             << ma_gNOx[i]      << ma_CCO[i]       << ma_CCH[i]
             << ma_CCO2in[i]    << ma_CCO2out[i]   << ma_CO2[i]
             << ma_Ka1m[i]      << ma_KaPerc[i]    << ma_FSN[i]
             << ma_Pr[i]        << ma_ts[i]        << ma_tauf[i]
             << ma_qmdw[i]      << ma_qmdew[i]     << ma_rd[i]
             << ma_alpha[i]     << ma_alpha_O2[i]  << ma_Pb[i]
             << ma_Pa[i]        << ma_fa[i]        << ma_ge_brutto[i]
             << ma_Ne_netto[i]  << ma_Me_netto[i]  << ma_Gexh[i]
             << ma_Ha[i]        << ma_Gaird[i]     << ma_Kw2[i]
             << ma_Ffh[i]       << ma_Kf[i]        << ma_Kwr[i]
             << ma_Khd[i]       << ma_ge_netto[i];

        if ( m_NOxCalcMethod != -1 ) {
            fout << ma_mNOx[i];
        }

        if ( m_gCOcalc == GCOCALC_YES ) {
            fout << ma_mCO[i] << ma_gCO[i];
        }

        if ( m_gCHcalc == GCHCALC_YES ) {
            fout << ma_mCH[i] << ma_gCH[i];
        }

        if ( m_gPTcalc == GPTCALC_YES ) {
            fout << ma_ror[i]   << ma_CPT[i] << ma_mPT[i] << ma_gPT[i]
                 << ma_qmedf[i] << ma_msepi[i];
        }

        if ( m_EGRcalc == EGRCALC_YES ) {
            fout << ma_rEGR[i] << ma_alpha_res[i];
        }

        if ( m_checkMeas == CHECKMEAS_YES ) {
            fout << ma_diff_alpha[i];
        }

        fout << qSetFieldWidth(0) << "\n";
    }

    choutData.close();

    return "libtoxic: Checkout data file \"" + fullPath + "\" created.\n";
}

QString txEmissionsOnR49R96::saveReportGAS() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "ReportGAS_"
            + m_calculationOptions->defStandardName(
                m_calculationOptions->val_standard()
                )
            + "_"
            + m_currTime
            + ".txt";

    QFile reportFile(fullPath);

    if ( !reportFile.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&reportFile);

    const int currstd = m_calculationOptions->val_standard();
    const int ptcalc = m_calculationOptions->val_PTcalc();

    fout << toxicIdentification{}.name()
         << " version "
         << toxicIdentification{}.version()
         << "    Report on cycle "
         << m_calculationOptions->defStandardName(currstd)
         << "    DateTime: "
         << m_currTime
         << "\n\n"
         << "Engine                 : ...\n";
    fout << fixed << qSetRealNumberPrecision(PRECISION);
    fout << "Environment parameters : t0_mean = "
         << m_t0Mean
         <<  " oC; B0_mean = "
         << m_B0Mean
         << " kPa; Ra_mean = "
         << m_RaMean
         << " %; "
         << m_testConditions
         << "\n"
         << "Calculation comments   : ";

    if ( m_GairVals == GAIRVALS_THROUGHMASSFLOW ) {
        fout << "direct Gair meas; ";
    }
    else if ( m_GairVals == GAIRVALS_THROUGHNOZZLE ) {
        fout << "Gair meas by nozzle (Dn = "
             << m_commonParameters->val_Dn()
             << "); ";
    }

    if ( m_calculationOptions->val_NOxSample() == NOXSAMPLE_WET ) {
        fout << "NOx sample type is wet; ";
    }
    else if ( m_calculationOptions->val_NOxSample() == NOXSAMPLE_DRY ) {
        fout << "NOx sample type is dry; ";
    }

    if ( ptcalc == PTCALC_THROUGHSMOKE ) {
        fout << "PT calc method based on smoke values;\n";
    }
    else if ( ptcalc == PTCALC_THROUGHPTMASS ) {
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << "PT calc method based on PT mass (mf = "
             << m_calculationOptions->val_PTmass()
             << " mg);\n";
    }
    else if ( ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << "PT calc method based on PT mass (mf = "
             << m_calculationOptions->val_PTmass()
             << " mg) and smoke values;\n";
    }
    else {
        fout << "PT emissions were not calculated;\n";
    }

    if ( m_checkMeas == CHECKMEAS_YES ) {
        fout << qSetRealNumberPrecision(PRECISION);
        fout << "                         "
             << "concentrations meas checked (conc O2air = "
             << m_commonParameters->val_concO2air()
             << " %)";
    }
    else {
        fout << "                         "
             << "concentrations meas were not checked";
    }

    fout << "\n\n";
    fout << right
         << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Point[-]"
         << "n[min-1]";

    if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
         currstd == STD_EU3 || currstd == STD_EU2 || currstd == STD_EU1 ||
         currstd == STD_EU0 ) {
        fout << "Ne_n[kW]";
    }
    else {
        fout << "Ne_b[kW]";
    }

    fout << "Gair[kg/h]"
         << "Gfuel[kg/h]";

    if ( m_EGRcalc == EGRCALC_YES ) {
        fout << "alpha_res[-]";
    }
    else {
        fout << "alpha[-]";
    }

    if ( m_smoke == SMOKE_KA1M ) {
        fout << "Ka[m-1]";
    }
    else if ( m_smoke == SMOKE_KAPERC ) {
        fout << "Ka[%]";
    }
    else if ( m_smoke == SMOKE_FSN ) {
        fout << "FSN[-]";
    }

    if ( m_NOxCalcMethod != -1 ) {
        fout << "C_NOx[ppm]"
             << "mNOx[g/h]"
             << "gNOx[g/kWh]";
    }

    fout << qSetFieldWidth(0)
         << "\n";

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(0);
        fout << (i + 1)
             << ma_n[i];
        fout << qSetRealNumberPrecision(PRECISION);

        if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
             currstd == STD_EU3 || currstd == STD_EU2 || currstd == STD_EU1 ||
             currstd == STD_EU0 ) {
            fout << ma_Ne_netto[i];
        }
        else {
            fout << ma_Ne_brutto[i];
        }

        fout << ma_Gair[i]
             << ma_Gfuel[i];

        if ( m_EGRcalc == EGRCALC_YES ) {
            fout << ma_alpha_res[i];
        }
        else {
            fout << ma_alpha[i];
        }

        fout << qSetRealNumberPrecision(PRECISION+1);

        if ( m_smoke == SMOKE_KA1M ) {
            fout << ma_Ka1m[i];
        }
        else if ( m_smoke == SMOKE_KAPERC ) {
            fout << ma_KaPerc[i];
        }
        else if ( m_smoke == SMOKE_FSN ) {
            fout << ma_FSN[i];
        }

        fout << qSetRealNumberPrecision(PRECISION);

        if ( m_NOxCalcMethod != -1 ) {
            fout << ma_CNOx[i]
                 << ma_mNOx[i]
                 << ma_gNOx[i];
        }

        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Point[-]";

    if ( m_gCOcalc == GCOCALC_YES ) {
        fout << "C_CO[ppm]"
             << "mCO[g/h]";
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        fout << "C_CH[ppm]"
             << "mCH[g/h]";
    }

    if ( ptcalc == PTCALC_THROUGHSMOKE ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
        fout << "gPTs[g/kWh]";
    }

    if ( m_EGRcalc == EGRCALC_YES ) {
        fout << "rEGR[%]";
    }

    if ( m_checkMeas == CHECKMEAS_YES ) {
        fout << "diff_alpha[%]";
    }

    fout << "fa[-]";
    fout << qSetFieldWidth(0)
         << "\n";

    for ( ptrdiff_t i=0; i<n; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(PRECISION);
        fout << (i + 1);

        if ( m_gCOcalc == GCOCALC_YES ) {
            fout << ma_CCO[i]
                 << ma_mCO[i];
        }

        if ( m_gCHcalc == GCHCALC_YES ) {
            fout << ma_CCH[i]
                 << ma_mCH[i];
        }

        if ( ptcalc == PTCALC_THROUGHSMOKE ||
             ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
            fout << ma_gPT[i];
        }

        if ( m_EGRcalc == EGRCALC_YES ) {
            fout << qSetRealNumberPrecision(PRECISION);
            fout << ma_rEGR[i];
        }

        if ( m_checkMeas == CHECKMEAS_YES ) {
            fout << ma_diff_alpha[i];
        }

        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << ma_fa[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetRealNumberPrecision(PRECISION);

    if ( ( currstd == STD_EU6 || currstd == STD_EU5 ||
           currstd == STD_EU4 || currstd == STD_EU3 ) &&
         m_calculationOptions->val_addPointsCalc() == ADDPOINTSCALC_YES ) {

        fout << "Additional points:\n\n";
        fout << qSetFieldWidth(COLUMNWIDTH-1);
        fout << "Point[-]"
             << "n[min-1]"
             << "Ne_n[kW]"
             << "Gair[kg/h]"
             << "Gfuel[kg/h]"
             << "alpha[-]"
             << "gNOm[g/kWh]"
             << "gNOc[g/kWh]"
             << "diff[%]";
        fout << qSetFieldWidth(0)
             << "\n"
             << qSetFieldWidth(COLUMNWIDTH-1);
        fout << (ESCPOINTSNUMBER+1)
             << ma_n[ESCPOINTSNUMBER]
             << ma_Ne_netto[ESCPOINTSNUMBER]
             << ma_Gair[ESCPOINTSNUMBER]
             << ma_Gfuel[ESCPOINTSNUMBER]
             << ma_alpha[ESCPOINTSNUMBER]
             << m_gNOx1m
             << m_gNOx1c
             << m_diffNOx1;
        fout << qSetFieldWidth(0)
             << "\n"
             << qSetFieldWidth(COLUMNWIDTH-1);
        fout << (ESCPOINTSNUMBER+2)
             << ma_n[ESCPOINTSNUMBER+1]
             << ma_Ne_netto[ESCPOINTSNUMBER+1]
             << ma_Gair[ESCPOINTSNUMBER+1]
             << ma_Gfuel[ESCPOINTSNUMBER+1]
             << ma_alpha[ESCPOINTSNUMBER+1]
             << m_gNOx2m
             << m_gNOx2c
             << m_diffNOx2;
        fout << qSetFieldWidth(0)
             << "\n"
             << qSetFieldWidth(COLUMNWIDTH-1);
        fout << (ESCPOINTSNUMBER+3)
             << ma_n[ESCPOINTSNUMBER+2]
             << ma_Ne_netto[ESCPOINTSNUMBER+2]
             << ma_Gair[ESCPOINTSNUMBER+2]
             << ma_Gfuel[ESCPOINTSNUMBER+2]
             << ma_alpha[ESCPOINTSNUMBER+2]
             << m_gNOx3m
             << m_gNOx3c
             << m_diffNOx3;
        fout << qSetFieldWidth(0)
             << "\n\n";
    }

    fout << "Specific emissions:\n\n";
    fout << qSetFieldWidth(COLUMNWIDTH-1+2)
         << " ";

    if ( currstd == STD_GOST17220597 ) {
        fout << qSetFieldWidth(COLUMNWIDTH-1);
        fout << "Limit (old1)"
             << "Limit (old2)"
             << "Limit (new1)"
             << "Limit (new2)";
    }
    else {
        fout << right << qSetFieldWidth(COLUMNWIDTH-1);
        fout << "Limitation";
    }

    fout << "CalcResult"
         << "Conclusion";
    fout << qSetFieldWidth(0)
         << "\n";

    if ( currstd == STD_R96H8 || currstd == STD_R96I8 ||
         currstd == STD_R96J8 || currstd == STD_R96K8 ||
         currstd == STD_R96H5 || currstd == STD_R96I5 ||
         currstd == STD_R96J5 || currstd == STD_R96K5 ) {

        const double gNOxCHLimit = NOxCHLimit(currstd);
        const double gCOLimit = COLimit(currstd);

        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gNOx+gCH[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(PRECISION+1);
        fout << gNOxCHLimit
             << m_gNOx + m_gCH;

        if ( (m_gNOx + m_gCH) <= gNOxCHLimit ) {
            fout << "OK";
        }
        else {
            fout << "failed";
        }

        fout << qSetFieldWidth(0)
             << "\n"
             << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gNOx[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH+COLUMNWIDTH-2);
        fout << m_gNOx;
        fout << qSetFieldWidth(0)
             << "\n";
        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gCH[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH+COLUMNWIDTH-2);
        fout << m_gCH;
        fout << qSetFieldWidth(0)
             << "\n";

        if ( m_gCOcalc == GCOCALC_YES ) {

            fout << qSetFieldWidth(COLUMNWIDTH-1+2);
            fout << "gCO[g/kWh]";
            fout << qSetFieldWidth(COLUMNWIDTH-1);
            fout << gCOLimit
                 << m_gCO;

            if ( m_gCO <= gCOLimit ) {
                fout << "OK";
            }
            else {
                fout << "failed";
            }

            fout << qSetFieldWidth(0)
                 << "\n";
        }
    }
    else {

        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gNOx[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(PRECISION+1);

        const double gNOxLimit = NOxLimit(currstd);

        if ( currstd == STD_GOST17220597 ) {
            fout << NOxLimit(currstd+30)
                 << NOxLimit(currstd+40)
                 << NOxLimit(currstd+10)
                 << NOxLimit(currstd+20);
        }
        else {
            fout << gNOxLimit;
        }

        fout << m_gNOx;

        if ( currstd != STD_GOST17220597 ) {

            if ( m_gNOx <= gNOxLimit ) {
                fout << "OK";
            }
            else {
                fout << "failed";
            }
        }

        fout << qSetFieldWidth(0)
             << "\n";

        const double gCOLimit = COLimit(currstd);

        if ( m_gCOcalc == GCOCALC_YES ) {

            fout << qSetFieldWidth(COLUMNWIDTH-1+2);
            fout << "gCO[g/kWh]";
            fout << qSetFieldWidth(COLUMNWIDTH-1);

            if ( currstd == STD_GOST17220597 ) {
                fout << COLimit(currstd+30)
                     << COLimit(currstd+40)
                     << COLimit(currstd+10)
                     << COLimit(currstd+20);
            }
            else {
                fout << gCOLimit;
            }

            fout << m_gCO;

            if ( currstd != STD_GOST17220597 ) {

                if ( m_gCO <= gCOLimit ) {
                    fout << "OK";
                }
                else {
                    fout << "failed";
                }
            }

            fout << qSetFieldWidth(0)
                 << "\n";
        }

        const double gCHLimit = CHLimit(currstd);

        if ( m_gCHcalc == GCHCALC_YES ) {

            fout << qSetFieldWidth(COLUMNWIDTH-1+2);
            fout << "gCH[g/kWh]";
            fout << qSetFieldWidth(COLUMNWIDTH-1);

            if ( currstd == STD_GOST17220597 ) {
                fout << CHLimit(currstd+30)
                     << CHLimit(currstd+40)
                     << CHLimit(currstd+10)
                     << CHLimit(currstd+20);
            }
            else {
                fout << gCHLimit;
            }

            fout << m_gCH;

            if ( currstd != STD_GOST17220597 ) {

                if ( m_gCH <= gCHLimit ) {
                    fout << "OK";
                }
                else {
                    fout << "failed";
                }
            }

            fout << qSetFieldWidth(0)
                 << "\n";
        }
    }

    if ( m_gPTcalc == GPTCALC_YES ) {

        const double gPTLimit = PTLimit(currstd);

        if ( ptcalc == PTCALC_THROUGHPTMASS ||
             ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {

            fout << qSetFieldWidth(COLUMNWIDTH-1+2);
            fout << "gPT[g/kWh]";
            fout << qSetFieldWidth(COLUMNWIDTH-1);
            fout << gPTLimit
                 << m_gPT;

            if ( m_gPT <= gPTLimit ) {
                fout << "OK";
            }
            else {
                fout << "failed";
            }

            fout << qSetFieldWidth(0)
                 << "\n";
        }

        if ( ptcalc == PTCALC_THROUGHSMOKE ||
             ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {

            fout << qSetFieldWidth(COLUMNWIDTH-1+2);
            fout << "gPTs[g/kWh]";
            fout << qSetFieldWidth(COLUMNWIDTH+COLUMNWIDTH-2);
            fout << m_gPTs;
            fout << qSetFieldWidth(0)
                 << "\n";
        }
    }

    fout << "\n"
         << qSetRealNumberPrecision(PRECISION);
    fout << "Mean specific fuel consumption: "
         << m_geMean
         << " g/kWh";

    reportFile.close();

    return "libtoxic: Report file \"" + fullPath + "\" created.\n";
}

QString txEmissionsOnR49R96::saveReportPT() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "ReportPT_"
            + m_calculationOptions->defStandardName(
                m_calculationOptions->val_standard()
                )
            + "_"
            + m_currTime
            + ".txt";

    QFile reportFile(fullPath);

    if ( !reportFile.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&reportFile);

    const int currstd = m_calculationOptions->val_standard();
    const int ptcalc = m_calculationOptions->val_PTcalc();

    fout << toxicIdentification{}.name()
         << " version "
         << toxicIdentification{}.version()
         << "    Report on cycle "
         << m_calculationOptions->defStandardName(currstd)
         << "    DateTime: "
         << m_currTime
         << "\n\n"
         << "Engine                 : ...\n";
    fout << fixed << qSetRealNumberPrecision(PRECISION);
    fout << "Environment parameters : t0_mean = "
         << m_t0Mean
         <<  " oC; B0_mean = "
         << m_B0Mean
         << " kPa; Ra_mean = "
         << m_RaMean
         << " %; "
         << m_testConditions
         << "\n"
         << "Calculation comments   : ";

    if ( m_GairVals == GAIRVALS_THROUGHMASSFLOW ) {
        fout << "direct Gair meas; ";
    }
    else if ( m_GairVals == GAIRVALS_THROUGHNOZZLE ) {
        fout << "Gair meas by nozzle (Dn = "
             << m_commonParameters->val_Dn()
             << "); ";
    }

    if ( ptcalc == PTCALC_THROUGHSMOKE ) {
        fout << "PT calc method based on smoke values";
    }
    else if ( ptcalc == PTCALC_THROUGHPTMASS ) {
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << "PT calc method based on PT mass (mf = "
             << m_calculationOptions->val_PTmass()
             << " mg)";
    }
    else if ( ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << "PT calc method based on PT mass (mf = "
             << m_calculationOptions->val_PTmass()
             << " mg) and smoke values";
    }
    else {
        fout << "PT emissions were not calculated";
    }

    fout << "\n\n";
    fout << right
         << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Point[-]"
         << "n[min-1]";

    if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
         currstd == STD_EU3 || currstd == STD_EU2 || currstd == STD_EU1 ||
         currstd == STD_EU0 ) {
        fout << "Ne_n[kW]";
    }
    else {
        fout << "Ne_b[kW]";
    }

    fout << "Gair[kg/h]"
         << "Gfuel[kg/h]";

    if ( m_EGRcalc == EGRCALC_YES ) {
        fout << "alpha_res[-]";
    }
    else {
        fout << "alpha[-]";
    }

    if ( m_smoke == SMOKE_KA1M ) {
        fout << "Ka[m-1]";
    }
    else if ( m_smoke == SMOKE_KAPERC ) {
        fout << "Ka[%]";
    }
    else if ( m_smoke == SMOKE_FSN ) {
        fout << "FSN[-]";
    }

    fout << "tauf[s]"
         << "qmdw[g/s]"
         << "qmdew[g/s]";
    fout << qSetFieldWidth(0)
         << "\n";

    double n = 0;
    if ( m_numberOfPoints == ESCPOINTSNUMBER+ESCADDPOINTSNUMBER ) {
        n = ESCPOINTSNUMBER;
    }
    else {
        n = m_numberOfPoints;
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(0);
        fout << (i + 1)
             << ma_n[i];
        fout << qSetRealNumberPrecision(PRECISION);

        if ( currstd == STD_EU6 || currstd == STD_EU5 || currstd == STD_EU4 ||
             currstd == STD_EU3 || currstd == STD_EU2 || currstd == STD_EU1 ||
             currstd == STD_EU0 ) {
            fout << ma_Ne_netto[i];
        }
        else {
            fout << ma_Ne_brutto[i];
        }

        fout << ma_Gair[i]
             << ma_Gfuel[i];

        if ( m_EGRcalc == EGRCALC_YES ) {
            fout << ma_alpha_res[i];
        }
        else {
            fout << ma_alpha[i];
        }

        fout << qSetRealNumberPrecision(PRECISION+1);

        if ( m_smoke == SMOKE_KA1M ) {
            fout << ma_Ka1m[i];
        }
        else if ( m_smoke == SMOKE_KAPERC ) {
            fout << ma_KaPerc[i];
        }
        else if ( m_smoke == SMOKE_FSN ) {
            fout << ma_FSN[i];
        }

        fout << qSetRealNumberPrecision(PRECISION);
        fout << ma_tauf[i];
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << ma_qmdw[i]
             << ma_qmdew[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Point[-]"
         << "rd[-]"
         << "qmedf[kg/h]"
         << "msepi[g]";
    fout << qSetFieldWidth(0)
         << "\n";

    for ( ptrdiff_t i=0; i<n; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(0);
        fout << (i + 1);
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << ma_rd[i];
        fout << qSetRealNumberPrecision(PRECISION);
        fout << ma_qmedf[i];
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << ma_msepi[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetFieldWidth(COLUMNWIDTH-1+2);
    fout << " ";
    fout << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Limitation"
         << "CalcResult"
         << "Conclusion";
    fout << qSetFieldWidth(0)
         << "\n";

    const double gPTLimit = PTLimit(currstd);

    fout << qSetFieldWidth(COLUMNWIDTH-1+2);
    fout << "gPT[g/kWh]";
    fout << qSetFieldWidth(COLUMNWIDTH-1);
    fout << gPTLimit
         << m_gPT;

    if ( m_gPT <= gPTLimit ) {
        fout << "OK";
    }
    else {
        fout << "failed";
    }

    fout << qSetFieldWidth(0)
         << "\n";

    if ( ptcalc == PTCALC_THROUGHSMOKE ||
         ptcalc == PTCALC_THROUGHSMOKEANDPTMASS ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gPTs[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH+COLUMNWIDTH-2)
             << qSetRealNumberPrecision(PRECISION+1);
        fout << m_gPTs;
        fout << qSetFieldWidth(0)
             << "\n\n";
    }

    reportFile.close();

    return "libtoxic: Report file \"" + fullPath + "\" created.\n";
}

} // namespace toxic
