/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txReducedPower.cpp

    Copyright (C) 2009-2013 Artem Petrov <pa2311@gmail.com>

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

#include "txReducedPower.h"
#include "txDataReader.h"
#include "txAuxiliaryFunctions.h"
#include "txError.h"
#include "txConstants.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace toxic {

txReducedPower::txReducedPower(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts) :
    m_commonParameters(commpars),
    m_calculationOptions(calcopts),
    m_numberOfPoints(0),
    m_n_rated(0),
    m_i_rated(0),
    m_N_fan_rated(0) {

    if ( !m_calculationOptions->val_calcConfigFile().isEmpty() ) {

        m_calculationOptions->
                readCalcConfigFile(m_calculationOptions->val_calcConfigFile());
    }
}

txReducedPower::~txReducedPower() {
}

void txReducedPower::setSourceData(
        const QVector<QVector<double> > &srcdata
        ) {

    m_numberOfPoints = srcdata.size();

    prepSrcArrays(srcdata);
}

void txReducedPower::setSourceData() {

    QSharedPointer<txDataReader> dataReader(new txDataReader());

    dataReader->readFile(m_commonParameters->val_srcFileNameRedPwr(), " ");
    QVector< QVector<double> > srcdata = dataReader->val_data();

    if ( srcdata.isEmpty() ) {
        throw txError("No data to calculate!");
    }

    //

    m_numberOfPoints = srcdata.size();

    prepSrcArrays(srcdata);
}

void txReducedPower::calculate() {

    m_currTime = QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss");

    //

    ma_Ne_brutto.clear();          ma_Ne_brutto.resize(m_numberOfPoints);
    ma_ge_brutto.clear();          ma_ge_brutto.resize(m_numberOfPoints);
    ma_qcs.clear();                ma_qcs.resize(m_numberOfPoints);
    ma_fm.clear();                 ma_fm.resize(m_numberOfPoints);
    ma_pa.clear();                 ma_pa.resize(m_numberOfPoints);
    ma_ps.clear();                 ma_ps.resize(m_numberOfPoints);
    ma_fa.clear();                 ma_fa.resize(m_numberOfPoints);
    ma_alphad.clear();             ma_alphad.resize(m_numberOfPoints);
    ma_Ne_reduced.clear();         ma_Ne_reduced.resize(m_numberOfPoints);
    ma_Ne_netto_reduced.clear();   ma_Ne_netto_reduced.resize(m_numberOfPoints);
    ma_Me_netto_reduced.clear();   ma_Me_netto_reduced.resize(m_numberOfPoints);
    ma_ge_netto_reduced.clear();   ma_ge_netto_reduced.resize(m_numberOfPoints);

    defRate();

    const double Vh = m_calculationOptions->val_Vh();
    const int chargtype = m_calculationOptions->val_chargingType();

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        ma_Ne_brutto[i] = ma_Me_brutto[i] * ma_n[i] / 9550.0;
        ma_ge_brutto[i] = ma_Gfuel[i] / ma_Ne_brutto[i] * 1000.0;

        if ( chargtype == CHARGINGTYPE_NO ) {
            ma_qcs[i] = 1;
        }
        else {
            ma_qcs[i] = ( (ma_Gfuel[i] * 1000000.0) /
                          (30.0 * ma_n[i] * Vh) ) /
                    ( (ma_pk[i] + ma_B0[i]) / (ma_S[i] + ma_B0[i]) );
        }

        if ( ma_qcs[i] < 40.0 ) {
            ma_fm[i] = 0.3;
        }
        else if ( ma_qcs[i] > 65.0 ) {
            ma_fm[i] = 1.2;
        }
        else {
            ma_fm[i] = 0.036 * ma_qcs[i] - 1.14;
        }

        ma_pa[i] = (0.506 + 0.1 * ma_t0[i] - 0.00245 *
                    pow(ma_t0[i], 2.0) + 0.0001 * pow(ma_t0[i], 3.0)) *
                (ma_Ra[i] / 100.0);
        ma_ps[i] = ma_B0[i] - ma_pa[i];

        if ( chargtype == CHARGINGTYPE_NO ) {
            ma_fa[i] = (99.0 / ma_ps[i]) *
                    pow( (ma_t0[i] + 273.0) / 298.0, 0.7 );
        }
        else {
            ma_fa[i] = pow( 99.0 / ma_ps[i], 0.7 ) *
                    pow( (ma_t0[i] + 273.0) / 298.0, 1.5 );
        }

        ma_alphad[i] = pow(ma_fa[i], ma_fm[i]);

        if ( ma_alphad[i] < 0.9 || ma_alphad[i] > 1.1 ) {
            throw txError("Parameter \"alphad\" "
                          "is out-of-range (0.9..1.1)!");
        }

        ma_Ne_reduced[i] = ma_alphad[i] * ma_Ne_brutto[i];
        ma_N_fan[i] = N_fan(m_N_fan_rated, ma_n[i], m_n_rated);
        ma_Ne_netto_reduced[i] = ma_Ne_reduced[i] - ma_N_fan[i] + ma_N_k[i];
        ma_Me_netto_reduced[i] = ma_Ne_netto_reduced[i] * 9550.0 / ma_n[i];
        ma_ge_netto_reduced[i] = ma_Gfuel[i] / ma_Ne_netto_reduced[i] * 1000.0;
    }
}

