/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: cyclepoints.cpp

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

#include "cyclepoints.h"
#include "libtoxicconstants.h"
#include "precalc.h"
#include "csvread.h"
#include "libtoxicparameters.h"
#include "commonparameters.h"
#include "toxicerror.h"

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include <cmath>

CyclePoints::CyclePoints(const QSharedPointer<LibtoxicParameters> &prms,
                         const QSharedPointer<CommonParameters> &cfg) :
    params(prms), // calculatin settings
    config(cfg),  // r49.cong file
    n_hi       (0),
    n_lo       (0),
    A          (0),
    B          (0),
    C          (0),
    a1         (0),
    a2         (0),
    a3         (0),
    n_ref      (0),
    idle       (0),
    n_rated    (0),
    N_fan_rated(0),
    Ne_A       (0),
    Ne_B       (0),
    Ne_C       (0),
    Ne_a1      (0),
    Ne_a2      (0),
    Ne_a3      (0),
    n_interim  (0),
    Ne_interim (0),
    Ne_rated   (0) {

    if (params->valCalcConfigFile() != "_._") {

        params->readCalcConfigFile(params->valCalcConfigFile());
    }
}

CyclePoints::~CyclePoints() {
}

void CyclePoints::readCSV(const QVector< QVector<double> > &data) {

    ptrdiff_t std = params->valStandard();

    QString filenameSource;

    QVector< QVector<double> > arraySourceData;

    if ( data.isEmpty() ) {

        if ( (std == STD_EU6) || (std == STD_EU5) ||
             (std == STD_EU4) || (std == STD_EU3) ) {

            filenameSource = config->valFileNameSourceEU3();
        }
        else {

            filenameSource = config->valFileNameSourceEU0();
        }

        QSharedPointer<csvRead>
                readerSourceData(new csvRead(filenameSource,
                                             " ",
                                             STRSNUMBERFORCOLUMNCAPTION));

        readerSourceData->readFile();
        arraySourceData = readerSourceData->csvData();

        if ( (std == STD_EU6) || (std == STD_EU5) ||
             (std == STD_EU4) || (std == STD_EU3) ) {

            if ( arraySourceData.isEmpty() ) {

                throw ToxicError("No data to calculate!");
            }

            if ( arraySourceData[0].size() != EU3SRCDATAPARAMSNUMBER ) {

                throw ToxicError("Incorrect source data!");
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

            calcABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
        }
        else if ( (std == STD_EU2) || (std == STD_EU1)  || (std == STD_EU0) ||
                  (std == STD_OST) || (std == STD_GOST) ||
                  (std == STD_R96E8) || (std == STD_R96F8) ||
                  (std == STD_R96G8) || (std == STD_R96D8) ||
                  (std == STD_R96E5) || (std == STD_R96F5) ||
                  (std == STD_R96G5) || (std == STD_R96D5) ||
                  (std == STD_R96H8) || (std == STD_R96I8) ||
                  (std == STD_R96J8) || (std == STD_R96K8) ||
                  (std == STD_R96H5) || (std == STD_R96I5) ||
                  (std == STD_R96J5) || (std == STD_R96K5) ||
                  (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                  (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                  (std == STD_E5) ||
                  (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            if ( arraySourceData.isEmpty() ) {

                throw ToxicError("No data to calculate!");
            }

            if ( arraySourceData[0].size() != EU0SRCDATAPARAMSNUMBER ) {

                throw ToxicError("Incorrect source data!");
            }

            idle        = arraySourceData[0][ 0];
            n_interim   = arraySourceData[0][ 1];
            n_rated     = arraySourceData[0][ 2];
            N_fan_rated = arraySourceData[0][ 3];
            Ne_interim  = arraySourceData[0][ 4];
            Ne_rated    = arraySourceData[0][ 5];
        }
        else {

            throw ToxicError("Unknown value of parameter \"standard\"!");
        }
    }
    else {

        arraySourceData = data;

        if ( (std == STD_EU6) || (std == STD_EU5) ||
             (std == STD_EU4) || (std == STD_EU3) ) {

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

            calcABC(n_hi, n_lo, &A, &B, &C, &a1, &a2, &a3, &n_ref);
        }
        else if ( (std == STD_EU2) || (std == STD_EU1)  || (std == STD_EU0) ||
                  (std == STD_OST) || (std == STD_GOST) ||
                  (std == STD_R96E8) || (std == STD_R96F8) ||
                  (std == STD_R96G8) || (std == STD_R96D8) ||
                  (std == STD_R96E5) || (std == STD_R96F5) ||
                  (std == STD_R96G5) || (std == STD_R96D5) ||
                  (std == STD_R96H8) || (std == STD_R96I8) ||
                  (std == STD_R96J8) || (std == STD_R96K8) ||
                  (std == STD_R96H5) || (std == STD_R96I5) ||
                  (std == STD_R96J5) || (std == STD_R96K5) ||
                  (std == STD_C1) || (std == STD_D1) || (std == STD_D2) ||
                  (std == STD_E1) || (std == STD_E2) || (std == STD_E3) ||
                  (std == STD_E5) ||
                  (std == STD_F)  || (std == STD_G1) || (std == STD_G2) ) {

            idle        = arraySourceData[0][ 0];
            n_interim   = arraySourceData[0][ 1];
            n_rated     = arraySourceData[0][ 2];
            N_fan_rated = arraySourceData[0][ 3];
            Ne_interim  = arraySourceData[0][ 4];
            Ne_rated    = arraySourceData[0][ 5];
        }
        else {

            throw ToxicError("Unknown value of parameter \"standard\"!");
        }
    }
}

void CyclePoints::fillArrays() {

    ptrdiff_t std = params->valStandard();
    ptrdiff_t addpc = params->valAddPointsCalc();

    ptrdiff_t n = 0;

    if ( (std == STD_EU6) || (std == STD_EU5) ||
         (std == STD_EU4) || (std == STD_EU3) ) {

        if ( addpc == ADDPOINTSCALC_YES ) {

            n = TCYCLEPOINTSNUMBER;
        }
        else {

            n = TCYCLEPOINTSNUMBER - TCYCLEADDPOINTSNUMBER;
        }

        arraysInit(n);

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

        if ( addpc == ADDPOINTSCALC_YES ) {

            array_n[13] = a1;
            array_n[14] = a2;
            array_n[15] = a3;
        }

        for ( ptrdiff_t i=0; i<n; i++ ) {

            array_N_fan[i] = N_fan(N_fan_rated, array_n[i], n_rated);
        }

        array_Ne_brutto[ 0] = 0;
        array_Ne_brutto[ 1] = Ne_A;
        array_Ne_brutto[ 2] = 0.50 * (Ne_B - array_N_fan[2]) + array_N_fan[2];
        array_Ne_brutto[ 3] = 0.75 * (Ne_B - array_N_fan[3]) + array_N_fan[3];
        array_Ne_brutto[ 4] = 0.50 * (Ne_A - array_N_fan[4]) + array_N_fan[4];
        array_Ne_brutto[ 5] = 0.75 * (Ne_A - array_N_fan[5]) + array_N_fan[5];
        array_Ne_brutto[ 6] = 0.25 * (Ne_A - array_N_fan[6]) + array_N_fan[6];
        array_Ne_brutto[ 7] = Ne_B;
        array_Ne_brutto[ 8] = 0.25 * (Ne_B - array_N_fan[8]) + array_N_fan[8];
        array_Ne_brutto[ 9] = Ne_C;
        array_Ne_brutto[10] = 0.25 * (Ne_C - array_N_fan[10]) + array_N_fan[10];
        array_Ne_brutto[11] = 0.75 * (Ne_C - array_N_fan[11]) + array_N_fan[11];
        array_Ne_brutto[12] = 0.50 * (Ne_C - array_N_fan[12]) + array_N_fan[12];

        if ( addpc == ADDPOINTSCALC_YES ) {

            array_Ne_brutto[13] =
                    0.875 * (Ne_a1 - array_N_fan[13]) + array_N_fan[13];

            array_Ne_brutto[14] =
                    0.625 * (Ne_a2 - array_N_fan[14]) + array_N_fan[14];

            array_Ne_brutto[15] =
                    0.375 * (Ne_a3 - array_N_fan[15]) + array_N_fan[15];
        }

        for ( ptrdiff_t i=0; i<n; i++ ) {

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

        if ( addpc == ADDPOINTSCALC_YES ) {

            array_w[13] = 1.0;
            array_w[14] = 1.0;
            array_w[15] = 1.0;
        }
    }
    else if ( (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) ) {

        n = TCYCLEPOINTSNUMBER - TCYCLEADDPOINTSNUMBER;

        arraysInit(n);

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

        for ( ptrdiff_t i=0; i<n; i++ ) {

            array_N_fan[i] = N_fan(N_fan_rated, array_n[i], n_rated);
        }

        array_Ne_brutto[ 0] = 0;

        array_Ne_brutto[ 1] =
                0.10 * (Ne_interim - array_N_fan[1]) + array_N_fan[1];

        array_Ne_brutto[ 2] =
                0.25 * (Ne_interim - array_N_fan[2]) + array_N_fan[2];

        array_Ne_brutto[ 3] =
                0.50 * (Ne_interim - array_N_fan[3]) + array_N_fan[3];

        array_Ne_brutto[ 4] =
                0.75 * (Ne_interim - array_N_fan[4]) + array_N_fan[4];

        array_Ne_brutto[ 5] = Ne_interim;

        array_Ne_brutto[ 6] = 0;

        array_Ne_brutto[ 7] = Ne_rated;

        array_Ne_brutto[ 8] =
                0.75 * (Ne_rated - array_N_fan[8]) + array_N_fan[8];

        array_Ne_brutto[ 9] =
                0.50 * (Ne_rated - array_N_fan[9]) + array_N_fan[9];

        array_Ne_brutto[10] =
                0.25 * (Ne_rated - array_N_fan[10]) + array_N_fan[10];

        array_Ne_brutto[11] =
                0.10 * (Ne_rated - array_N_fan[11]) + array_N_fan[11];

        array_Ne_brutto[12] = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

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
    else if ( (std == STD_OST) || (std == STD_GOST) ) {

        n = TCYCLEPOINTSNUMBER - TCYCLEADDPOINTSNUMBER;

        arraysInit(n);

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

        for ( ptrdiff_t i=0; i<n; i++ ) {

            array_N_fan[i] = N_fan(N_fan_rated, array_n[i], n_rated);
        }

        array_Ne_brutto[ 0] = 0;

        if (std == STD_OST) {

            array_Ne_brutto[ 1] =
                    0.02 * (Ne_interim - array_N_fan[1]) + array_N_fan[1];
        }
        else {

            array_Ne_brutto[ 1] =
                    0.10 * (Ne_interim - array_N_fan[1]) + array_N_fan[1];
        }

        array_Ne_brutto[ 2] =
                0.25 * (Ne_interim - array_N_fan[2]) + array_N_fan[2];

        array_Ne_brutto[ 3] =
                0.50 * (Ne_interim - array_N_fan[3]) + array_N_fan[3];

        array_Ne_brutto[ 4] =
                0.75 * (Ne_interim - array_N_fan[4]) + array_N_fan[4];

        array_Ne_brutto[ 5] = Ne_interim;

        array_Ne_brutto[ 6] = 0;

        array_Ne_brutto[ 7] = Ne_rated;

        array_Ne_brutto[ 8] =
                0.75 * (Ne_rated - array_N_fan[8]) + array_N_fan[8];

        array_Ne_brutto[ 9] =
                0.50 * (Ne_rated - array_N_fan[9]) + array_N_fan[9];

        array_Ne_brutto[10] =
                0.25 * (Ne_rated - array_N_fan[10]) + array_N_fan[10];

        if ( std == STD_OST ) {

            array_Ne_brutto[11] =
                    0.02 * (Ne_rated - array_N_fan[11]) + array_N_fan[11];
        }
        else {

            array_Ne_brutto[11] =
                    0.10 * (Ne_rated - array_N_fan[11]) + array_N_fan[11];
        }

        array_Ne_brutto[12] = 0;

        for ( ptrdiff_t i=0; i<n; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        if ( std == STD_OST ) {

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
    else if ( (std == STD_R96E8) || (std == STD_R96F8) ||
              (std == STD_R96G8) || (std == STD_R96D8) ||
              (std == STD_R96H8) || (std == STD_R96I8) ||
              (std == STD_R96J8) || (std == STD_R96K8) ) {

        arraysInit(ECYCLEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<ECYCLEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
            array_N_fan[i] = N_fan(N_fan_rated, array_n[i], n_rated);
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
    else if ( (std == STD_R96E5) || (std == STD_R96F5) ||
              (std == STD_R96G5) || (std == STD_R96D5) ||
              (std == STD_R96H5) || (std == STD_R96I5) ||
              (std == STD_R96J5) || (std == STD_R96K5) ) {

        arraysInit(FCYCLEPOINTSNUMBER);

        array_Ne_brutto[ 0] =         Ne_rated;
        array_Ne_brutto[ 1] = 0.75  * Ne_rated;
        array_Ne_brutto[ 2] = 0.50  * Ne_rated;
        array_Ne_brutto[ 3] = 0.25  * Ne_rated;
        array_Ne_brutto[ 4] = 0.10  * Ne_rated;

        for ( ptrdiff_t i=0; i<FCYCLEPOINTSNUMBER; i++ ) {

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
    else if ( std == STD_C1 ) {

        arraysInit(GC1CYLCEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<GC1CYLCEPOINTSNUMBER; i++ ) {

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
    else if ( std == STD_D1 ) {

        arraysInit(GD1CYLCEPOINTSNUMBER);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;

        for ( ptrdiff_t i=0; i<GD1CYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.3;
        array_w[ 1] = 0.5;
        array_w[ 2] = 0.2;
    }
    else if ( std == STD_D2 ) {

        arraysInit(GD2CYLCEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<GD2CYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.05;
        array_w[ 1] = 0.25;
        array_w[ 2] = 0.3;
        array_w[ 3] = 0.3;
        array_w[ 4] = 0.1;
    }
    else if ( std == STD_E1 ) {

        arraysInit(GE1CYLCEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<GE1CYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.08;
        array_w[ 1] = 0.11;
        array_w[ 2] = 0.19;
        array_w[ 3] = 0.32;
        array_w[ 4] = 0.3;
    }
    else if ( std == STD_E2 ) {

        arraysInit(GE2CYLCEPOINTSNUMBER);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_rated;
        array_n[ 2] = n_rated;
        array_n[ 3] = n_rated;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.75 * Ne_rated;
        array_Ne_brutto[ 2] = 0.5 * Ne_rated;
        array_Ne_brutto[ 3] = 0.25 * Ne_rated;

        for ( ptrdiff_t i=0; i<GE2CYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.2;
        array_w[ 1] = 0.5;
        array_w[ 2] = 0.15;
        array_w[ 3] = 0.15;
    }
    else if ( std == STD_E3 ) {

        arraysInit(GE3CYLCEPOINTSNUMBER);

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
    else if ( std == STD_E5 ) {

        arraysInit(GE5CYLCEPOINTSNUMBER);

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
    else if ( std == STD_F ) {

        arraysInit(GFCYLCEPOINTSNUMBER);

        array_n[ 0] = n_rated;
        array_n[ 1] = n_interim;
        array_n[ 2] = idle;

        array_Ne_brutto[ 0] = Ne_rated;
        array_Ne_brutto[ 1] = 0.5 * Ne_interim;
        array_Ne_brutto[ 2] = 0;

        for ( ptrdiff_t i=0; i<GFCYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.25;
        array_w[ 1] = 0.15;
        array_w[ 2] = 0.6;
    }
    else if ( std == STD_G1 ) {

        arraysInit(GG1CYLCEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<GG1CYLCEPOINTSNUMBER; i++ ) {

            array_Me_brutto[i] = array_Ne_brutto[i] * 9550.0 / array_n[i];
        }

        array_w[ 0] = 0.09;
        array_w[ 1] = 0.2;
        array_w[ 2] = 0.29;
        array_w[ 3] = 0.3;
        array_w[ 4] = 0.07;
        array_w[ 5] = 0.05;
    }
    else if ( std == STD_G2 ) {

        arraysInit(GG2CYLCEPOINTSNUMBER);

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

        for ( ptrdiff_t i=0; i<GG2CYLCEPOINTSNUMBER; i++ ) {

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

        throw ToxicError("Unknown value of parameter \"standard\"!");
    }
}

QString CyclePoints::createReport() const {

    QString message;

    QString filenamePoints = config->valFileNamePoints();

    QFile data1(filenamePoints);

    if ( !data1.open(QFile::WriteOnly) ) {

        throw ToxicError("Can not open file " + filenamePoints + "!");
    }

    QTextStream fout1(&data1);

    fout1 << fixed << right << qSetFieldWidth(WIDTHOFCOLUMN)
          << "Point[-]"   << "n[min-1]"    << "Me_b[Nm]"
          << "Ne_b[kW]"   << "N_fan[kW]"   << "w[-]"
          << "t0[oC]"     << "B0[kPa]"     << "Ra[%]"
          << "dPn[mmH2O]" << "Gair[kg/h]"  << "Gfuel[kg/h]"
          << "C_NOx[ppm]" << "gNOx[g/kWh]" << "C_CO[ppm]"
          << "C_CH[ppm]"  << "C_CO2in[%]"  << "C_CO2out[%]"
          << "C_O2[%]"    << "Ka[m-1]"     << "Ka[%]"
          << "FSN[-]"     << "Pr[kPa]"     << "ts[oC]"
          << "tauf[s]"    << "qmdw[g/s]"   << "qmdew[g/s]"
          << "rd[-]"      << "\n";

    ptrdiff_t std = params->valStandard();

    ptrdiff_t n = 0;

    if ( (std == STD_EU6) || (std == STD_EU5) ||
         (std == STD_EU4) || (std == STD_EU3) ) {

        if ( params->valAddPointsCalc() == ADDPOINTSCALC_YES ) {

            n = TCYCLEPOINTSNUMBER;
        }
        else {

            n = TCYCLEPOINTSNUMBER - TCYCLEADDPOINTSNUMBER;
        }
    }
    else if ( (std == STD_EU2) || (std == STD_EU1) || (std == STD_EU0) ||
              (std == STD_OST) || (std == STD_GOST) ) {

        n = TCYCLEPOINTSNUMBER - TCYCLEADDPOINTSNUMBER;
    }
    else if ( (std == STD_R96E8) || (std == STD_R96F8) ||
              (std == STD_R96G8) || (std == STD_R96D8) ||
              (std == STD_R96H8) || (std == STD_R96I8) ||
              (std == STD_R96J8) || (std == STD_R96K8) ) {

        n = ECYCLEPOINTSNUMBER;
    }
    else if ( (std == STD_R96E5) || (std == STD_R96F5) ||
              (std == STD_R96G5) || (std == STD_R96D5) ||
              (std == STD_R96H5) || (std == STD_R96I5) ||
              (std == STD_R96J5) || (std == STD_R96K5) ) {

        n = FCYCLEPOINTSNUMBER;
    }
    else if ( std == STD_C1 ) {

        n = GC1CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_D1 ) {

        n = GD1CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_D2 ) {

        n = GD2CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_E1 ) {

        n = GE1CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_E2 ) {

        n = GE2CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_E3 ) {

        n = GE3CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_E5 ) {

        n = GE5CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_F ) {

        n = GFCYLCEPOINTSNUMBER;
    }
    else if ( std == STD_G1 ) {

        n = GG1CYLCEPOINTSNUMBER;
    }
    else if ( std == STD_G2 ) {

        n = GG2CYLCEPOINTSNUMBER;
    }
    else {

        data1.close();

        throw ToxicError("Unknown value of parameter \"standard\"!");
    }

    for ( ptrdiff_t i=0; i<n; i++ ) {

        fout1 << qSetFieldWidth(WIDTHOFCOLUMN);

        fout1 << qSetRealNumberPrecision(0) << (i + 1) << array_n[i];

        fout1 << qSetRealNumberPrecision(PRECISION)
              << array_Me_brutto[i] << array_Ne_brutto[i] << array_N_fan[i];

        fout1 << qSetRealNumberPrecision(PRECISION+1) << array_w[i];

        fout1 << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0"
              << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0"
              << "0" << "0" << "0" << "0";

        fout1 << qSetFieldWidth(0) << "\n";
    }

    data1.close();

    message += "libtoxic: File \"" + filenamePoints + "\" rewrited.\n";

    return message;
}

void CyclePoints::arraysInit(const ptrdiff_t &n) {

    array_n.clear();         array_n.resize(n);
    array_Me_brutto.clear(); array_Me_brutto.resize(n);
    array_Ne_brutto.clear(); array_Ne_brutto.resize(n);
    array_N_fan.clear();     array_N_fan.resize(n);
    array_w.clear();         array_w.resize(n);
}
