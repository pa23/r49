/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsBase.cpp

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

#include "txEmissionsBase.h"
#include "txError.h"
#include "txConstants.h"
#include "txAuxiliaryFunctions.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

namespace toxic {

txEmissionsBase::txEmissionsBase() :
    m_NeCalcMethod(NECALCMETHOD_THROUGHME),
    m_GairVals(GAIRVALS_THROUGHMASSFLOW),
    m_EGRcalc(EGRCALC_NO),
    m_checkMeas(CHECKMEAS_NO),
    m_numberOfPoints(0) {
}

txEmissionsBase::~txEmissionsBase() {
}

void txEmissionsBase::prepSrcArrays(
        const QVector< QVector<double> > &srcdata
        ) {

    if ( srcdata.isEmpty() ) {
        throw txError("Incorrect source data array!");
    }

    ma_n.clear();         ma_n.resize(m_numberOfPoints);
    ma_Me_brutto.clear(); ma_Me_brutto.resize(m_numberOfPoints);
    ma_Ne_brutto.clear(); ma_Ne_brutto.resize(m_numberOfPoints);
    ma_N_fan.clear();     ma_N_fan.resize(m_numberOfPoints);
    ma_w.clear();         ma_w.resize(m_numberOfPoints);
    ma_t0.clear();        ma_t0.resize(m_numberOfPoints);
    ma_B0.clear();        ma_B0.resize(m_numberOfPoints);
    ma_Ra.clear();        ma_Ra.resize(m_numberOfPoints);
    ma_dPn.clear();       ma_dPn.resize(m_numberOfPoints);
    ma_Gair.clear();      ma_Gair.resize(m_numberOfPoints);
    ma_Gfuel.clear();     ma_Gfuel.resize(m_numberOfPoints);
    ma_CNOx.clear();      ma_CNOx.resize(m_numberOfPoints);
    ma_gNOx.clear();      ma_gNOx.resize(m_numberOfPoints);
    ma_CCO.clear();       ma_CCO.resize(m_numberOfPoints);
    ma_CCH.clear();       ma_CCH.resize(m_numberOfPoints);
    ma_CCO2in.clear();    ma_CCO2in.resize(m_numberOfPoints);
    ma_CCO2out.clear();   ma_CCO2out.resize(m_numberOfPoints);
    ma_CO2.clear();       ma_CO2.resize(m_numberOfPoints);
    ma_Ka1m.clear();      ma_Ka1m.resize(m_numberOfPoints);
    ma_KaPerc.clear();    ma_KaPerc.resize(m_numberOfPoints);
    ma_FSN.clear();       ma_FSN.resize(m_numberOfPoints);
    ma_Pr.clear();        ma_Pr.resize(m_numberOfPoints);
    ma_ts.clear();        ma_ts.resize(m_numberOfPoints);
    ma_tauf.clear();      ma_tauf.resize(m_numberOfPoints);
    ma_qmdw.clear();      ma_qmdw.resize(m_numberOfPoints);
    ma_qmdew.clear();     ma_qmdew.resize(m_numberOfPoints);
    ma_rd.clear();        ma_rd.resize(m_numberOfPoints);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( srcdata[i].size() != SRCDATACAPTIONS_EMISSIONS.size() ) {
            throw txError("Incorrect source data array!");
        }

        ma_n        [i] = srcdata[i][ 1];
        ma_Me_brutto[i] = srcdata[i][ 2];
        ma_Ne_brutto[i] = srcdata[i][ 3];
        ma_N_fan    [i] = srcdata[i][ 4];
        ma_w        [i] = srcdata[i][ 5];
        ma_t0       [i] = srcdata[i][ 6];
        ma_B0       [i] = srcdata[i][ 7];
        ma_Ra       [i] = srcdata[i][ 8];
        ma_dPn      [i] = srcdata[i][ 9];
        ma_Gair     [i] = srcdata[i][10];
        ma_Gfuel    [i] = srcdata[i][11];
        ma_CNOx     [i] = srcdata[i][12];
        ma_gNOx     [i] = srcdata[i][13];
        ma_CCO      [i] = srcdata[i][14];
        ma_CCH      [i] = srcdata[i][15];
        ma_CCO2in   [i] = srcdata[i][16];
        ma_CCO2out  [i] = srcdata[i][17];
        ma_CO2      [i] = srcdata[i][18];
        ma_Ka1m     [i] = srcdata[i][19];
        ma_KaPerc   [i] = srcdata[i][20];
        ma_FSN      [i] = srcdata[i][21];
        ma_Pr       [i] = srcdata[i][22];
        ma_ts       [i] = srcdata[i][23];
        ma_tauf     [i] = srcdata[i][24];
        ma_qmdw     [i] = srcdata[i][25];
        ma_qmdew    [i] = srcdata[i][26];
        ma_rd       [i] = srcdata[i][27];
    }
}