QString txReducedPower::createReports() {

    m_fullReportsPath =
            m_commonParameters->val_reportsDirName()
            + QDir::separator()
            + "R85_"
            + m_currTime;

    QDir reportDir;
    reportDir.mkdir(m_fullReportsPath);

    return saveSourceData() + saveCheckoutData();
}

void txReducedPower::prepSrcArrays(
        const QVector< QVector<double> > &srcdata
        ) {

    if ( srcdata.isEmpty() ) {
        throw txError("Incorrect source data array!");
    }

    ma_n.clear();         ma_n.resize(m_numberOfPoints);
    ma_Me_brutto.clear(); ma_Me_brutto.resize(m_numberOfPoints);
    ma_t0.clear();        ma_t0.resize(m_numberOfPoints);
    ma_B0.clear();        ma_B0.resize(m_numberOfPoints);
    ma_Ra.clear();        ma_Ra.resize(m_numberOfPoints);
    ma_S.clear();         ma_S.resize(m_numberOfPoints);
    ma_pk.clear();        ma_pk.resize(m_numberOfPoints);
    ma_Gfuel.clear();     ma_Gfuel.resize(m_numberOfPoints);
    ma_N_k.clear();       ma_N_k.resize(m_numberOfPoints);
    ma_N_fan.clear();     ma_N_fan.resize(m_numberOfPoints);
    ma_t_cool.clear();    ma_t_cool.resize(m_numberOfPoints);
    ma_t_oil.clear();     ma_t_oil.resize(m_numberOfPoints);
    ma_tk.clear();        ma_tk.resize(m_numberOfPoints);
    ma_tks.clear();       ma_tks.resize(m_numberOfPoints);
    ma_t_fuel.clear();    ma_t_fuel.resize(m_numberOfPoints);
    ma_pks.clear();       ma_pks.resize(m_numberOfPoints);
    ma_Gair.clear();      ma_Gair.resize(m_numberOfPoints);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( srcdata[i].size() != SRCDATACAPTIONS_REDPOWER.size() ) {
            throw txError("Incorrect source data array!");
        }

        ma_n        [i] = srcdata[i][ 1];
        ma_Me_brutto[i] = srcdata[i][ 2];
        ma_t0       [i] = srcdata[i][ 3];
        ma_B0       [i] = srcdata[i][ 4];
        ma_Ra       [i] = srcdata[i][ 5];
        ma_S        [i] = srcdata[i][ 6];
        ma_pk       [i] = srcdata[i][ 7];
        ma_Gfuel    [i] = srcdata[i][ 8];
        ma_N_k      [i] = srcdata[i][ 9];
        ma_N_fan    [i] = srcdata[i][10];
        ma_t_cool   [i] = srcdata[i][11];
        ma_t_oil    [i] = srcdata[i][12];
        ma_tk       [i] = srcdata[i][13];
        ma_tks      [i] = srcdata[i][14];
        ma_t_fuel   [i] = srcdata[i][15];
        ma_pks      [i] = srcdata[i][16];
        ma_Gair     [i] = srcdata[i][17];

    }

    if ( zeroArray(ma_n) ||
         zeroArray(ma_Me_brutto) ||
         zeroArray(ma_t0) ||
         zeroArray(ma_B0) ||
         zeroArray(ma_Ra) ||
         zeroArray(ma_pk) ||
         zeroArray(ma_Gfuel) ||
         (m_calculationOptions->val_Vh() < 0.0000001) ) {

        throw txError("Incorrect source data!\n"
                      "Please check Vh value and arrays of values "
                      "n, Me, t0, B0, Ra, pk and Gfuel.");
    }
}

