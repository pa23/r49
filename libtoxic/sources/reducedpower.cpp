/*
    reducedpower.cpp

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
#include "stringoperations.h"
#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"
#include "datetime.h"
#include "precalc.h"

#include <string>
#include <cmath>
#include <fstream>
#include <iomanip>

#include <QSharedPointer>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QDir>

using std::string;
using std::endl;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

ReducedPower::ReducedPower(QSharedPointer<LibtoxicParameters> prms, QSharedPointer<CommonParameters> cfg) :
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

bool ReducedPower::readCSV(QVector< QVector<double> > data) {

    if ( data.isEmpty() ) {

        QString filenamePowers = config.data()->val_filenamePowers();
        QString csvdelimiter = config.data()->val_csvDelimiter();

        QSharedPointer<csvRead> readerDataForCalc(new csvRead());

        array_DataForCalc = readerDataForCalc.data()->csvData(filenamePowers, csvdelimiter);

        if (array_DataForCalc.at(0).size() != PowersFileColumnsNumber) {

            qDebug() << "libtoxic ERROR: ReducedPower: readCSV: incorrect source data!";

            return false;
        }
    }
    else {

        array_DataForCalc = data;
    }

    NumberOfPoints = array_DataForCalc.size();

    array_n.resize(NumberOfPoints);
    array_Me_brutto.resize(NumberOfPoints);
    array_t0.resize(NumberOfPoints);
    array_B0.resize(NumberOfPoints);
    array_Ra.resize(NumberOfPoints);
    array_S.resize(NumberOfPoints);
    array_pk.resize(NumberOfPoints);
    array_Gfuel.resize(NumberOfPoints);
    array_N_k.resize(NumberOfPoints);
    array_N_fan.resize(NumberOfPoints);

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

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

    mytime = dateTimeNow();

    //

    if ( !nonZeroArray(array_n) ||
         !nonZeroArray(array_Me_brutto) ||
         !nonZeroArray(array_t0) ||
         !nonZeroArray(array_B0) ||
         !nonZeroArray(array_Ra) ||
         !nonZeroArray(array_pk) ||
         !nonZeroArray(array_Gfuel) ||
         (params.data()->val_Vh() < 0.0000001) ) {

        qDebug() << "libtoxic ERROR: ReducedPower: readCSV: Bad source data or calculation settings!";
        return false;
    }

    //

    return true;
}

void ReducedPower::setRate() {

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        if (array_n[i] > n_rated) { n_rated = array_n[i]; i_rated = i; }
    }

    N_fan_rated = array_N_fan[i_rated];
}

bool ReducedPower::reducePower() {

    array_Ne_brutto.resize(NumberOfPoints);
    array_qcs.resize(NumberOfPoints);
    array_fm.resize(NumberOfPoints);
    array_pa.resize(NumberOfPoints);
    array_ps.resize(NumberOfPoints);
    array_fa.resize(NumberOfPoints);
    array_alphad.resize(NumberOfPoints);
    array_Ne_reduced.resize(NumberOfPoints);
    array_Ne_brake_reduced.resize(NumberOfPoints);
    array_Ne_netto_reduced.resize(NumberOfPoints);
    array_Me_netto_reduced.resize(NumberOfPoints);
    array_ge_netto_reduced.resize(NumberOfPoints);

    setRate();

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        array_Ne_brutto[i] = array_Me_brutto[i] * array_n[i] / 9550.0;
        array_qcs[i] = ( (array_Gfuel[i] * 1000000.0) / (30.0 * array_n[i] * params.data()->val_Vh()) ) /
                       ( (array_pk[i] + array_B0[i]) / (array_S[i] + array_B0[i]) );

        if      (array_qcs[i] < 40.0) { array_fm[i] = 0.3;                         }
        else if (array_qcs[i] > 65.0) { array_fm[i] = 1.2;                         }
        else                          { array_fm[i] = 0.036 * array_qcs[i] - 1.14; }

        array_pa[i] = (0.506 + 0.1 * array_t0[i] - 0.00245 * pow(array_t0[i], 2) + 0.0001 * pow(array_t0[i], 3)) * (array_Ra[i] / 100.0);
        array_ps[i] = array_B0[i] - array_pa[i];
        array_fa[i] = pow(99.0/array_ps[i], 0.7) * pow((array_t0[i]+273.0)/298.0, 1.5);
        array_alphad[i] = pow(array_fa[i], array_fm[i]);

        if ( (array_alphad[i] < 0.9) || (array_alphad[i] > 1.1) ) {

            qDebug() << "r49 ERROR: ReducedPower: reducePower: alphad is out-of-range (0.9..1.1)!";

            return false;
        }

        array_Ne_reduced[i] = array_alphad[i] * array_Ne_brutto[i];
        array_Ne_brake_reduced[i] = array_Ne_brutto[i] + array_N_k[i];
        array_N_fan[i] = N_fan_rated * pow(array_n[i] / n_rated, 3);
        array_Ne_netto_reduced[i] = array_Ne_reduced[i] - array_N_fan[i];
        array_Me_netto_reduced[i] = array_Ne_netto_reduced[i] * 9550.0 / array_n[i];
        array_ge_netto_reduced[i] = array_Gfuel[i] / array_Ne_netto_reduced[i] * 1000.0;
    }

    return true;
}

QString ReducedPower::createReports() {

    QString message = "";

    string csvdelimiter = (config.data()->val_csvDelimiter()).toStdString();
    QString dirnameReports = config.data()->val_dirnameReports();

    fullReportsPath = dirnameReports + "/" + "r85_" + QString::fromStdString(mytime);
    QDir reportdir;
    reportdir.mkdir(fullReportsPath);

    //

    string CheckoutDataFileName;
    CheckoutDataFileName = "CheckoutData_" + mytime + ".csv";

    QString checkoutdata = reportdir.relativeFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(CheckoutDataFileName);
    ofstream fout1(checkoutdata.toAscii());

    if (!fout1) {

        message += "libtoxic ERROR: ReducedPower: createReports: fout1 was not created!\n";
        qDebug() << "libtoxic ERROR: ReducedPower: createReports: fout1 was not created!";

        return message;
    }

    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Point[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "n[min-1]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Me_b[Nm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "t0[oC]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "B0[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ra[%]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "S[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "pk[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Gfuel[kg/h]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "N_k[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "N_fan[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_b[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "qcs[mg/cyc.l]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "fm[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "pa[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "ps[kPa]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "fa[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "alphad[-]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_r[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "N_br[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Ne_nr[kW]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "Me_nr[Nm]" << csvdelimiter;
    fout1 << right << setw(WidthOfColumn) << setfill(' ') << "ge_nr[g/kWh]" << csvdelimiter << endl;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << (i + 1) << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_n[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Me_brutto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_t0[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_B0[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ra[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_S[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_pk[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Gfuel[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_N_k[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_N_fan[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_brutto[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_qcs[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_fm[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_pa[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_ps[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+2) << array_fa[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+2) << array_alphad[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_reduced[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_brake_reduced[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_netto_reduced[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Me_netto_reduced[i] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_ge_netto_reduced[i] << csvdelimiter << endl;
    }

    fout1.close();

    message += "libtoxic: Additional file \"" + QString::fromStdString(CheckoutDataFileName) + "\" rewrited.\n";
    qDebug() << "\nlibtoxic: Additional file" << QString::fromStdString(CheckoutDataFileName) << "rewrited.";

    string SourceDataFileName;

    SourceDataFileName = "SourceData85_" + mytime + ".csv";

    QString srcdata = reportdir.absoluteFilePath(fullReportsPath) + reportdir.separator() + QString::fromStdString(SourceDataFileName);
    ofstream fout4(srcdata.toAscii());

    if (!fout4) {

        message += "libtoxic ERROR: ReducedPower: createReports: fout4 was not created!\n";
        qDebug() << "libtoxic ERROR: ReducedPower: createReports: fout4 was not created!";

        return message;
    }

    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Point[-]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "n[min-1]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Me_b[Nm]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "t0[oC]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "B0[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Ra[%]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "S[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "pk[kPa]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "Gfuel[kg/h]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "N_k[kW]" << csvdelimiter;
    fout4 << right << setw(WidthOfColumn) << setfill(' ') << "N_fan[kW]" << csvdelimiter << endl;

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        for (ptrdiff_t j=0; j<PowersFileColumnsNumber; j++) {

            fout4 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision + 1) << array_DataForCalc[i][j] << csvdelimiter;
        }

        fout4 << endl;
    }

    fout4.close();

    message += "libtoxic: SourceData file \"" + QString::fromStdString(SourceDataFileName) + "\" created.\n";
    qDebug() << "libtoxic: SourceData file" << QString::fromStdString(SourceDataFileName) << "created.";

    return message;
}

QString ReducedPower::lastReportsDir() {

    return fullReportsPath;
}