void txEmissionsBase::baseCalc() {

    ma_alpha.clear();     ma_alpha.resize(m_numberOfPoints);
    ma_alpha_O2.clear();  ma_alpha_O2.resize(m_numberOfPoints);
    ma_Pb.clear();        ma_Pb.resize(m_numberOfPoints);
    ma_Pa.clear();        ma_Pa.resize(m_numberOfPoints);
    ma_fa.clear();        ma_fa.resize(m_numberOfPoints);
    ma_ge_brutto.clear(); ma_ge_brutto.resize(m_numberOfPoints);

    const double Dn = m_commonParameters->val_Dn();
    if ( Dn < 1 ) {
        throw txError("Incorrect \"Dn\" parameter!");
    }
    const double concO2air = m_commonParameters->val_concO2air();
    if ( concO2air < 1 ) {
        throw txError("Incorrect \"concO2air\" parameter!");
    }
    const int chargtype = m_calculationOptions->val_chargingType();

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( m_NeCalcMethod == NECALCMETHOD_THROUGHNE ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }
        else if ( m_NeCalcMethod == NECALCMETHOD_THROUGHME ) {
            ma_Ne_brutto[i] = ma_Me_brutto[i] * ma_n[i] / 9550.0;
        }
        else {
            throw txError("Incorrect Ne_netto calculation method!");
        }

        if ( m_GairVals == GAIRVALS_THROUGHNOZZLE ) {
            ma_Gair[i] = Gair(Dn, ma_B0[i], ma_t0[i], ma_dPn[i]);
        }

        ma_alpha[i] = ma_Gair[i] / (ma_Gfuel[i] * m_commonParameters->val_L0());

        if ( m_checkMeas == CHECKMEAS_YES && m_EGRcalc == EGRCALC_NO ) {
            ma_alpha_O2[i] = (0.01 * concO2air * (1.0 - 0.01 * ma_CO2[i]) +
                              0.0683185 * 0.01 * ma_CO2[i]) /
                    (0.01 * concO2air - 0.01 * ma_CO2[i]);
        }

        ma_Pb[i] = ma_B0[i];
        ma_Pa[i] = pa(ma_t0[i]);

        if ( chargtype == CHARGINGTYPE_NO ) {
            ma_fa[i] = (99.0 / (ma_Pb[i] - ma_Ra[i] * ma_Pa[i] * 0.01)) *
                    pow((ma_t0[i] + 273.0) / 298, 0.7);
        }
        else if ( chargtype == CHARGINGTYPE_GASTURBINE ) {
            ma_fa[i] = pow(99.0 / (ma_Pb[i] - ma_Ra[i] * ma_Pa[i] * 0.01), 0.7) *
                    pow((ma_t0[i] + 273.0) / 298, 1.5);
        }
        else {
            throw txError("Incorrect charging type!");
        }

        ma_ge_brutto[i] = ma_Gfuel[i] / ma_Ne_brutto[i] * 1000.0;
    }
}

void txEmissionsBase::calc_rEGR() {

    ma_rEGR.clear();      ma_rEGR.resize(m_numberOfPoints);
    ma_alpha_res.clear(); ma_alpha_res.resize(m_numberOfPoints);

    const double concCO2air = m_commonParameters->val_concCO2air();

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        ma_rEGR[i] = (ma_CCO2in[i] - concCO2air) /
                (ma_CCO2out[i] - concCO2air) * 100.0;
        ma_alpha_res[i] = (ma_alpha[i] - ma_alpha[i] * ma_rEGR[i] / 100.0 -
                           ma_rEGR[i] / 100.0) / (1.0 - 2.0 * ma_rEGR[i] / 100.0);
    }
}