void txReducedPower::defRate() {

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        if ( ma_n[i] > m_n_rated ) {
            m_n_rated = ma_n[i]; m_i_rated = i;
        }
    }

    m_N_fan_rated = ma_N_fan[m_i_rated];
}

QString txReducedPower::saveSourceData() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "SourceData_R85_"
            + m_currTime
            + ".dat";

    QFile srcData(fullPath);

    if ( !srcData.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&srcData);

    fout << right
         << qSetFieldWidth(COLUMNWIDTH);

    for ( ptrdiff_t i=0; i<SRCDATACAPTIONS_REDPOWER.size(); i++ ) {
        fout << SRCDATACAPTIONS_REDPOWER[i];
    }

    fout << qSetFieldWidth(0)
         << "\n"
         << fixed
         << qSetFieldWidth(COLUMNWIDTH)
         << qSetRealNumberPrecision(PRECISION+1);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH);
        fout << i + 1;
        fout << ma_n[i]     << ma_Me_brutto[i] << ma_t0[i]
             << ma_B0[i]    << ma_Ra[i]        << ma_S[i]
             << ma_pk[i]    << ma_Gfuel[i]     << ma_N_k[i]
             << ma_N_fan[i] << ma_t_cool[i]    << ma_t_oil[i]
             << ma_tk[i]    << ma_tks[i]       << ma_t_fuel[i]
             << ma_pks[i]   << ma_Gair[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    srcData.close();

    return "libtoxic: Source data file \"" + fullPath + "\" created.\n";
}

QString txReducedPower::saveCheckoutData() const {

    QDir reportDir;

    const QString fullPath =
            reportDir.relativeFilePath(m_fullReportsPath)
            + QDir::separator()
            + "CheckoutData_R85_"
            + m_currTime
            + ".dat";

    QFile choutData(fullPath);

    if ( !choutData.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + fullPath + "!");
    }

    QTextStream fout(&choutData);

    fout << right
         << qSetFieldWidth(COLUMNWIDTH);

    for ( ptrdiff_t i=0; i<SRCDATACAPTIONS_REDPOWER.size(); i++ ) {
        fout << SRCDATACAPTIONS_REDPOWER[i];
    }

    fout << "Ne_b[kW]"      << "ge_b[g/kWh]"  << "qcs[mg/cyc.l]"
         << "fm[-]"         << "pa[kPa]"      << "ps[kPa]"
         << "fa[-]"         << "alphad[-]"    << "Ne_r[kW]"
         << "Ne_nr[kW]"     << "Me_nr[Nm]"     << "ge_nr[g/kWh]";
    fout << qSetFieldWidth(0)
         << "\n"
         << fixed
         << qSetFieldWidth(COLUMNWIDTH)
         << qSetRealNumberPrecision(PRECISION+1);

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH)
             << qSetRealNumberPrecision(PRECISION);
        fout << (i + 1)
             << ma_n[i]         << ma_Me_brutto[i] << ma_t0[i]
             << ma_B0[i]        << ma_Ra[i]        << ma_S[i]
             << ma_pk[i]        << ma_Gfuel[i]     << ma_N_k[i]
             << ma_N_fan[i]     << ma_t_cool[i]    << ma_t_oil[i]
             << ma_tk[i]        << ma_tks[i]       << ma_t_fuel[i]
             << ma_pks[i]       << ma_Gair[i]      << ma_Ne_brutto[i]
             << ma_ge_brutto[i] << ma_qcs[i]       << ma_fm[i]
             << ma_pa[i]        << ma_ps[i];
        fout << qSetRealNumberPrecision(PRECISION+2);
        fout << ma_fa[i] << ma_alphad[i];
        fout << qSetRealNumberPrecision(PRECISION);
        fout << ma_Ne_reduced[i]       << ma_Ne_netto_reduced[i]
             << ma_Me_netto_reduced[i] << ma_ge_netto_reduced[i];
        fout << qSetFieldWidth(0)
             << "\n";
    }

    choutData.close();

    return "libtoxic: Checkout data file \"" + fullPath + "\" created.\n";
}

} // namespace toxic
