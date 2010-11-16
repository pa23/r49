/*
    cyclepoints.cpp

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

#include "cyclepoints.h"
#include "libtoxicconstants.h"
#include "double2darray.h"
#include "precalc.h"
#include "csvread.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"

#include <fstream>
#include <iomanip>
#include <cmath>

#include <QDebug>
#include <QString>

using std::string;
using std::endl;
using std::ofstream;
using std::setfill;
using std::setw;
using std::right;
using std::setprecision;
using std::fixed;

CyclePoints::CyclePoints(LibtoxicParameters *prms, CommonParameters *cfg) : FillArrays_OK  (false),
                                                                            NumberOfPoints     (0),
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

    if (params->GetCalcConfigFile() != "_._") {

        params->ReadCalcConfigFile(params->GetCalcConfigFile());
    }
}

CyclePoints::~CyclePoints() {

    if (FillArrays_OK) {

        delete Array_n;
        delete Array_Me_brutto;
        delete Array_Ne_brutto;
        delete Array_N_fan;
        delete Array_w;
    }
}

CyclePoints::CyclePoints(const CyclePoints &orig) {

    (void)(&orig);
}

CyclePoints &CyclePoints::operator =(const CyclePoints &x) {

    (void)(&x);
    return *this;
}

bool CyclePoints::GetDataFromCSV(double **data, ptrdiff_t n, ptrdiff_t m) {

    QString std = params->GetStandard();
    QString csvdelimiter = config->Get_csvDelimiter();
    QString filenameSource = "";

    Double2DArray  *SourceData;
    double        **arraySourceData;

    if ( (n == 0) && (m == 0) ) {

        csvRead *ReaderSourceData = new csvRead();

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            filenameSource = config->Get_filenameSourceEU3();
        }
        else {

            filenameSource = config->Get_filenameSourceEU0();
        }

        if (!ReaderSourceData->ReadData(filenameSource, csvdelimiter, &NumberOfPoints)) {

            delete ReaderSourceData;

            qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: ReadData function returns false!";

            return false;
        }

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            SourceData = new Double2DArray(NumberOfPoints, EU3SrcDataParamsNumber);
            arraySourceData = SourceData->GetPointerOnArray();

            if (!ReaderSourceData->CheckArrayDimension(EU3SrcDataParamsNumber)) {

                delete SourceData;
                delete ReaderSourceData;

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: CheckArrayDimension function returns false!";

                return false;
            }

            if (!ReaderSourceData->FillArray(arraySourceData)) {

                delete SourceData;
                delete ReaderSourceData;

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: FillArray function returns false!";

                return false;
            }

            n_hi        = arraySourceData[StrsNumberForColumnCaption][ 0];
            n_lo        = arraySourceData[StrsNumberForColumnCaption][ 1];
            idle        = arraySourceData[StrsNumberForColumnCaption][ 2];
            n_rated     = arraySourceData[StrsNumberForColumnCaption][ 3];
            N_fan_rated = arraySourceData[StrsNumberForColumnCaption][ 4];
            Ne_A        = arraySourceData[StrsNumberForColumnCaption][ 5];
            Ne_B        = arraySourceData[StrsNumberForColumnCaption][ 6];
            Ne_C        = arraySourceData[StrsNumberForColumnCaption][ 7];
            Ne_a1       = arraySourceData[StrsNumberForColumnCaption][ 8];
            Ne_a2       = arraySourceData[StrsNumberForColumnCaption][ 9];
            Ne_a3       = arraySourceData[StrsNumberForColumnCaption][10];

            if (!CalcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

                delete SourceData;
                delete ReaderSourceData;

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: CalcABC function returns false!";

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

            SourceData = new Double2DArray(NumberOfPoints, EU0SrcDataParamsNumber);
            arraySourceData = SourceData->GetPointerOnArray();

            if (!ReaderSourceData->CheckArrayDimension(EU0SrcDataParamsNumber)) {

                delete SourceData;
                delete ReaderSourceData;

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: CheckArrayDimension function returns false!";

                return false;
            }

            if (!ReaderSourceData->FillArray(arraySourceData)) {

                delete SourceData;
                delete ReaderSourceData;

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: FillArray function returns false!";

                return false;
            }

            idle        = arraySourceData[StrsNumberForColumnCaption][ 0];
            n_interim   = arraySourceData[StrsNumberForColumnCaption][ 1];
            n_rated     = arraySourceData[StrsNumberForColumnCaption][ 2];
            N_fan_rated = arraySourceData[StrsNumberForColumnCaption][ 3];
            Ne_interim  = arraySourceData[StrsNumberForColumnCaption][ 4];
            Ne_rated    = arraySourceData[StrsNumberForColumnCaption][ 5];
        }
        else {

            delete ReaderSourceData;

            qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: incorrect program configuration!";

            return false;
        }

        delete ReaderSourceData;
        delete SourceData;
    }
    else if ( (n > 0) && (m > 0) ) {

        arraySourceData = data;

        NumberOfPoints = n;

        if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

            n_hi        = arraySourceData[StrsNumberForColumnCaption][ 0];
            n_lo        = arraySourceData[StrsNumberForColumnCaption][ 1];
            idle        = arraySourceData[StrsNumberForColumnCaption][ 2];
            n_rated     = arraySourceData[StrsNumberForColumnCaption][ 3];
            N_fan_rated = arraySourceData[StrsNumberForColumnCaption][ 4];
            Ne_A        = arraySourceData[StrsNumberForColumnCaption][ 5];
            Ne_B        = arraySourceData[StrsNumberForColumnCaption][ 6];
            Ne_C        = arraySourceData[StrsNumberForColumnCaption][ 7];
            Ne_a1       = arraySourceData[StrsNumberForColumnCaption][ 8];
            Ne_a2       = arraySourceData[StrsNumberForColumnCaption][ 9];
            Ne_a3       = arraySourceData[StrsNumberForColumnCaption][10];

            if (!CalcABC(&n_hi, &n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref)) {

                qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: CalcABC function returns false!";

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

            idle        = arraySourceData[StrsNumberForColumnCaption][ 0];
            n_interim   = arraySourceData[StrsNumberForColumnCaption][ 1];
            n_rated     = arraySourceData[StrsNumberForColumnCaption][ 2];
            N_fan_rated = arraySourceData[StrsNumberForColumnCaption][ 3];
            Ne_interim  = arraySourceData[StrsNumberForColumnCaption][ 4];
            Ne_rated    = arraySourceData[StrsNumberForColumnCaption][ 5];
        }
        else {

            qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: incorrect program configuration!";

            return false;
        }
    }
    else {

        qDebug() << "libtoxic ERROR: CyclePoints: GetDataFromCSV: Illegal data!";

        return false;
    }

    return true;
}

bool CyclePoints::FillArrays() {

    FillArrays_OK = false;

    QString std = params->GetStandard();

    if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

        ptrdiff_t n = 0;

        if (params->GetAddPointsCalc() == "yes") {

            n = TCyclePointsNumber;
        }
        else {

            n = TCyclePointsNumber - TCycleAddPointsNumber;
        }

        Array_n = new Double2DArray(n, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(n, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(n, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(n, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(n, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = idle;
        array_n[ 1][0] = A;
        array_n[ 2][0] = B;
        array_n[ 3][0] = B;
        array_n[ 4][0] = A;
        array_n[ 5][0] = A;
        array_n[ 6][0] = A;
        array_n[ 7][0] = B;
        array_n[ 8][0] = B;
        array_n[ 9][0] = C;
        array_n[10][0] = C;
        array_n[11][0] = C;
        array_n[12][0] = C;

        if (params->GetAddPointsCalc() == "yes") {

            array_n[13][0] = a1;
            array_n[14][0] = a2;
            array_n[15][0] = a3;
        }

        for (ptrdiff_t i=0; i<n; i++) {

            array_N_fan[i][0] = N_fan_rated * pow(array_n[i][0] / n_rated, 3);
        }

        array_Ne_brutto[ 0][0] = 0;
        array_Ne_brutto[ 1][0] = Ne_A;
        array_Ne_brutto[ 2][0] = 0.50  * (Ne_B - array_N_fan[2][0]) + array_N_fan[2][0];
        array_Ne_brutto[ 3][0] = 0.75  * (Ne_B - array_N_fan[3][0]) + array_N_fan[3][0];
        array_Ne_brutto[ 4][0] = 0.50  * (Ne_A - array_N_fan[4][0]) + array_N_fan[4][0];
        array_Ne_brutto[ 5][0] = 0.75  * (Ne_A - array_N_fan[5][0]) + array_N_fan[5][0];
        array_Ne_brutto[ 6][0] = 0.25  * (Ne_A - array_N_fan[6][0]) + array_N_fan[6][0];
        array_Ne_brutto[ 7][0] = Ne_B;
        array_Ne_brutto[ 8][0] = 0.25  * (Ne_B - array_N_fan[8][0]) + array_N_fan[8][0];
        array_Ne_brutto[ 9][0] = Ne_C;
        array_Ne_brutto[10][0] = 0.25  * (Ne_C - array_N_fan[10][0]) + array_N_fan[10][0];
        array_Ne_brutto[11][0] = 0.75  * (Ne_C - array_N_fan[11][0]) + array_N_fan[11][0];
        array_Ne_brutto[12][0] = 0.50  * (Ne_C - array_N_fan[12][0]) + array_N_fan[12][0];

        if (params->GetAddPointsCalc() == "yes") {

            array_Ne_brutto[13][0] = 0.875 * (Ne_a1 - array_N_fan[13][0]) + array_N_fan[13][0];
            array_Ne_brutto[14][0] = 0.625 * (Ne_a2 - array_N_fan[14][0]) + array_N_fan[14][0];
            array_Ne_brutto[15][0] = 0.375 * (Ne_a3 - array_N_fan[15][0]) + array_N_fan[15][0];
        }

        for (ptrdiff_t i=0; i<n; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.15;
        array_w[ 1][0] = 0.08;
        array_w[ 2][0] = 0.10;
        array_w[ 3][0] = 0.10;
        array_w[ 4][0] = 0.05;
        array_w[ 5][0] = 0.05;
        array_w[ 6][0] = 0.05;
        array_w[ 7][0] = 0.09;
        array_w[ 8][0] = 0.10;
        array_w[ 9][0] = 0.08;
        array_w[10][0] = 0.05;
        array_w[11][0] = 0.05;
        array_w[12][0] = 0.05;

        if (params->GetAddPointsCalc() == "yes") {

            array_w[13][0] = 1.0;
            array_w[14][0] = 1.0;
            array_w[15][0] = 1.0;
        }
    }
    else if ( (std == "EU2") || (std == "EU1") || (std == "EU0") ) {

        Array_n = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = idle;
        array_n[ 1][0] = n_interim;
        array_n[ 2][0] = n_interim;
        array_n[ 3][0] = n_interim;
        array_n[ 4][0] = n_interim;
        array_n[ 5][0] = n_interim;
        array_n[ 6][0] = idle;
        array_n[ 7][0] = n_rated;
        array_n[ 8][0] = n_rated;
        array_n[ 9][0] = n_rated;
        array_n[10][0] = n_rated;
        array_n[11][0] = n_rated;
        array_n[12][0] = idle;

        for (ptrdiff_t i=0; i<(TCyclePointsNumber-TCycleAddPointsNumber); i++) {

            array_N_fan[i][0] = N_fan_rated * pow(array_n[i][0] / n_rated, 3);
        }

        array_Ne_brutto[ 0][0] = 0;
        array_Ne_brutto[ 1][0] = 0.10 * (Ne_interim - array_N_fan[1][0]) + array_N_fan[1][0];
        array_Ne_brutto[ 2][0] = 0.25 * (Ne_interim - array_N_fan[2][0]) + array_N_fan[2][0];
        array_Ne_brutto[ 3][0] = 0.50 * (Ne_interim - array_N_fan[3][0]) + array_N_fan[3][0];
        array_Ne_brutto[ 4][0] = 0.75 * (Ne_interim - array_N_fan[4][0]) + array_N_fan[4][0];
        array_Ne_brutto[ 5][0] = Ne_interim;
        array_Ne_brutto[ 6][0] = 0;
        array_Ne_brutto[ 7][0] = Ne_rated;
        array_Ne_brutto[ 8][0] = 0.75 * (Ne_rated - array_N_fan[8][0]) + array_N_fan[8][0];
        array_Ne_brutto[ 9][0] = 0.50 * (Ne_rated - array_N_fan[9][0]) + array_N_fan[9][0];
        array_Ne_brutto[10][0] = 0.25 * (Ne_rated - array_N_fan[10][0]) + array_N_fan[10][0];
        array_Ne_brutto[11][0] = 0.10 * (Ne_rated - array_N_fan[11][0]) + array_N_fan[11][0];
        array_Ne_brutto[12][0] = 0;

        for (ptrdiff_t i=0; i<(TCyclePointsNumber-TCycleAddPointsNumber); i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.0833;
        array_w[ 1][0] = 0.080;
        array_w[ 2][0] = 0.080;
        array_w[ 3][0] = 0.080;
        array_w[ 4][0] = 0.080;
        array_w[ 5][0] = 0.250;
        array_w[ 6][0] = 0.0833;
        array_w[ 7][0] = 0.100;
        array_w[ 8][0] = 0.020;
        array_w[ 9][0] = 0.020;
        array_w[10][0] = 0.020;
        array_w[11][0] = 0.020;
        array_w[12][0] = 0.0833;
    }
    else if ( (std == "OST") || (std == "GOST") ) {

        Array_n = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(TCyclePointsNumber - TCycleAddPointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = idle;
        array_n[ 1][0] = n_interim;
        array_n[ 2][0] = n_interim;
        array_n[ 3][0] = n_interim;
        array_n[ 4][0] = n_interim;
        array_n[ 5][0] = n_interim;
        array_n[ 6][0] = idle;
        array_n[ 7][0] = n_rated;
        array_n[ 8][0] = n_rated;
        array_n[ 9][0] = n_rated;
        array_n[10][0] = n_rated;
        array_n[11][0] = n_rated;
        array_n[12][0] = idle;

        for (ptrdiff_t i=0; i<(TCyclePointsNumber-TCycleAddPointsNumber); i++) {

            array_N_fan[i][0] = N_fan_rated * pow(array_n[i][0] / n_rated, 3);
        }

        array_Ne_brutto[ 0][0] = 0;
        if (std == "OST") { array_Ne_brutto[ 1][0] = 0.02 * (Ne_interim - array_N_fan[1][0]) + array_N_fan[1][0]; }
        else              { array_Ne_brutto[ 1][0] = 0.10 * (Ne_interim - array_N_fan[1][0]) + array_N_fan[1][0]; }
        array_Ne_brutto[ 2][0] = 0.25 * (Ne_interim - array_N_fan[2][0]) + array_N_fan[2][0];
        array_Ne_brutto[ 3][0] = 0.50 * (Ne_interim - array_N_fan[3][0]) + array_N_fan[3][0];
        array_Ne_brutto[ 4][0] = 0.75 * (Ne_interim - array_N_fan[4][0]) + array_N_fan[4][0];
        array_Ne_brutto[ 5][0] = Ne_interim;
        array_Ne_brutto[ 6][0] = 0;
        array_Ne_brutto[ 7][0] = Ne_rated;
        array_Ne_brutto[ 8][0] = 0.75 * (Ne_rated - array_N_fan[8][0]) + array_N_fan[8][0];
        array_Ne_brutto[ 9][0] = 0.50 * (Ne_rated - array_N_fan[9][0]) + array_N_fan[9][0];
        array_Ne_brutto[10][0] = 0.25 * (Ne_rated - array_N_fan[10][0]) + array_N_fan[10][0];
        if (std == "OST") { array_Ne_brutto[11][0] = 0.02 * (Ne_rated - array_N_fan[11][0]) + array_N_fan[11][0]; }
        else              { array_Ne_brutto[11][0] = 0.10 * (Ne_rated - array_N_fan[11][0]) + array_N_fan[11][0]; }
        array_Ne_brutto[12][0] = 0;

        for (ptrdiff_t i=0; i<(TCyclePointsNumber-TCycleAddPointsNumber); i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        if (std == "OST") {

            array_w[ 0][0] = 0.066666667;
            array_w[ 1][0] = 0.080;
            array_w[ 2][0] = 0.080;
            array_w[ 3][0] = 0.080;
            array_w[ 4][0] = 0.080;
            array_w[ 5][0] = 0.080;
            array_w[ 6][0] = 0.066666667;
            array_w[ 7][0] = 0.080;
            array_w[ 8][0] = 0.080;
            array_w[ 9][0] = 0.080;
            array_w[10][0] = 0.080;
            array_w[11][0] = 0.080;
            array_w[12][0] = 0.066666667;
        }
        else {

            array_w[ 0][0] = 0.0833;
            array_w[ 1][0] = 0.080;
            array_w[ 2][0] = 0.080;
            array_w[ 3][0] = 0.080;
            array_w[ 4][0] = 0.080;
            array_w[ 5][0] = 0.250;
            array_w[ 6][0] = 0.0833;
            array_w[ 7][0] = 0.100;
            array_w[ 8][0] = 0.020;
            array_w[ 9][0] = 0.020;
            array_w[10][0] = 0.020;
            array_w[11][0] = 0.020;
            array_w[12][0] = 0.0833;
        }
    }
    else if ( (std == "r96E8") || (std == "r96F8") || (std == "r96G8") || (std == "r96D8") ||
              (std == "r96H8") || (std == "r96I8") || (std == "r96J8") || (std == "r96K8") ) {

        Array_n = new Double2DArray(ECyclePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(ECyclePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(ECyclePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(ECyclePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(ECyclePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;
        array_n[ 3][0] = n_rated;
        array_n[ 4][0] = n_interim;
        array_n[ 5][0] = n_interim;
        array_n[ 6][0] = n_interim;
        array_n[ 7][0] = idle;

        array_Ne_brutto[ 0][0] =         Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75  * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.50  * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.10  * Ne_rated;
        array_Ne_brutto[ 4][0] =         Ne_interim;
        array_Ne_brutto[ 5][0] = 0.75  * Ne_interim;
        array_Ne_brutto[ 6][0] = 0.50  * Ne_interim;
        array_Ne_brutto[ 7][0] = 0;

        for (ptrdiff_t i=0; i<ECyclePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
            array_N_fan[i][0] = N_fan_rated * pow(array_n[i][0] / n_rated, 3);
        }

        array_w[ 0][0] = 0.15;
        array_w[ 1][0] = 0.15;
        array_w[ 2][0] = 0.15;
        array_w[ 3][0] = 0.10;
        array_w[ 4][0] = 0.10;
        array_w[ 5][0] = 0.10;
        array_w[ 6][0] = 0.10;
        array_w[ 7][0] = 0.15;
    }
    else if ( (std == "r96E5") || (std == "r96F5") || (std == "r96G5") || (std == "r96D5") ||
              (std == "r96H5") || (std == "r96I5") || (std == "r96J5") || (std == "r96K5") ) {

        Array_n = new Double2DArray(FCyclePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(FCyclePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(FCyclePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(FCyclePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(FCyclePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_Ne_brutto[ 0][0] =         Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75  * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.50  * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.25  * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.10  * Ne_rated;

        for (ptrdiff_t i=0; i<FCyclePointsNumber; i++) {

            array_n[i][0] = n_rated;
            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
            array_N_fan[i][0] = N_fan_rated;
        }

        array_w[ 0][0] = 0.05;
        array_w[ 1][0] = 0.25;
        array_w[ 2][0] = 0.30;
        array_w[ 3][0] = 0.30;
        array_w[ 4][0] = 0.10;
    }
    else if (std == "C1") {

        Array_n = new Double2DArray(GC1CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GC1CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GC1CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GC1CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GC1CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;
        array_n[ 3][0] = n_rated;
        array_n[ 4][0] = n_interim;
        array_n[ 5][0] = n_interim;
        array_n[ 6][0] = n_interim;
        array_n[ 7][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.1 * Ne_rated;
        array_Ne_brutto[ 4][0] = Ne_interim;
        array_Ne_brutto[ 5][0] = 0.75 * Ne_interim;
        array_Ne_brutto[ 6][0] = 0.5 * Ne_interim;
        array_Ne_brutto[ 7][0] = 0;

        for (ptrdiff_t i=0; i<GC1CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.15;
        array_w[ 1][0] = 0.15;
        array_w[ 2][0] = 0.15;
        array_w[ 3][0] = 0.1;
        array_w[ 4][0] = 0.1;
        array_w[ 5][0] = 0.1;
        array_w[ 6][0] = 0.1;
        array_w[ 7][0] = 0.15;
    }
    else if (std == "D1") {

        Array_n = new Double2DArray(GD1CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GD1CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GD1CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GD1CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GD1CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;

        for (ptrdiff_t i=0; i<GD1CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.3;
        array_w[ 1][0] = 0.5;
        array_w[ 2][0] = 0.2;
    }
    else if (std == "D2") {

        Array_n = new Double2DArray(GD2CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GD2CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GD2CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GD2CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GD2CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;
        array_n[ 3][0] = n_rated;
        array_n[ 4][0] = n_rated;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4][0] = 0.1 * Ne_rated;

        for (ptrdiff_t i=0; i<GD2CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.05;
        array_w[ 1][0] = 0.25;
        array_w[ 2][0] = 0.3;
        array_w[ 3][0] = 0.3;
        array_w[ 4][0] = 0.1;
    }
    else if (std == "E1") {

        Array_n = new Double2DArray(GE1CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GE1CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GE1CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GE1CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GE1CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_interim;
        array_n[ 3][0] = n_interim;
        array_n[ 4][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.75 * Ne_interim;
        array_Ne_brutto[ 3][0] = 0.5 * Ne_interim;
        array_Ne_brutto[ 4][0] = 0;

        for (ptrdiff_t i=0; i<GE1CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.08;
        array_w[ 1][0] = 0.11;
        array_w[ 2][0] = 0.19;
        array_w[ 3][0] = 0.32;
        array_w[ 4][0] = 0.3;
    }
    else if (std == "E2") {

        Array_n = new Double2DArray(GE2CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GE2CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GE2CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GE2CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GE2CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;
        array_n[ 3][0] = n_rated;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_rated;

        for (ptrdiff_t i=0; i<GE2CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.2;
        array_w[ 1][0] = 0.5;
        array_w[ 2][0] = 0.15;
        array_w[ 3][0] = 0.15;
    }
    else if (std == "E3") {

        Array_n = new Double2DArray(GE3CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GE3CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GE3CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GE3CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GE3CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = 0.91 * n_rated;
        array_n[ 2][0] = 0.8 * n_rated;
        array_n[ 3][0] = 0.63 * n_rated;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_rated;

        array_w[ 0][0] = 0.2;
        array_w[ 1][0] = 0.5;
        array_w[ 2][0] = 0.15;
        array_w[ 3][0] = 0.15;
    }
    else if (std == "E5") {

        Array_n = new Double2DArray(GE5CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GE5CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GE5CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GE5CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GE5CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = 0.91 * n_rated;
        array_n[ 2][0] = 0.8 * n_rated;
        array_n[ 3][0] = 0.63 * n_rated;
        array_n[ 4][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4][0] = 0;

        array_w[ 0][0] = 0.08;
        array_w[ 1][0] = 0.13;
        array_w[ 2][0] = 0.17;
        array_w[ 3][0] = 0.32;
        array_w[ 4][0] = 0.3;
    }
    else if (std == "F") {

        Array_n = new Double2DArray(GFCylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GFCylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GFCylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GFCylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GFCylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_interim;
        array_n[ 2][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.5 * Ne_interim;
        array_Ne_brutto[ 2][0] = 0;

        for (ptrdiff_t i=0; i<GFCylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.25;
        array_w[ 1][0] = 0.15;
        array_w[ 2][0] = 0.6;
    }
    else if (std == "G1") {

        Array_n = new Double2DArray(GG1CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GG1CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GG1CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GG1CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GG1CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_interim;
        array_n[ 1][0] = n_interim;
        array_n[ 2][0] = n_interim;
        array_n[ 3][0] = n_interim;
        array_n[ 4][0] = n_interim;
        array_n[ 5][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_interim;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_interim;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_interim;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_interim;
        array_Ne_brutto[ 4][0] = 0.1 * Ne_interim;
        array_Ne_brutto[ 5][0] = 0;

        for (ptrdiff_t i=0; i<GG1CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.09;
        array_w[ 1][0] = 0.2;
        array_w[ 2][0] = 0.29;
        array_w[ 3][0] = 0.3;
        array_w[ 4][0] = 0.07;
        array_w[ 5][0] = 0.05;
    }
    else if (std == "G2") {

        Array_n = new Double2DArray(GG2CylcePointsNumber, 1);
        array_n = Array_n->GetPointerOnArray();

        Array_Me_brutto = new Double2DArray(GG2CylcePointsNumber, 1);
        array_Me_brutto = Array_Me_brutto->GetPointerOnArray();

        Array_Ne_brutto = new Double2DArray(GG2CylcePointsNumber, 1);
        array_Ne_brutto = Array_Ne_brutto->GetPointerOnArray();

        Array_N_fan = new Double2DArray(GG2CylcePointsNumber, 1);
        array_N_fan = Array_N_fan->GetPointerOnArray();

        Array_w = new Double2DArray(GG2CylcePointsNumber, 1);
        array_w = Array_w->GetPointerOnArray();

        array_n[ 0][0] = n_rated;
        array_n[ 1][0] = n_rated;
        array_n[ 2][0] = n_rated;
        array_n[ 3][0] = n_rated;
        array_n[ 4][0] = n_rated;
        array_n[ 5][0] = idle;

        array_Ne_brutto[ 0][0] = Ne_rated;
        array_Ne_brutto[ 1][0] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2][0] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3][0] = 0.25 * Ne_rated;
        array_Ne_brutto[ 4][0] = 0.1 * Ne_rated;
        array_Ne_brutto[ 5][0] = 0;

        for (ptrdiff_t i=0; i<GG2CylcePointsNumber; i++) {

            array_Me_brutto[i][0] = array_Ne_brutto[i][0] * 9550.0 / array_n[i][0];
        }

        array_w[ 0][0] = 0.09;
        array_w[ 1][0] = 0.2;
        array_w[ 2][0] = 0.29;
        array_w[ 3][0] = 0.3;
        array_w[ 4][0] = 0.07;
        array_w[ 5][0] = 0.05;
    }
    else {

        qDebug() << "r49 ERROR: CyclePoints: FillArrays: incorrect program configuration!";

        return false;
    }

    FillArrays_OK = true;

    return true;
}

QString CyclePoints::CreateReport() const {

    QString message = "";

    QString filenamePoints = config->Get_filenamePoints();
    string csvdelimiter = (config->Get_csvDelimiter()).toStdString();

    ofstream fout(filenamePoints.toAscii());

    if (!fout) {

        message += "libtoxic ERROR: CyclePoints: CreateReport: fout was not created!\n";
        qDebug() << "\nlibtoxic ERROR: CyclePoints: CreateReport: fout was not created!";

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

    QString std = params->GetStandard();

    ptrdiff_t n = 0;
    if ( (std == "EU6") || (std == "EU5") || (std == "EU4") || (std == "EU3") ) {

        if (params->GetAddPointsCalc() == "yes") {

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

        message += "libtoxic ERROR: CyclePoints: CreateReport: points can be calculated only for cycles!\n";
        qDebug() << "\nlibtoxic ERROR: CyclePoints: CreateReport: points can be calculated only for cycles!";

        return message;
    }

    for (ptrdiff_t i=0; i<n; i++) {

        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(        0) << (i + 1) << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_n[i][0] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_Me_brutto[i][0] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_Ne_brutto[i][0] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision) << array_N_fan[i][0] << csvdelimiter;
        fout << fixed << right << setw(WidthOfColumn - 1) << setfill(' ') << setprecision(Precision+1) << array_w[i][0] << csvdelimiter;
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
