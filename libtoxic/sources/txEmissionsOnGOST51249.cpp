/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnGOST51249.cpp

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

#include "txEmissionsOnGOST51249.h"
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

namespace toxic {

txEmissionsOnGOST51249::txEmissionsOnGOST51249(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts
        ) :
    m_NOxCalcMethod(NOXCALCMETHOD_DIRECT),
    m_gCOcalc(GCOCALC_YES),
    m_gCHcalc(GCHCALC_YES),
    m_smoke(SMOKE_KA1M),
    m_gNOx(0),
    m_gCO(0),
    m_gCH(0),
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

txEmissionsOnGOST51249::~txEmissionsOnGOST51249() {
}

void txEmissionsOnGOST51249::setSourceData(
        const QVector<QVector<double> > &srcdata
        ) {

    m_numberOfPoints = srcdata.size();

    checkSrcData();
    prepSrcArrays(srcdata);
}

void txEmissionsOnGOST51249::setSourceData() {

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

void txEmissionsOnGOST51249::calculate() {

    setupCalc();
    baseCalc();
    prelimCalc();

    if ( m_NOxCalcMethod != -1 ) {
        calc_gNOx();
    }

    if ( m_gCOcalc == GCOCALC_YES ) {
        calc_gCO();
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        calc_gCH();
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

QString txEmissionsOnGOST51249::results() const {

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

    res += "\n" + m_testConditions + "\n";

    return res;
}

QString txEmissionsOnGOST51249::createReports() {

    m_fullReportsPath =
            m_commonParameters->val_reportsDirName()
            + QDir::separator()
            + m_calculationOptions->defStandardName(m_calculationOptions->
                                                    val_standard())
            + "_"
            + m_currTime;

    QDir reportDir;
    reportDir.mkdir(m_fullReportsPath);

    return saveSourceData()
            + saveCheckoutData()
            + saveCalcOptions()
            + saveReportGAS();
}

void txEmissionsOnGOST51249::checkSrcData() {

    const int currstd = m_calculationOptions->val_standard();

    if ( currstd < STD_C1 || currstd > STD_G2 ) {
        throw txError("Wrong source data or calculation settings! "
                      "This library module is not suitable for the data at "
                      "the specified standard! "
                      "If you see this message, your program uses libtoxic "
                      "incorrectly!");
    }

    if (
            (
                (
                    (currstd == STD_C1)
                    ) &&
                (
                    (m_numberOfPoints != C1POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_D1)
                    ) &&
                (
                    (m_numberOfPoints != D1POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_D2)
                    ) &&
                (
                    (m_numberOfPoints != D2POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_E1)
                    ) &&
                (
                    (m_numberOfPoints != E1POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_E2)
                    ) &&
                (
                    (m_numberOfPoints != E2POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_E3)
                    ) &&
                (
                    (m_numberOfPoints != E3POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_E5)
                    ) &&
                (
                    (m_numberOfPoints != E5POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_F)
                    ) &&
                (
                    (m_numberOfPoints != FPOINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_G1)
                    ) &&
                (
                    (m_numberOfPoints != G1POINTSNUMBER)
                    )
                ) ||
            (
                (
                    (currstd == STD_G2)
                    ) &&
                (
                    (m_numberOfPoints != G2POINTSNUMBER)
                    )
                )
            ) {

        throw txError("Wrong source data or calculation settings! "
                      "Check the number of points of source data and "
                      "calculation parameters.");
    }
}

void txEmissionsOnGOST51249::setupCalc() {

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
        throw txError("Reverse calculation for NOx is impossible for "
                      "selected standard!");
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

    if ( zeroArray(ma_CCO2in) && zeroArray(ma_CCO2out) ) {
        m_EGRcalc = EGRCALC_NO;
    }
    else {
        m_EGRcalc = EGRCALC_YES;
    }

    if ( zeroArray(ma_CO2) ) {
        m_checkMeas = CHECKMEAS_NO;
    }
    else {
        m_checkMeas = CHECKMEAS_YES;
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

void txEmissionsOnGOST51249::prelimCalc() {

    ma_Gexh.clear();      ma_Gexh.resize(m_numberOfPoints);
    ma_Gexhd.clear();     ma_Gexhd.resize(m_numberOfPoints);

    double Ffw = 0;
    double Ffd = 0;
    const int fueltype = m_calculationOptions->val_fuelType();

    if ( fueltype == FUELTYPE_DIESEL ) {
        Ffw = 0.75;
        Ffd = -0.77;
    }
    else if ( fueltype == FUELTYPE_MOTOR ) {
        Ffw = 0.72;
        Ffd = -0.74;
    }
    else if ( fueltype == FUELTYPE_MAZUT ) {
        Ffw = 0.69;
        Ffd = -0.71;
    }
    else {
        throw txError("Incorrect fuel type!");
    }

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        ma_Gexh[i] = (2.695780903 * ma_B0[i] *
                      (ma_Gair[i] / rhoAir(ma_B0[i], ma_t0[i])) /
                      (ma_t0[i] + 273)) + Ffw * ma_Gfuel[i];
        ma_Gexhd[i] = (2.695780903 * ma_B0[i] *
                       (ma_Gair[i] / rhoAir(ma_B0[i], ma_t0[i])) /
                       (ma_t0[i] + 273)) + Ffd * ma_Gfuel[i];
    }
}

void txEmissionsOnGOST51249::calc_gNOx() {

    const int noxsample = m_calculationOptions->val_NOxSample();
    double summnum = 0;
    double summdenom = 0;

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( noxsample == NOXSAMPLE_WET ) {
            summnum += ma_CNOx[i] / 10000.0 * ma_Gexh[i] * ma_w[i];
        }
        else if ( noxsample == NOXSAMPLE_DRY ) {
            summnum += ma_CNOx[i] / 10000.0 * ma_Gexhd[i] * ma_w[i];
        }
        else {
            throw txError("Incorrect NOx sample type!");
        }

        summdenom += ma_Ne_brutto[i] / ma_Ne_brutto[0] * ma_w[i];
    }

    m_gNOx = 0.446 * m_commonParameters->val_muNO2() * summnum /
            (ma_Ne_brutto[0] * summdenom);
}

void txEmissionsOnGOST51249::calc_gCO() {

    double summnum = 0;
    double summdenom = 0;

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        summnum += ma_CCO[i] / 10000.0 * ma_Gexhd[i] * ma_w[i];
        summdenom += ma_Ne_brutto[i] / ma_Ne_brutto[0] * ma_w[i];
    }

    m_gCO = 0.446 * m_commonParameters->val_muCO() * summnum /
            (ma_Ne_brutto[0] * summdenom);
}

void txEmissionsOnGOST51249::calc_gCH() {

    double summnum = 0;
    double summdenom = 0;

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        summnum += ma_CCH[i] / 10000.0 * ma_Gexh[i] * ma_w[i];
        summdenom += ma_Ne_brutto[i] / ma_Ne_brutto[0] * ma_w[i];
    }

    m_gCH = 0.446 * m_commonParameters->val_muCH() * summnum /
            (ma_Ne_brutto[0] * summdenom);
}

void txEmissionsOnGOST51249::calc_Means() {

    double summ_Gfuel = 0;
    double summ_Ne_brutto = 0;
    double summ_t0 = 0;
    double summ_B0 = 0;
    double summ_Ra = 0;

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        summ_Gfuel += ma_Gfuel[i] * ma_w[i];
        summ_Ne_brutto += ma_Ne_brutto[i] * ma_w[i];
        summ_t0 += ma_t0[i];
        summ_B0 += ma_B0[i];
        summ_Ra += ma_Ra[i];
    }

    m_geMean = summ_Gfuel / summ_Ne_brutto * 1000.0;
    m_t0Mean = summ_t0 / m_numberOfPoints;
    m_B0Mean = summ_B0 / m_numberOfPoints;
    m_RaMean = summ_Ra / m_numberOfPoints;
}

QString txEmissionsOnGOST51249::saveCheckoutData() const {

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

    fout << "Ne_n[kW]"     << "Me_n[Nm]"    << "alpha[-]"
         << "alpha_O2[-]"  << "Gexh[m3/h]"  << "Gexhd[m3/h]"
         << "Pb[kPa]"      << "Pa[kPa]"     << "fa[-]"
         << "ge_b[g/kWh]"  << "ge_n[g/kWh]" << "rEGR[%]"
         << "alpha_res[-]" << "diff_alpha[%]";

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
             << ma_alpha[i]     << ma_alpha_O2[i]  << ma_Gexh[i]
             << ma_Gexhd[i]     << ma_Pb[i]        << ma_Pa[i]
             << ma_fa[i]        << ma_ge_brutto[i] << ma_rEGR[i]
             << ma_alpha_res[i] << ma_diff_alpha[i];
        fout << qSetFieldWidth(0) << "\n";
    }

