/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: reducedpower.cpp

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

#include "reducedpower.h"
#include "csvread.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"
#include "precalc.h"

#include <QSharedPointer>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QFile>

#include <cmath>

ReducedPower::ReducedPower(const QSharedPointer<LibtoxicParameters> &prms,
                           const QSharedPointer<CommonParameters> &cfg) :
        NumberOfPoints    (    0),
        mytime            (  "_"),
        fullReportsPath   (  "_"),
        n_rated           (    0),
        i_rated           (    0),
        N_fan_rated       (    0) {

    params = prms;
    config = cfg;

    if (params.data()->val_CalcConfigFile() != "_._") {

        params.data()->readCalcConfigFile(params.data()->val_CalcConfigFile());
    }
}

ReducedPower::~ReducedPower() {
}

ReducedPower::ReducedPower(const ReducedPower &orig) {

    (void)(&orig);
}

ReducedPower &ReducedPower::operator =(const ReducedPower &x) {

    (void)(&x);
    return *this;
}

bool ReducedPower::readCSV(const QVector< QVector<double> > &data) {

    if ( data.isEmpty() ) {

        QString filenamePowers = config.data()->val_filenamePowers();

        QSharedPointer<csvRead>
                readerDataForCalc(new csvRead(filenamePowers,
                                              " ",
                                              STRSNUMBERFORCOLUMNCAPTION));

        array_DataForCalc = readerDataForCalc.data()->csvData();

        if ( array_DataForCalc.isEmpty() ) {

            qDebug() << Q_FUNC_INFO << ":::" << "Incorrect source data!";

            return false;
        }

        if ( array_DataForCalc.at(0).size() != POWERSFILECOLUMNSNUMBER ) {

            qDebug() << Q_FUNC_INFO << ":::" << "Incorrect source data!";

            return false;
        }
    }
    else {

        array_DataForCalc = data;
    }

    NumberOfPoints = array_DataForCalc.size();

    array_n.clear();         array_n.resize(NumberOfPoints);
    array_Me_brutto.clear(); array_Me_brutto.resize(NumberOfPoints);
    array_t0.clear();        array_t0.resize(NumberOfPoints);
    array_B0.clear();        array_B0.resize(NumberOfPoints);
    array_Ra.clear();        array_Ra.resize(NumberOfPoints);
    array_S.clear();         array_S.resize(NumberOfPoints);
    array_pk.clear();        array_pk.resize(NumberOfPoints);
    array_Gfuel.clear();     array_Gfuel.resize(NumberOfPoints);
    array_N_k.clear();       array_N_k.resize(NumberOfPoints);
    array_N_fan.clear();     array_N_fan.resize(NumberOfPoints);

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        array_n        [i] = array_DataForCalc[i][ 1];
        array_Me_brutto[i] = array_DataForCalc[i][ 2];
        array_t0       [i] = array_DataForCalc[i][ 3];
        array_B0       [i] = array_DataForCalc[i][ 4];
        array_Ra       [i] = array_DataForCalc[i][ 5];
        array_S        [i] = array_DataForCalc[i][ 6];
        array_pk       [i] = array_DataForCalc[i][ 7];
        array_Gfuel    [i] = array_DataForCalc[i][ 8];
        array_N_k      [i] = array_DataForCalc[i][ 9];
        array_N_fan    [i] = array_DataForCalc[i][10];
    }

    mytime = QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss");

    //

    if ( !nonZeroArray(array_n) ||
         !nonZeroArray(array_Me_brutto) ||
         !nonZeroArray(array_t0) ||
         !nonZeroArray(array_B0) ||
         !nonZeroArray(array_Ra) ||
         !nonZeroArray(array_pk) ||
         !nonZeroArray(array_Gfuel) ||
         (params.data()->val_Vh() < 0.0000001) ) {

        qDebug() << Q_FUNC_INFO << ":::"
                 << "Bad source data or calculation settings!";
        return false;
    }

    //

    return true;
}

void ReducedPower::setRate() {

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        if ( array_n[i] > n_rated ) { n_rated = array_n[i]; i_rated = i; }
    }

    N_fan_rated = array_N_fan[i_rated];
}

