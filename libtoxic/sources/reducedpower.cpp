/*
    reducedpower.cpp

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

#include <QDebug>
#include <QString>
#include <QDir>

using std::string;
using std::endl;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

ReducedPower::ReducedPower(LibtoxicParameters *prms, CommonParameters *cfg) :
        GetDataFromCSV_OK (false),
        ReducePower_OK    (false),
        NumberOfPoints    (    0),
        mytime            (  "_"),
        fullReportsPath   (  "_"),
        n_rated           (    0),
        i_rated           (    0),
        N_fan_rated       (    0) {

    params = prms;
    config = cfg;

    if (params->val_CalcConfigFile() != "_._") {

        params->readCalcConfigFile(params->val_CalcConfigFile());
    }
}

ReducedPower::~ReducedPower() {

    if (GetDataFromCSV_OK) {

        delete Array_DataForCalc;
        delete Array_n;
        delete Array_Me_brutto;
        delete Array_t0;
        delete Array_B0;
        delete Array_Ra;
        delete Array_S;
        delete Array_pk;
        delete Array_Gfuel;
        delete Array_N_k;
        delete Array_N_fan;
    }

    if (ReducePower_OK) {

        delete Array_Ne_brutto;
        delete Array_qcs;
        delete Array_fm;
        delete Array_pa;
        delete Array_ps;
        delete Array_fa;
        delete Array_alphad;
        delete Array_Ne_reduced;
        delete Array_Ne_brake_reduced;
        delete Array_Ne_netto_reduced;
        delete Array_Me_netto_reduced;
        delete Array_ge_netto_reduced;
    }
}

ReducedPower::ReducedPower(const ReducedPower &orig) {

    (void)(&orig);
}

ReducedPower &ReducedPower::operator =(const ReducedPower &x) {

    (void)(&x);
    return *this;
}

bool ReducedPower::readCSV(double **data, ptrdiff_t n, ptrdiff_t m) {

    GetDataFromCSV_OK = false;

    if ( (n == 0) && (m == 0) ) {

        QString filenamePowers = config->val_filenamePowers();
        QString csvdelimiter = config->val_csvDelimiter();

        csvRead *ReaderDataForCalc = new csvRead();

        if (!ReaderDataForCalc->readData(filenamePowers, csvdelimiter, &NumberOfPoints)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: ReducedPower: readCSV: readData function returns 1!";

            return false;
        }

        Array_DataForCalc = new Double2DArray(NumberOfPoints, PowersFileColumnsNumber);
        array_DataForCalc = Array_DataForCalc->arrayPointer();

        if (!ReaderDataForCalc->checkArrayDimension(PowersFileColumnsNumber)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: ReducedPower: readCSV: checkArrayDimension function returns 1!";

            return false;
        }

        if (!ReaderDataForCalc->fillArray(array_DataForCalc)) {

            delete ReaderDataForCalc;

            qDebug() << "libtoxic ERROR: ReducedPower: readCSV: fillArray function returns 1!";

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

        qDebug() << "libtoxic ERROR: ReducedPower: readCSV: Illegal data!";

        return false;
    }

    Array_n = new Double2DArray(NumberOfPoints, 1);
    array_n = Array_n->arrayPointer();

    Array_Me_brutto = new Double2DArray(NumberOfPoints, 1);
    array_Me_brutto = Array_Me_brutto->arrayPointer();

    Array_t0 = new Double2DArray(NumberOfPoints, 1);
    array_t0 = Array_t0->arrayPointer();

    Array_B0 = new Double2DArray(NumberOfPoints, 1);
    array_B0 = Array_B0->arrayPointer();

    Array_Ra = new Double2DArray(NumberOfPoints, 1);
    array_Ra = Array_Ra->arrayPointer();

    Array_S = new Double2DArray(NumberOfPoints, 1);
    array_S = Array_S->arrayPointer();

    Array_pk = new Double2DArray(NumberOfPoints, 1);
    array_pk = Array_pk->arrayPointer();

    Array_Gfuel = new Double2DArray(NumberOfPoints, 1);
    array_Gfuel = Array_Gfuel->arrayPointer();

    Array_N_k = new Double2DArray(NumberOfPoints, 1);
    array_N_k = Array_N_k->arrayPointer();

    Array_N_fan = new Double2DArray(NumberOfPoints, 1);
    array_N_fan = Array_N_fan->arrayPointer();

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        array_n        [i][0] = array_DataForCalc[i+1][ 1];
        array_Me_brutto[i][0] = array_DataForCalc[i+1][ 2];
        array_t0       [i][0] = array_DataForCalc[i+1][ 3];
        array_B0       [i][0] = array_DataForCalc[i+1][ 4];
        array_Ra       [i][0] = array_DataForCalc[i+1][ 5];
        array_S        [i][0] = array_DataForCalc[i+1][ 6];
        array_pk       [i][0] = array_DataForCalc[i+1][ 7];
        array_Gfuel    [i][0] = array_DataForCalc[i+1][ 8];
        array_N_k      [i][0] = array_DataForCalc[i+1][ 9];
        array_N_fan    [i][0] = array_DataForCalc[i+1][10];
    }

    mytime = dateTimeNow();

    GetDataFromCSV_OK = true;

    //

    if ( !nonZeroArray(array_n, &NumberOfPoints) ||
         !nonZeroArray(array_Me_brutto, &NumberOfPoints) ||
         !nonZeroArray(array_t0, &NumberOfPoints) ||
         !nonZeroArray(array_B0, &NumberOfPoints) ||
         !nonZeroArray(array_Ra, &NumberOfPoints) ||
         !nonZeroArray(array_pk, &NumberOfPoints) ||
         !nonZeroArray(array_Gfuel, &NumberOfPoints) ||
         (params->val_Vh() < 0.0000001) ) {

        qDebug() << "libtoxic ERROR: ReducedPower: readCSV: Bad source data or calculation settings!";
        return false;
    }

    //

    return true;
}

void ReducedPower::SetRate() {

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        if (array_n[i][0] > n_rated) { n_rated = array_n[i][0]; i_rated = i; }
    }

    N_fan_rated = array_N_fan[i_rated][0];
}

bool ReducedPower::reducePower() {

    ReducePower_OK = false;

    Array_Ne_brutto = new Double2DArray(NumberOfPoints, 1);
    array_Ne_brutto = Array_Ne_brutto->arrayPointer();

    Array_qcs = new Double2DArray(NumberOfPoints, 1);
    array_qcs = Array_qcs->arrayPointer();

    Array_fm = new Double2DArray(NumberOfPoints, 1);
    array_fm = Array_fm->arrayPointer();

    Array_pa = new Double2DArray(NumberOfPoints, 1);
    array_pa = Array_pa->arrayPointer();

    Array_ps = new Double2DArray(NumberOfPoints, 1);
    array_ps = Array_ps->arrayPointer();

    Array_fa = new Double2DArray(NumberOfPoints, 1);
    array_fa = Array_fa->arrayPointer();

    Array_alphad = new Double2DArray(NumberOfPoints, 1);
    array_alphad = Array_alphad->arrayPointer();

    Array_Ne_reduced = new Double2DArray(NumberOfPoints, 1);
    array_Ne_reduced = Array_Ne_reduced->arrayPointer();

    Array_Ne_brake_reduced = new Double2DArray(NumberOfPoints, 1);
    array_Ne_brake_reduced = Array_Ne_brake_reduced->arrayPointer();

    Array_Ne_netto_reduced = new Double2DArray(NumberOfPoints, 1);
    array_Ne_netto_reduced = Array_Ne_netto_reduced->arrayPointer();

    Array_Me_netto_reduced = new Double2DArray(NumberOfPoints, 1);
    array_Me_netto_reduced = Array_Me_netto_reduced->arrayPointer();

    Array_ge_netto_reduced = new Double2DArray(NumberOfPoints, 1);
    array_ge_netto_reduced = Array_ge_netto_reduced->arrayPointer();

    SetRate();

    for (ptrdiff_t i=0; i<NumberOfPoints; i++) {

        array_Ne_brutto[i][0] = array_Me_brutto[i][0] * array_n[i][0] / 9550.0;
        array_qcs[i][0] = ( (array_Gfuel[i][0] * 1000000.0)   / (30.0 * array_n[i][0] * params->val_Vh()) ) /
                          ( (array_pk[i][0] + array_B0[i][0]) / (array_S[i][0] + array_B0[i][0])       );

        if      (array_qcs[i][0] < 40.0) { array_fm[i][0] = 0.3;                            }
        else if (array_qcs[i][0] > 65.0) { array_fm[i][0] = 1.2;                            }
        else                             { array_fm[i][0] = 0.036 * array_qcs[i][0] - 1.14; }

        array_pa[i][0] = (0.506 + 0.1 * array_t0[i][0] - 0.00245 * pow(array_t0[i][0], 2) + 0.0001 * pow(array_t0[i][0], 3)) * (array_Ra[i][0] / 100.0);
        array_ps[i][0] = array_B0[i][0] - array_pa[i][0];
        array_fa[i][0] = pow(99.0/array_ps[i][0], 0.7) * pow((array_t0[i][0]+273.0)/298.0, 1.5);
        array_alphad[i][0] = pow(array_fa[i][0], array_fm[i][0]);

        if ( (array_alphad[i][0] < 0.9) || (array_alphad[i][0] > 1.1) ) {

            qDebug() << "r49 ERROR: ReducedPower: reducePower: alphad is out-of-range (0.9..1.1)!";

            return false;
        }

        array_Ne_reduced[i][0] = array_alphad[i][0] * array_Ne_brutto[i][0];
        array_Ne_brake_reduced[i][0] = array_Ne_brutto[i][0] + array_N_k[i][0];
        array_N_fan[i][0] = N_fan_rated * pow(array_n[i][0] / n_rated, 3);
        array_Ne_netto_reduced[i][0] = array_Ne_reduced[i][0] - array_N_fan[i][0];
        array_Me_netto_reduced[i][0] = array_Ne_netto_reduced[i][0] * 9550.0 / array_n[i][0];
        array_ge_netto_reduced[i][0] = array_Gfuel[i][0] / array_Ne_netto_reduced[i][0] * 1000.0;
    }

    ReducePower_OK = true;

    return true;
}

QString ReducedPower::createReports() {

    QString message = "";

    string csvdelimiter = (config->val_csvDelimiter()).toStdString();
    QString dirnameReports = config->val_dirnameReports();

    fullReportsPath = dirnameReports + "r85_" + QString::fromStdString(mytime);
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
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_n[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Me_brutto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_t0[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_B0[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ra[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_S[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_pk[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Gfuel[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_N_k[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_N_fan[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_brutto[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_qcs[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_fm[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_pa[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_ps[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+2) << array_fa[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision+2) << array_alphad[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_reduced[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_brake_reduced[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Ne_netto_reduced[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_Me_netto_reduced[i][0] << csvdelimiter;
        fout1 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision) << array_ge_netto_reduced[i][0] << csvdelimiter << endl;
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

            fout4 << fixed << right << setw(WidthOfColumn) << setfill(' ') << setprecision(Precision + 1) << array_DataForCalc[i+1][j] << csvdelimiter;
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