    choutData.close();

    return "libtoxic: Additional file \"" + fullPath + "\" created.\n";
}

QString txEmissionsOnGOST51249::saveReportGAS() const {

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

    fout << toxicName
         << " version "
         << toxicVersion
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

    if ( m_checkMeas == CHECKMEAS_YES ) {
        fout << qSetRealNumberPrecision(PRECISION);
        fout << "concentrations meas checked (conc O2air = "
             << m_commonParameters->val_concO2air()
             << " %)";
    }
    else {
        fout << "concentrations meas were not checked";
    }

    fout << "\n\n";
    fout << right
         << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Point[-]"
         << "n[min-1]"
         << "Ne_b[kW]"
         << "Gair[kg/h]"
         << "Gfuel[kg/h]";

    if ( m_EGRcalc == EGRCALC_YES ) {
        fout << "alpha_res[-]";
    }
    else {
        fout << "alpha[-]";
    }

    if ( m_NOxCalcMethod != -1 ) {
        fout << "C_NOx[ppm]";
    }

    fout << qSetFieldWidth(0)
         << "\n";

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(0);
        fout << (i + 1)
             << ma_n[i];
        fout << qSetRealNumberPrecision(PRECISION);
        fout << ma_Ne_brutto[i]
             << ma_Gair[i]
             << ma_Gfuel[i];

        if ( m_EGRcalc == EGRCALC_YES ) {
            fout << ma_alpha_res[i];
        }
        else {
            fout << ma_alpha[i];
        }

        if ( m_NOxCalcMethod != -1 ) {
            fout << ma_CNOx[i];
        }

        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetFieldWidth(COLUMNWIDTH-1);

    fout << "Point[-]";

    if ( m_gCOcalc == GCOCALC_YES ) {
        fout << "C_CO[ppm]";
    }

    if ( m_gCHcalc == GCHCALC_YES ) {
        fout << "C_CH[ppm]";
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

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1)
             << qSetRealNumberPrecision(PRECISION);
        fout << (i + 1);

        if ( m_gCOcalc == GCOCALC_YES ) {
            fout << ma_CCO[i];
        }

        if ( m_gCHcalc == GCHCALC_YES ) {
            fout << ma_CCH[i];
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
    fout << "Specific emissions:\n\n";
    fout << qSetFieldWidth(COLUMNWIDTH-1+2)
         << " ";
    fout << qSetFieldWidth(COLUMNWIDTH-1);
    fout << "Limit (old)"
         << "Limit (new)"
         << "CalcResult";
    fout << qSetFieldWidth(0)
         << "\n";
    fout << qSetFieldWidth(COLUMNWIDTH-1+2)
         << "gNOx[g/kWh]";
    fout << qSetFieldWidth(COLUMNWIDTH-1)
         << qSetRealNumberPrecision(PRECISION+1);

    double gNOxLimit1 = 0;
    double gNOxLimit2 = 0;

    if ( currstd == STD_C1 || currstd == STD_D1 || currstd == STD_D2 ||
         currstd == STD_F  || currstd == STD_G1 || currstd == STD_G2 ) {

        gNOxLimit1 = NOxLimit(currstd+20);
        gNOxLimit2 = NOxLimit(currstd+10);
    }
    else if ( currstd == STD_E1 || currstd == STD_E2 ||
              currstd == STD_E3 || currstd == STD_E5 ) {

        gNOxLimit1 = NOxLimit(currstd+20, ma_n[0]);
        gNOxLimit2 = NOxLimit(currstd+10, ma_n[0]);
    }

    fout << gNOxLimit1
         << gNOxLimit2
         << m_gNOx;
    fout << qSetFieldWidth(0)
         << "\n";

    if ( m_gCOcalc == GCOCALC_YES ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gCO[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH-1);
        fout << COLimit(currstd+20)
             << COLimit(currstd+10)
             << m_gCO;
        fout << qSetFieldWidth(0)
             << "\n";
    }

    if ( m_gCOcalc == GCOCALC_YES ) {

        fout << qSetFieldWidth(COLUMNWIDTH-1+2);
        fout << "gCH[g/kWh]";
        fout << qSetFieldWidth(COLUMNWIDTH-1);
        fout << CHLimit(currstd+20)
             << CHLimit(currstd+10)
             << m_gCH;
        fout << qSetFieldWidth(0)
             << "\n";
    }

    fout << "\n"
         << qSetRealNumberPrecision(PRECISION);
    fout << "Mean specific fuel consumption: "
         << m_geMean
         << " g/kWh\n";

    reportFile.close();

    return "libtoxic: Report file \"" + fullPath + "\" created.\n";
}

} // namespace toxic