bool ReducedPower::reducePower() {

    array_Ne_brutto.clear();        array_Ne_brutto.resize(NumberOfPoints);
    array_qcs.clear();              array_qcs.resize(NumberOfPoints);
    array_fm.clear();               array_fm.resize(NumberOfPoints);
    array_pa.clear();               array_pa.resize(NumberOfPoints);
    array_ps.clear();               array_ps.resize(NumberOfPoints);
    array_fa.clear();               array_fa.resize(NumberOfPoints);
    array_alphad.clear();           array_alphad.resize(NumberOfPoints);
    array_Ne_reduced.clear();       array_Ne_reduced.resize(NumberOfPoints);
    array_Ne_brake_reduced.clear(); array_Ne_brake_reduced.resize(NumberOfPoints);
    array_Ne_netto_reduced.clear(); array_Ne_netto_reduced.resize(NumberOfPoints);
    array_Me_netto_reduced.clear(); array_Me_netto_reduced.resize(NumberOfPoints);
    array_ge_netto_reduced.clear(); array_ge_netto_reduced.resize(NumberOfPoints);

    setRate();

    double Vh = params.data()->val_Vh();

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        array_Ne_brutto[i] = array_Me_brutto[i] * array_n[i] / 9550.0;

        array_qcs[i] = ( (array_Gfuel[i] * 1000000.0) /
                         (30.0 * array_n[i] * Vh) ) /
                ( (array_pk[i] + array_B0[i]) / (array_S[i] + array_B0[i]) );

        if ( array_qcs[i] < 40.0 ) {

            array_fm[i] = 0.3;
        }
        else if ( array_qcs[i] > 65.0 ) {

            array_fm[i] = 1.2;
        }
        else {

            array_fm[i] = 0.036 * array_qcs[i] - 1.14;
        }

        array_pa[i] = (0.506 + 0.1 * array_t0[i] - 0.00245 *
                       pow(array_t0[i], 2) + 0.0001 * pow(array_t0[i], 3)) *
                (array_Ra[i] / 100.0);

        array_ps[i] = array_B0[i] - array_pa[i];

        if ( params.data()->val_ChargingType() == CHARGINGTYPE_NO ) {

            array_fa[i] = (99.0 / array_ps[i]) *
                    pow( (array_t0[i] + 273.0) / 298.0, 0.7 );
        }
        else {

            array_fa[i] = pow( 99.0 / array_ps[i], 0.7 ) *
                    pow( (array_t0[i] + 273.0) / 298.0, 1.5 );
        }

        array_alphad[i] = pow(array_fa[i], array_fm[i]);

        if ( (array_alphad[i] < 0.9) || (array_alphad[i] > 1.1) ) {

            qDebug() << Q_FUNC_INFO << ":::"
                     << "alphad is out-of-range (0.9..1.1)!";

            return false;
        }

        array_Ne_reduced[i] = array_alphad[i] * array_Ne_brutto[i];
        array_Ne_brake_reduced[i] = array_Ne_brutto[i] + array_N_k[i];
        array_N_fan[i] = calcNfan(N_fan_rated, array_n[i], n_rated);
        array_Ne_netto_reduced[i] = array_Ne_reduced[i] - array_N_fan[i];

        array_Me_netto_reduced[i] = array_Ne_netto_reduced[i] * 9550.0 /
                array_n[i];

        array_ge_netto_reduced[i] = array_Gfuel[i] /
                array_Ne_netto_reduced[i] * 1000.0;
    }

    return true;
}