void txEmissionsBase::compareAlpha() {

    ma_diff_alpha.clear(); ma_diff_alpha.resize(m_numberOfPoints);

    if ( m_EGRcalc == EGRCALC_YES ) {

        double concO2mix = 0;
        const double concO2air = m_commonParameters->val_concO2air();
        if ( concO2air < 1 ) {
            throw txError("Incorrect \"concO2air\" parameter!");
        }

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

            concO2mix = concO2air * (1.0 - ma_rEGR[i] / 100.0) +
                    ma_CO2[i] * ma_rEGR[i] / 100.0;
            ma_alpha_O2[i] = (0.01 * concO2mix * (1.0 - 0.01 * ma_CO2[i]) +
                              0.0683185 * 0.01 * ma_CO2[i]) /
                    (0.01 * concO2mix - 0.01 * ma_CO2[i]);
            ma_diff_alpha[i] = (ma_alpha_res[i] - ma_alpha_O2[i]) /
                    ma_alpha_O2[i] * 100.0;
        }
    }
    else if ( m_EGRcalc == EGRCALC_NO ) {

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

            ma_diff_alpha[i] = (ma_alpha[i] - ma_alpha_O2[i]) /
                    ma_alpha_O2[i] * 100.0;
        }
    }
    else {
        throw txError("Incorrect task for EGR ratio calculation!");
    }
}

void txEmissionsBase::checkTestConditions() {

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( (ma_fa[i] < 0.96) || (ma_fa[i] > 1.06) ) {
            m_testConditions = "Check test conditions: FAILED.";
            return;
        }
    }

    m_testConditions = "Check test conditions: OK.";
}

QString txEmissionsBase::saveSourceData() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "SourceData_"
            + m_calculationOptions->defStandardName(
                m_calculationOptions->val_standard()
                )
            + "_"
            + m_currTime
            + ".dat";

    QFile srcData(fullPath);

    if ( !srcData.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&srcData);

    fout << right
         << qSetFieldWidth(COLUMNWIDTH);

    for ( ptrdiff_t i=0; i<SRCDATACAPTIONS_EMISSIONS.size(); i++ ) {
        fout << SRCDATACAPTIONS_EMISSIONS[i];
    }

    fout << qSetFieldWidth(0)
         << "\n"
         << fixed
         << qSetFieldWidth(COLUMNWIDTH)
         << qSetRealNumberPrecision(PRECISION+1);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH);
        fout << i + 1;
        fout << ma_n[i]      << ma_Me_brutto[i] << ma_Ne_brutto[i]
             << ma_N_fan[i]  << ma_w[i]         << ma_t0[i]
             << ma_B0[i]     << ma_Ra[i]        << ma_dPn[i]
             << ma_Gair[i]   << ma_Gfuel[i]     << ma_CNOx[i]
             << ma_gNOx[i]   << ma_CCO[i]       << ma_CCH[i]
             << ma_CCO2in[i] << ma_CCO2out[i]   << ma_CO2[i]
             << ma_Ka1m[i]   << ma_KaPerc[i]    << ma_FSN[i]
             << ma_Pr[i]     << ma_ts[i]        << ma_tauf[i]
             << ma_qmdw[i]   << ma_qmdew[i]     << ma_rd[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    srcData.close();

    return "libtoxic: Source data file \"" + fullPath + "\" created.\n";
}

QString txEmissionsBase::saveCalcOptions() const {

    QDir calcConfDir;

    const QString fullPath =
            calcConfDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + m_currTime + ".conf";

    QFile confFile(fullPath);

    if ( !confFile.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&confFile);

    fout << "task"           << "=" << m_calculationOptions->val_task()           << "\n"
         << "Vh"             << "=" << m_calculationOptions->val_Vh()             << "\n"
         << "standard"       << "=" << m_calculationOptions->val_standard()       << "\n"
         << "chargingType"   << "=" << m_calculationOptions->val_chargingType()   << "\n"
         << "fuelType"       << "=" << m_calculationOptions->val_fuelType()       << "\n"
         << "NOxSample"      << "=" << m_calculationOptions->val_NOxSample()      << "\n"
         << "PTcalc"         << "=" << m_calculationOptions->val_PTcalc()         << "\n"
         << "PTmass"         << "=" << m_calculationOptions->val_PTmass()         << "\n"
         << "addPointsCalc"  << "=" << m_calculationOptions->val_addPointsCalc()  << "\n"
         << "calcConfigFile" << "=" << m_calculationOptions->val_calcConfigFile() << "\n";

    confFile.close();

    return "libtoxic: Calculation config file \"" + fullPath + "\" created.\n";
}

} // namespace toxic
