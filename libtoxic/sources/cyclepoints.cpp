/*
    cyclepoints.cpp

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

#include "cyclepoints.h"
#include "libtoxicconstants.h"
#include "precalc.h"
#include "csvread.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <fstream>
#include <iomanip>
#include <cmath>

#include <QDebug>
#include <QString>
#include <QVector>

using std::string;
using std::endl;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

CyclePoints::CyclePoints(QSharedPointer<LibtoxicParameters> prms, QSharedPointer<CommonParameters> cfg) :
        n_hi               (0),
        n_lo               (0),
        A                  (0),
        B                  (0),
        C                  (0),
        a1                 (0),
        a2                 (0),
        a3                 (0),
        n_ref              (0),
        idle               (0),
        n_rated            (0),
        N_fan_rated        (0),
        Ne_A               (0),
        Ne_B               (0),
        Ne_C               (0),
        Ne_a1              (0),
        Ne_a2              (0),
        Ne_a3              (0),
        n_interim          (0),
        Ne_interim         (0),
        Ne_rated           (0) {

    params = prms; // calculatin settings
    config = cfg;  // r49.cong file

    if (params.data()->val_CalcConfigFile() != "_._") {

        params.data()->readCalcConfigFile(params.data()->val_CalcConfigFile());
    }
}

CyclePoints::~CyclePoints() {
}

CyclePoints::CyclePoints(const CyclePoints &orig) {

    (void)(&orig);
}

CyclePoints &CyclePoints::operator =(const CyclePoints &x) {

    (void)(&x);
    return *this;
}

bool CyclePoints::readCSV(QVector< QVector<double> > data) {

    QString std = params.data()->val_Standard();
    QString csvdelimiter = config.data()->val_csvDelimiter();
    QString filenameSource = "";

    QVector< QVector<double> > arraySourceData;

    if ( data.isEmpty() ) {

        QSharedPointer<csvRead> readerSourceData(new csvRead());

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            filenameSource = config.data()->val_filenameSourceEU3();
        }
        else {

            filenameSource = config.data()->val_filenameSourceEU0();
        }

        arraySourceData = readerSourceData.data()->csvData(filenameSource, csvdelimiter);

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            if (arraySourceData.at(0).size() != EU3SrcDataParamsNumber) {

                qDebug() << "libtoxic ERROR: CyclePoints: readCSV: incorrect source data!";

                return false;
            }

            n_hi        = arraySourceData[0][ 0];
            n_lo        = arraySourceData[0][ 1];
            idle        = arraySourceData[0][ 2];
            n_rated     = arraySourceData[0][ 3];
            N_fan_rated = arraySourceData[0][ 4];
            Ne_A        = arraySourceData[0][ 5];
            Ne_B        = arraySourceData[0][ 6];
            Ne_C        = arraySourceData[0][ 7];
            Ne_a1       = arraySourceData[0][ 8];
            Ne_a2       = arraySourceData[0][ 9];
            Ne_a3       = arraySourceData[0][10];

            if (!calcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

                qDebug() << "libtoxic ERROR: CyclePoints: readCSV: calcABC function returns false!";

                return false;
            }
        }
        else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") || (std == "OST") || (std == "GOST") ||
                  (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
                  (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
                  (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
                  (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ||
                  (std == "C1") || (std == "D1") || (std == "D2") ||
                  (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                  (std == "F") || (std == "G1") || (std == "G2") ) {

            if (arraySourceData.at(0).size() != EU0SrcDataParamsNumber) {

                qDebug() << "libtoxic ERROR: CyclePoints: readCSV: incorrect source data!";

                return false;
            }

            idle        = arraySourceData[0][ 0];
            n_interim   = arraySourceData[0][ 1];
            n_rated     = arraySourceData[0][ 2];
            N_fan_rated = arraySourceData[0][ 3];
            Ne_interim  = arraySourceData[0][ 4];
            Ne_rated    = arraySourceData[0][ 5];
        }
        else {

            qDebug() << "libtoxic ERROR: CyclePoints: readCSV: incorrect program configuration!";

            return false;
        }
    }
    else {

        arraySourceData = data;

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            n_hi        = arraySourceData[0][ 0];
            n_lo        = arraySourceData[0][ 1];
            idle        = arraySourceData[0][ 2];
            n_rated     = arraySourceData[0][ 3];
            N_fan_rated = arraySourceData[0][ 4];
            Ne_A        = arraySourceData[0][ 5];
            Ne_B        = arraySourceData[0][ 6];
            Ne_C        = arraySourceData[0][ 7];
            Ne_a1       = arraySourceData[0][ 8];
            Ne_a2       = arraySourceData[0][ 9];
            Ne_a3       = arraySourceData[0][10];

            if (!calcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

                qDebug() << "libtoxic ERROR: CyclePoints: readCSV: calcABC function returns false!";

                return false;
            }
        }
        else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") || (std == "OST") || (std == "GOST") ||
                  (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
                  (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
                  (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ||
                  (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ||
                  (std == "C1") || (std == "D1") || (std == "D2") ||
                  (std == "E1") || (std == "E2") || (std == "E3") || (std == "E5") ||
                  (std == "F") || (std == "G1") || (std == "G2") ) {

            idle        = arraySourceData[0][ 0];
            n_interim   = arraySourceData[0][ 1];
            n_rated     = arraySourceData[0][ 2];
            N_fan_rated = arraySourceData[0][ 3];
            Ne_interim  = arraySourceData[0][ 4];
            Ne_rated    = arraySourceData[0][ 5];
        }
        else {

            qDebug() << "libtoxic ERROR: CyclePoints: readCSV: incorrect program configuration!";

            return false;
        }
    }

    return true;
}

bool CyclePoints::fillArrays() {

    QString std = params.data()->val_Standard();

    ptrdiff_t n = 0;

    if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

        if (params.data()->val_AddPointsCalc() == "yes") {

            n = TCyclePointsNumber;
        }
        else {

            n = TCyclePointsNumber - TCycleAddPointsNumber;
        }

        array_n.resize(n);
        array_Me_brutto.resize(n);
        array_Ne_brutto.resize(n);
        array_N_fan.resize(n);
        array_w.resize(n);

        array_n[ 0] = idle;
        array_n[ 1] = A;
        array_n[ 2] = B;
        array_n[ 3] = B;
        array_n[ 4] = A;
        array_n[ 5] = A;
        array_n[ 6] = A;
        array_n[ 7] = B;
        array_n[ 8] = B;
        array_n[ 9] = C;
        array_n[10] = C;
        array_n[11] = C;
        array_n[12] = C;

        if (params.data()->val_AddPointsCalc() == "yes") {

            array_n[13] = a1;
            array_n[14] = a2;
            array_n[15] = a3;
        }

        for (ptrdiff_t i=0; i<n; i++) {

            array_N_fan[i] = N_fan_rated * pow(array_n[i] / n_rated, 3);
        }

        array_Ne_brutto[ 0] = 0;
        array_Ne_brutto[ 1] = Ne_A;
        array_Ne_brutto[ 2] = 0.50  * (Ne_B - array_N_fan[2]) + array_N_fan[2];
        array_Ne_brutto[ 3] = 0.75  * (Ne_B - array_N_fan[3]) + array_N_fan[3];
        array_Ne_brutto[ 4] = 0.50  * (Ne_A - array_N_fan[4]) + array_N_fan[4];
        array_Ne_brutto[ 5] = 0.75  * (Ne_A - array_N_fan[5]) + array_N_fan[5];
        array_Ne_brutto[ 6] = 0.25  * (Ne_A - array_N_fan[6]) + array_N_fan[6];
        array_Ne_brutto[ 7] = Ne_B;
        array_Ne_brutto[ 8] = 0.25  * (Ne_B - array_N_fan[8]) + array_N_fan[8];
        array_Ne_brutto[ 9] = Ne_C;
        array_Ne_brutto[10] = 0.25  * (Ne_C - array_N_fan[10]) + array_N_fan[10];
        array_Ne_brutto[11] = 0.75  * (Ne_C - array_N_fan[11]) + array_N_fan[11];
        array_Ne_brutto[12] = 0.50  * (Ne_C - array_N_fan[12]) + array_N_fan[12];

        if (params.data()->val_AddPointsCalc() == "yes") {

            array_Ne_brutto[13] = 0.875 * (Ne_a1 - array_N_fan[13]) + array_N_fan[13];
            array_Ne_brutto[14] = 0.625 * (Ne_a2 - array_N_fan[14]) + array_N_fan[14];
            array_Ne_brutto[15] = 0.375 * (Ne_a3 - array_N_fan[15]) + array_N_fan[15];
        }

        for (ptrdiff_t i=0; i<n; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.15;
        array_w[ 1] = 0.08;
        array_w[ 2] = 0.10;
        array_w[ 3] = 0.10;
        array_w[ 4] = 0.05;
        array_w[ 5] = 0.05;
        array_w[ 6] = 0.05;
        array_w[ 7] = 0.09;
        array_w[ 8] = 0.10;
        array_w[ 9] = 0.08;
        array_w[10] = 0.05;
        array_w[11] = 0.05;
        array_w[12] = 0.05;

        if (params.data()->val_AddPointsCalc() == "yes") {

            array_w[13] = 1.0;
            array_w[14] = 1.0;
            array_w[15] = 1.0;
        }
    }
    else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") ) {

        n = TCyclePointsNumber - TCycleAddPointsNumber;

        array_n.resize(n);
        array_Me_brutto.resize(n);
        array_Ne_brutto.resize(n);
        array_N_fan.resize(n);
        array_w.resize(n);

        array_n[ 0] = idle;
        array_n[ 1] = n_interim;
        array_n[ 2] = n_interim;
        array_n[ 3] = n_interim;
        array_n[ 4] = n_interim;
        array_n[ 5] = n_interim;
        array_n[ 6] = idle;
        array_n[ 7] = n_rated;
        array_n[ 8] = n_rated;
        array_n[ 9] = n_rated;
        array_n[10] = n_rated;
        array_n[11] = n_rated;
        array_n[12] = idle;

        for (ptrdiff_t i=0; i<n; i++) {

            array_N_fan[i] = N_fan_rated * pow(array_n[i] / n_rated, 3);
        }

        array_Ne_brutto[ 0] = 0;
        array_Ne_brutto[ 1] = 0.10 * (Ne_interim - array_N_fan[1]) + array_N_fan[1];
        array_Ne_brutto[ 2] = 0.25 * (Ne_interim - array_N_fan[2]) + array_N_fan[2];
        array_Ne_brutto[ 3] = 0.50 * (Ne_interim - array_N_fan[3]) + array_N_fan[3];
        array_Ne_brutto[ 4] = 0.75 * (Ne_interim - array_N_fan[4]) + array_N_fan[4];
        array_Ne_brutto[ 5] = Ne_interim;
        array_Ne_brutto[ 6] = 0;
        array_Ne_brutto[ 7] = Ne_rated;
        array_Ne_brutto[ 8] = 0.75 * (Ne_rated - array_N_fan[8]) + array_N_fan[8];
        array_Ne_brutto[ 9] = 0.50 * (Ne_rated - array_N_fan[9]) + array_N_fan[9];
        array_Ne_brutto[10] = 0.25 * (Ne_rated - array_N_fan[10]) + array_N_fan[10];
        array_Ne_brutto[11] = 0.10 * (Ne_rated - array_N_fan[11]) + array_N_fan[11];
        array_Ne_brutto[12] = 0;

        for (ptrdiff_t i=0; i<n; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.0833;
        array_w[ 1] = 0.080;
        array_w[ 2] = 0.080;
        array_w[ 3] = 0.080;
        array_w[ 4] = 0.080;
        array_w[ 5] = 0.250;
        array_w[ 6] = 0.0833;
        array_w[ 7] = 0.100;
        array_w[ 8] = 0.020;
        array_w[ 9] = 0.020;
        array_w[10] = 0.020;
        array_w[11] = 0.020;
        array_w[12] = 0.0833;
    }
    else if ( (std == "OST") || (std == "GOST") ) {

        n = TCyclePointsNumber - TCycleAddPointsNumber;

        array_n.resize(n);
        array_Me_brutto.resize(n);
        array_Ne_brutto.resize(n);
        array_N_fan.resize(n);
        array_w.resize(n);

        array_n[ 0] = idle;
        array_n[ 1] = n_interim;
        array_n[ 2] = n_interim;
        array_n[ 3] = n_interim;
        array_n[ 4] = n_interim;
        array_n[ 5] = n_interim;
        array_n[ 6] = idle;
        array_n[ 7] = n_rated;
        array_n[ 8] = n_rated;
        array_n[ 9] = n_rated;
        array_n[10] = n_rated;
        array_n[11] = n_rated;
        array_n[12] = idle;

        for (ptrdiff_t i=0; i<n; i++) {

            array_N_fan[i] = N_fan_rated * pow(array_n[i] / n_rated, 3);
        }

        array_Ne_brutto[ 0] = 0;
        if (std == "OST") { array_Ne_brutto[ 1] = 0.02 * (Ne_interim - array_N_fan[1]) + array_N_fan[1]; }
        else              { array_Ne_brutto[ 1] = 0.10 * (Ne_interim - array_N_fan[1]) + array_N_fan[1]; }
        array_Ne_brutto[ 2] = 0.25 * (Ne_interim - array_N_fan[2]) + array_N_fan[2];
        array_Ne_brutto[ 3] = 0.50 * (Ne_interim - array_N_fan[3]) + array_N_fan[3];
        array_Ne_brutto[ 4] = 0.75 * (Ne_interim - array_N_fan[4]) + array_N_fan[4];
        array_Ne_brutto[ 5] = Ne_interim;
        array_Ne_brutto[ 6] = 0;
        array_Ne_brutto[ 7] = Ne_rated;
        array_Ne_brutto[ 8] = 0.75 * (Ne_rated - array_N_fan[8]) + array_N_fan[8];
        array_Ne_brutto[ 9] = 0.50 * (Ne_rated - array_N_fan[9]) + array_N_fan[9];
        array_Ne_brutto[10] = 0.25 * (Ne_rated - array_N_fan[10]) + array_N_fan[10];
        if (std == "OST") { array_Ne_brutto[11] = 0.02 * (Ne_rated - array_N_fan[11]) + array_N_fan[11]; }
        else              { array_Ne_brutto[11] = 0.10 * (Ne_rated - array_N_fan[11]) + array_N_fan[11]; }
        array_Ne_brutto[12] = 0;

        for (ptrdiff_t i=0; i<n; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        if (std == "OST") {

            array_w[ 0] = 0.066666667;
            array_w[ 1] = 0.080;
            array_w[ 2] = 0.080;
            array_w[ 3] = 0.080;
            array_w[ 4] = 0.080;
            array_w[ 5] = 0.080;
            array_w[ 6] = 0.066666667;
            array_w[ 7] = 0.080;
            array_w[ 8] = 0.080;
            array_w[ 9] = 0.080;
            array_w[10] = 0.080;
            array_w[11] = 0.080;
            array_w[12] = 0.066666667;
        }
        else {

            array_w[ 0] = 0.0833;
            array_w[ 1] = 0.080;
            array_w[ 2] = 0.080;
            array_w[ 3] = 0.080;
            array_w[ 4] = 0.080;
            array_w[ 5] = 0.250;
            array_w[ 6] = 0.0833;
            array_w[ 7] = 0.100;
            array_w[ 8] = 0.020;
            array_w[ 9] = 0.020;
            array_w[10] = 0.020;
            array_w[11] = 0.020;
            array_w[12] = 0.0833;
        }
    }
    else if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
              (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ) {

        array_n.resize(ECyclePointsNumber);
        array_Me_brutto.resize(ECyclePointsNumber);
        array_Ne_brutto.resize(ECyclePointsNumber);
        array_N_fan.resize(ECyclePointsNumber);
        array_w.resize(ECyclePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;
        array_n[ 4] = n_interim;
        array_n[ 5] = n_interim;
        array_n[ 6] = n_interim;
        array_n[ 7] = idle;

        array_Ne_brutto[ 0] =         Ne_rated;
        array_Ne_brutto[ 1] = 0.75  * Ne_rated;
        array_Ne_brutto[ 2] = 0.50  * Ne_rated;
        array_Ne_brutto[ 3] = 0.10  * Ne_rated;
        array_Ne_brutto[ 4] =         Ne_interim;
        array_Ne_brutto[ 5] = 0.75  * Ne_interim;
        array_Ne_brutto[ 6] = 0.50  * Ne_interim;
        array_Ne_brutto[ 7] = 0;

        for (ptrdiff_t i=0; i<ECyclePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
            array_N_fan[i] = N_fan_rated * pow(array_n[i] / n_rated, 3);
        }

        array_w[ 0] = 0.15;
        array_w[ 1] = 0.15;
        array_w[ 2] = 0.15;
        array_w[ 3] = 0.10;
        array_w[ 4] = 0.10;
        array_w[ 5] = 0.10;
        array_w[ 6] = 0.10;
        array_w[ 7] = 0.15;
    }
    else if ( (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
              (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

        array_n.resize(FCyclePointsNumber);
        array_Me_brutto.resize(FCyclePointsNumber);
        array_Ne_brutto.resize(FCyclePointsNumber);
        array_N_fan.resize(FCyclePointsNumber);
        array_w.resize(FCyclePointsNumber);

        array_Ne_brutto[ 0] =         Ne_rated;
        array_Ne_brutto[ 1] = 0.75  * Ne_rated;
        array_Ne_brutto[ 2] = 0.50  * Ne_rated;
        array_Ne_brutto[ 2] = 0.25  * Ne_rated;
        array_Ne_brutto[ 3] = 0.10  * Ne_rated;

        for (ptrdiff_t i=0; i<FCyclePointsNumber; i++) {

            array_n[i] = n_rated;
            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
            array_N_fan[i] = N_fan_rated;
        }

        array_w[ 0] = 0.05;
        array_w[ 1] = 0.25;
        array_w[ 2] = 0.30;
        array_w[ 3] = 0.30;
        array_w[ 4] = 0.10;
    }
    else if (std == "C1") {

        array_n.resize(GC1CylcePointsNumber);
        array_Me_brutto.resize(GC1CylcePointsNumber);
        array_Ne_brutto.resize(GC1CylcePointsNumber);
        array_N_fan.resize(GC1CylcePointsNumber);
        array_w.resize(GC1CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;
        array_n[ 4] = n_interim;
        array_n[ 5] = n_interim;
        array_n[ 6] = n_interim;
        array_n[ 7] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.1 * Ne_rated;
        array_Ne_brutto[ 4] = Ne_interim;
        array_Ne_brutto[ 5] = 0.75 * Ne_interim;
        array_Ne_brutto[ 6] = 0.5 * Ne_interim;
        array_Ne_brutto[ 7] = 0;

        for (ptrdiff_t i=0; i<GC1CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.15;
        array_w[ 1] = 0.15;
        array_w[ 2] = 0.15;
        array_w[ 3] = 0.1;
        array_w[ 4] = 0.1;
        array_w[ 5] = 0.1;
        array_w[ 6] = 0.1;
        array_w[ 7] = 0.15;
    }
    else if (std == "D1") {

        array_n.resize(GD1CylcePointsNumber);
        array_Me_brutto.resize(GD1CylcePointsNumber);
        array_Ne_brutto.resize(GD1CylcePointsNumber);
        array_N_fan.resize(GD1CylcePointsNumber);
        array_w.resize(GD1CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;

        for (ptrdiff_t i=0; i<GD1CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.3;
        array_w[ 1] = 0.5;
        array_w[ 2] = 0.2;
    }
    else if (std == "D2") {

        array_n.resize(GD2CylcePointsNumber);
        array_Me_brutto.resize(GD2CylcePointsNumber);
        array_Ne_brutto.resize(GD2CylcePointsNumber);
        array_N_fan.resize(GD2CylcePointsNumber);
        array_w.resize(GD2CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;
        array_n[ 4] = n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4] = 0.1 * Ne_rated;

        for (ptrdiff_t i=0; i<GD2CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.05;
        array_w[ 1] = 0.25;
        array_w[ 2] = 0.3;
        array_w[ 3] = 0.3;
        array_w[ 4] = 0.1;
    }
    else if (std == "E1") {

        array_n.resize(GE1CylcePointsNumber);
        array_Me_brutto.resize(GE1CylcePointsNumber);
        array_Ne_brutto.resize(GE1CylcePointsNumber);
        array_N_fan.resize(GE1CylcePointsNumber);
        array_w.resize(GE1CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_interim;
        array_n[ 3] = n_interim;
        array_n[ 4] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.75 * Ne_interim;
        array_Ne_brutto[ 3] = 0.5 * Ne_interim;
        array_Ne_brutto[ 4] = 0;

        for (ptrdiff_t i=0; i<GE1CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.08;
        array_w[ 1] = 0.11;
        array_w[ 2] = 0.19;
        array_w[ 3] = 0.32;
        array_w[ 4] = 0.3;
    }
    else if (std == "E2") {

        array_n.resize(GE2CylcePointsNumber);
        array_Me_brutto.resize(GE2CylcePointsNumber);
        array_Ne_brutto.resize(GE2CylcePointsNumber);
        array_N_fan.resize(GE2CylcePointsNumber);
        array_w.resize(GE2CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;

        for (ptrdiff_t i=0; i<GE2CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.2;
        array_w[ 1] = 0.5;
        array_w[ 2] = 0.15;
        array_w[ 3] = 0.15;
    }
    else if (std == "E3") {

        array_n.resize(GE3CylcePointsNumber);
        array_Me_brutto.resize(GE3CylcePointsNumber);
        array_Ne_brutto.resize(GE3CylcePointsNumber);
        array_N_fan.resize(GE3CylcePointsNumber);
        array_w.resize(GE3CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = 0.91 * n_rated;
        array_n[ 2] = 0.8 * n_rated;
        array_n[ 3] = 0.63 * n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;

        array_w[ 0] = 0.2;
        array_w[ 1] = 0.5;
        array_w[ 2] = 0.15;
        array_w[ 3] = 0.15;
    }
    else if (std == "E5") {

        array_n.resize(GE5CylcePointsNumber);
        array_Me_brutto.resize(GE5CylcePointsNumber);
        array_Ne_brutto.resize(GE5CylcePointsNumber);
        array_N_fan.resize(GE5CylcePointsNumber);
        array_w.resize(GE5CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = 0.91 * n_rated;
        array_n[ 2] = 0.8 * n_rated;
        array_n[ 3] = 0.63 * n_rated;
        array_n[ 4] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4] = 0;

        array_w[ 0] = 0.08;
        array_w[ 1] = 0.13;
        array_w[ 2] = 0.17;
        array_w[ 3] = 0.32;
        array_w[ 4] = 0.3;
    }
    else if (std == "F") {

        array_n.resize(GFCylcePointsNumber);
        array_Me_brutto.resize(GFCylcePointsNumber);
        array_Ne_brutto.resize(GFCylcePointsNumber);
        array_N_fan.resize(GFCylcePointsNumber);
        array_w.resize(GFCylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_interim;
        array_n[ 2] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.5 * Ne_interim;
        array_Ne_brutto[ 2] = 0;

        for (ptrdiff_t i=0; i<GFCylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.25;
        array_w[ 1] = 0.15;
        array_w[ 2] = 0.6;
    }
    else if (std == "G1") {

        array_n.resize(GG1CylcePointsNumber);
        array_Me_brutto.resize(GG1CylcePointsNumber);
        array_Ne_brutto.resize(GG1CylcePointsNumber);
        array_N_fan.resize(GG1CylcePointsNumber);
        array_w.resize(GG1CylcePointsNumber);

        array_n[ 0] = n_interim;
        array_n[ 1] = n_interim;
        array_n[ 2] = n_interim;
        array_n[ 3] = n_interim;
        array_n[ 4] = n_interim;
        array_n[ 5] = idle;

        array_Ne_brutto[ 0] = Ne_interim;
        array_Ne_brutto[ 1] = 0.75 * Ne_interim;
        array_Ne_brutto[ 2] = 0.5 * Ne_interim;
        array_Ne_brutto[ 3] = 0.25 * Ne_interim;
        array_Ne_brutto[ 4] = 0.1 * Ne_interim;
        array_Ne_brutto[ 5] = 0;

        for (ptrdiff_t i=0; i<GG1CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.09;
        array_w[ 1] = 0.2;
        array_w[ 2] = 0.29;
        array_w[ 3] = 0.3;
        array_w[ 4] = 0.07;
        array_w[ 5] = 0.05;
    }
    else if (std == "G2") {

        array_n.resize(GG2CylcePointsNumber);
        array_Me_brutto.resize(GG2CylcePointsNumber);
        array_Ne_brutto.resize(GG2CylcePointsNumber);
        array_N_fan.resize(GG2CylcePointsNumber);
        array_w.resize(GG2CylcePointsNumber);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;
        array_n[ 4] = n_rated;
        array_n[ 5] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4] = 0.1 * Ne_rated;
        array_Ne_brutto[ 5] = 0;

        for (ptrdiff_t i=0; i<GG2CylcePointsNumber; i++) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.09;
        array_w[ 1] = 0.2;
        array_w[ 2] = 0.29;
        array_w[ 3] = 0.3;
        array_w[ 4] = 0.07;
        array_w[ 5] = 0.05;
    }
    else {

        qDebug() << "r49 ERROR: CyclePoints: fillArrays: incorrect program configuration!";

        return false;
    }

    return true;
}

QString CyclePoints::createReport() const {

    QString message = "";

    QString filenamePoints = config.data()->val_filenamePoints();
    string csvdelimiter = (config.data()->val_csvDelimiter()).toStdString();

    ofstream fout(filenamePoints.toAscii());

    if (!fout) {

        message += "libtoxic ERROR: CyclePoints: createReport: fout was not created!\n";
        qDebug() << "\nlibtoxic ERROR: CyclePoints: createReport: fout was not created!";

        return message;
    }

    fout << right << setw(WidthOfColumn) << setfill(' ') << "Point[-]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "n[min-1]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Me_b[Nm]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Ne_b[kW]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "N_fan[kW]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "w[-]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "t0[oC]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "B0[kPa]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Ra[%]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "dPn[mmH2O]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Gair[kg/h]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Gfuel[kg/h]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CNOx[ppm]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "gNOx[g/kWh]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CCO[ppm]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CCH[ppm]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CCO2in[%]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CCO2out[%]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "CO2[%]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Ka[m-1]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Ka[%]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "FSN[-]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "Pr[kPa]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "ts[oC]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "tauf[s]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "qmdw[g/s]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "qmdew[g/s]" << csvdelimiter;
    fout << right << setw(WidthOfColumn) << setfill(' ') << "rd[-]" << csvdelimiter << endl;

    QString std = params.data()->val_Standard();

    ptrdiff_t n = 0;

    if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

        if (params.data()->val_AddPointsCalc() == "yes") {

            n = TCyclePointsNumber;
        }
        else {

            n = TCyclePointsNumber - TCycleAddPointsNumber;
        }
    }
    else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") || (std == "OST") || (std == "GOST") ) {

        n = TCyclePointsNumber - TCycleAddPointsNumber;
    }
    else if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
              (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ) {

        n = ECyclePointsNumber;
    }
    else if ( (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
              (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

        n = FCyclePointsNumber;
    }
    else if (std == "C1") {

        n = GC1CylcePointsNumber;
    }
    else if (std == "D1") {

        n = GD1CylcePointsNumber;
    }
    else if (std == "D2") {

        n = GD2CylcePointsNumber;
    }
    else if (std == "E1") {

        n = GE1CylcePointsNumber;
    }
    else if (std == "E2") {

        n = GE2CylcePointsNumber;
    }
    else if (std == "E3") {

        n = GE3CylcePointsNumber;
    }
    else if (std == "E5") {

        n = GE5CylcePointsNumber;
    }
    else if (std == "F") {

        n = GFCylcePointsNumber;
    }
    else {

        fout.close();

        message += "libtoxic ERROR: CyclePoints: createReport: points can be calculated only for cycles!\n";
        qDebug() << "\nlibtoxic ERROR: CyclePoints: createReport: points can be calculated only for cycles!";

        return message;
    }

    for (ptrdiff_t i=0; i<n; i++) {

        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(        0) << (i + 1) << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_n[i] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_Me_brutto[i] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_Ne_brutto[i] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_N_fan[i] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision+1) << array_w[i] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << "0" << csvdelimiter << endl;
    }

    fout.close();

    message += "libtoxic: File \"" + filenamePoints + "\" rewrited.\n";

    qDebug() << "\nlibtoxic: File" << filenamePoints << "rewrited.";
    qDebug() << "libtoxic: Add measured data in this file.";

    return message;
}