QString ReducedPower::createReports() {

    QString message = "";

    QString dirnameReports = config.data()->val_dirnameReports();

    fullReportsPath = dirnameReports + "/" + "R85_" + mytime;
    QDir reportdir;
    reportdir.mkdir(fullReportsPath);

    //

    QString checkoutDataFileName = "CheckoutData_" + mytime + ".csv";

    QString checkoutdata = reportdir.relativeFilePath(fullReportsPath) +
            reportdir.separator() + checkoutDataFileName;

    QFile data1(checkoutdata);

    if ( !data1.open(QFile::WriteOnly) ) {

        message += QString::fromAscii(Q_FUNC_INFO) + ":::" +
                "Can not open data1 to write!\n";
        qDebug() << Q_FUNC_INFO << ":::"
                 << "Can not open data1 to write!";

        return message;
    }

    QTextStream fout1(&data1);

    fout1 << fixed << right << qSetFieldWidth(WIDTHOFCOLUMN)
          << "Point[-]"      << "n[min-1]"     << "Me_b[Nm]"
          << "t0[oC]"        << "B0[kPa]"      << "Ra[%]"
          << "S[kPa]"        << "pk[kPa]"      << "Gfuel[kg/h]"
          << "N_k[kW]"       << "N_fan[kW]"    << "Ne_b[kW]"
          << "qcs[mg/cyc.l]" << "fm[-]"        << "pa[kPa]"
          << "ps[kPa]"       << "fa[-]"        << "alphad[-]"
          << "Ne_r[kW]"      << "N_br[kW]"     << "Ne_nr[kW]"
          << "Me_nr[Nm]"     << "ge_nr[g/kWh]"
          << qSetFieldWidth(0) << "\n";

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        fout1 << qSetFieldWidth(WIDTHOFCOLUMN);

        fout1 << qSetRealNumberPrecision(PRECISION)
              << (i + 1)      << array_n[i]     << array_Me_brutto[i]
              << array_t0[i]  << array_B0[i]    << array_Ra[i]
              << array_S[i]   << array_pk[i]    << array_Gfuel[i]
              << array_N_k[i] << array_N_fan[i] << array_Ne_brutto[i]
              << array_qcs[i] << array_fm[i]    << array_pa[i]
              << array_ps[i];

        fout1 << qSetRealNumberPrecision(PRECISION+2)
              << array_fa[i] << array_alphad[i];

        fout1 << qSetRealNumberPrecision(PRECISION)
              << array_Ne_reduced[i]       << array_Ne_brake_reduced[i]
              << array_Ne_netto_reduced[i] << array_Me_netto_reduced[i]
              << array_ge_netto_reduced[i];

        fout1 << qSetFieldWidth(0) << "\n";
    }

    data1.close();

    message += "libtoxic: Additional file \"" +
            checkoutDataFileName + "\" rewrited.\n";
    qDebug() << "\nlibtoxic: Additional file"
             << checkoutDataFileName << "rewrited.";

    //

    QString sourceDataFileName = "SourceData85_" + mytime + ".csv";

    QString srcdata = reportdir.absoluteFilePath(fullReportsPath) +
            reportdir.separator() + sourceDataFileName;

    QFile data4(srcdata);

    if ( !data4.open(QFile::WriteOnly) ) {

        message += QString::fromAscii(Q_FUNC_INFO) + ":::" +
                "Can not open data4 to write!\n";
        qDebug() << Q_FUNC_INFO << ":::"
                 << "Can not open data4 to write!";

        return message;
    }

    QTextStream fout4(&data4);

    fout4 << fixed << right << qSetFieldWidth(WIDTHOFCOLUMN)
          << qSetRealNumberPrecision(PRECISION+1)
          << "Point[-]" << "n[min-1]"  << "Me_b[Nm]"
          << "t0[oC]"   << "B0[kPa]"   << "Ra[%]"
          << "S[kPa]"   << "pk[kPa]"   << "Gfuel[kg/h]"
          << "N_k[kW]"  << "N_fan[kW]"
          << qSetFieldWidth(0) << "\n";

    for ( ptrdiff_t i=0; i<NumberOfPoints; i++ ) {

        fout4 << qSetFieldWidth(WIDTHOFCOLUMN);

        for ( ptrdiff_t j=0; j<POWERSFILECOLUMNSNUMBER; j++ ) {

            fout4 << array_DataForCalc[i][j];
        }

        fout4 << qSetFieldWidth(0) << "\n";
    }

    data4.close();

    message += "libtoxic: SourceData file \"" +
            sourceDataFileName + "\" created.\n";
    qDebug() << "libtoxic: SourceData file"
             << sourceDataFileName << "created.";

    return message;
}

QString ReducedPower::lastReportsDir() const {

    return fullReportsPath;
}
