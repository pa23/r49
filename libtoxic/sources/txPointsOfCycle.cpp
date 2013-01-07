/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txPointsOfCycle.cpp

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

#include "txPointsOfCycle.h"
#include "txError.h"
#include "txAuxiliaryFunctions.h"
#include "txConstants.h"
#include "txDataReader.h"

#include <QFile>
#include <QTextStream>

namespace toxic {

txPointsOfCycle::txPointsOfCycle(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts) :
    m_commonParameters(commpars),
    m_calculationOptions(calcopts),
    m_numberOfPoints(0),
    m_n_hi(0),
    m_n_lo(0),
    m_A(0),
    m_B(0),
    m_C(0),
    m_a1(0),
    m_a2(0),
    m_a3(0),
    m_n_ref(0),
    m_idle(0),
    m_n_rated(0),
    m_N_fan_rated(0),
    m_Ne_A(0),
    m_Ne_B(0),
    m_Ne_C(0),
    m_Ne_a1(0),
    m_Ne_a2(0),
    m_Ne_a3(0),
    m_n_interim(0),
    m_Ne_interim(0),
    m_Ne_rated(0) {

    if ( !m_calculationOptions->val_calcConfigFile().isEmpty() ) {

        m_calculationOptions->
                readCalcConfigFile(m_calculationOptions->val_calcConfigFile());
    }
}

txPointsOfCycle::~txPointsOfCycle() {
}

void txPointsOfCycle::setSourceData(const QVector<QVector<double> > &srcdata) {

    prepSrcData(srcdata);
}

void txPointsOfCycle::setSourceData() {

    const int currstd = m_calculationOptions->val_standard();

    QSharedPointer<txDataReader> dataReader(new txDataReader());

    if ( currstd == STD_EU6 || currstd == STD_EU5 ||
         currstd == STD_EU4 || currstd == STD_EU3 ) {
        dataReader->readFile(m_commonParameters->val_srcFileNameEU3(), " ");
    }
    else {
        dataReader->readFile(m_commonParameters->val_srcFileNameEU0(), " ");
    }

    QVector< QVector<double> > srcdata = dataReader->val_data();

    if ( srcdata.isEmpty() ) {
        throw txError("No data to calculate!");
    }

    prepSrcData(srcdata);
}

void txPointsOfCycle::calculate() {

    const int currstd = m_calculationOptions->val_standard();
    const int addpoints = m_calculationOptions->val_addPointsCalc();

    if ( currstd == STD_EU6 || currstd == STD_EU5 ||
         currstd == STD_EU4 || currstd == STD_EU3 ) {

        if ( addpoints == ADDPOINTSCALC_YES ) {
            m_numberOfPoints = ESCPOINTSNUMBER+ESCADDPOINTSNUMBER;
        }
        else {
            m_numberOfPoints = ESCPOINTSNUMBER;
        }

        prepArrays();

        ma_n[ 0] = m_idle;
        ma_n[ 1] = m_A;
        ma_n[ 2] = m_B;
        ma_n[ 3] = m_B;
        ma_n[ 4] = m_A;
        ma_n[ 5] = m_A;
        ma_n[ 6] = m_A;
        ma_n[ 7] = m_B;
        ma_n[ 8] = m_B;
        ma_n[ 9] = m_C;
        ma_n[10] = m_C;
        ma_n[11] = m_C;
        ma_n[12] = m_C;

        if ( addpoints == ADDPOINTSCALC_YES ) {
            ma_n[13] = m_a1;
            ma_n[14] = m_a2;
            ma_n[15] = m_a3;
        }

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_N_fan[i] = N_fan(m_N_fan_rated, ma_n[i], m_n_rated);
        }

        ma_Ne_brutto[ 0] = 0;
        ma_Ne_brutto[ 1] = m_Ne_A;
        ma_Ne_brutto[ 2] = 0.50 * (m_Ne_B - ma_N_fan[2]) + ma_N_fan[2];
        ma_Ne_brutto[ 3] = 0.75 * (m_Ne_B - ma_N_fan[3]) + ma_N_fan[3];
        ma_Ne_brutto[ 4] = 0.50 * (m_Ne_A - ma_N_fan[4]) + ma_N_fan[4];
        ma_Ne_brutto[ 5] = 0.75 * (m_Ne_A - ma_N_fan[5]) + ma_N_fan[5];
        ma_Ne_brutto[ 6] = 0.25 * (m_Ne_A - ma_N_fan[6]) + ma_N_fan[6];
        ma_Ne_brutto[ 7] = m_Ne_B;
        ma_Ne_brutto[ 8] = 0.25 * (m_Ne_B - ma_N_fan[8]) + ma_N_fan[8];
        ma_Ne_brutto[ 9] = m_Ne_C;
        ma_Ne_brutto[10] = 0.25 * (m_Ne_C - ma_N_fan[10]) + ma_N_fan[10];
        ma_Ne_brutto[11] = 0.75 * (m_Ne_C - ma_N_fan[11]) + ma_N_fan[11];
        ma_Ne_brutto[12] = 0.50 * (m_Ne_C - ma_N_fan[12]) + ma_N_fan[12];

        if ( addpoints == ADDPOINTSCALC_YES ) {
            ma_Ne_brutto[13] = 0.875 * (m_Ne_a1 - ma_N_fan[13]) + ma_N_fan[13];
            ma_Ne_brutto[14] = 0.625 * (m_Ne_a2 - ma_N_fan[14]) + ma_N_fan[14];
            ma_Ne_brutto[15] = 0.375 * (m_Ne_a3 - ma_N_fan[15]) + ma_N_fan[15];
        }

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[ 0] = 0.15;
        ma_w[ 1] = 0.08;
        ma_w[ 2] = 0.10;
        ma_w[ 3] = 0.10;
        ma_w[ 4] = 0.05;
        ma_w[ 5] = 0.05;
        ma_w[ 6] = 0.05;
        ma_w[ 7] = 0.09;
        ma_w[ 8] = 0.10;
        ma_w[ 9] = 0.08;
        ma_w[10] = 0.05;
        ma_w[11] = 0.05;
        ma_w[12] = 0.05;

        if ( addpoints == ADDPOINTSCALC_YES ) {
            ma_w[13] = 1.0;
            ma_w[14] = 1.0;
            ma_w[15] = 1.0;
        }
    }
    else if ( currstd == STD_EU2 || currstd == STD_EU1 || currstd == STD_EU0 ) {

        m_numberOfPoints = ESCPOINTSNUMBER;

        prepArrays();

        ma_n[ 0] = m_idle;
        ma_n[ 1] = m_n_interim;
        ma_n[ 2] = m_n_interim;
        ma_n[ 3] = m_n_interim;
        ma_n[ 4] = m_n_interim;
        ma_n[ 5] = m_n_interim;
        ma_n[ 6] = m_idle;
        ma_n[ 7] = m_n_rated;
        ma_n[ 8] = m_n_rated;
        ma_n[ 9] = m_n_rated;
        ma_n[10] = m_n_rated;
        ma_n[11] = m_n_rated;
        ma_n[12] = m_idle;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_N_fan[i] = N_fan(m_N_fan_rated, ma_n[i], m_n_rated);
        }

        ma_Ne_brutto[ 0] = 0;
        ma_Ne_brutto[ 1] = 0.10 * (m_Ne_interim - ma_N_fan[1]) + ma_N_fan[1];
        ma_Ne_brutto[ 2] = 0.25 * (m_Ne_interim - ma_N_fan[2]) + ma_N_fan[2];
        ma_Ne_brutto[ 3] = 0.50 * (m_Ne_interim - ma_N_fan[3]) + ma_N_fan[3];
        ma_Ne_brutto[ 4] = 0.75 * (m_Ne_interim - ma_N_fan[4]) + ma_N_fan[4];
        ma_Ne_brutto[ 5] = m_Ne_interim;
        ma_Ne_brutto[ 6] = 0;
        ma_Ne_brutto[ 7] = m_Ne_rated;
        ma_Ne_brutto[ 8] = 0.75 * (m_Ne_rated - ma_N_fan[8]) + ma_N_fan[8];
        ma_Ne_brutto[ 9] = 0.50 * (m_Ne_rated - ma_N_fan[9]) + ma_N_fan[9];
        ma_Ne_brutto[10] = 0.25 * (m_Ne_rated - ma_N_fan[10]) + ma_N_fan[10];
        ma_Ne_brutto[11] = 0.10 * (m_Ne_rated - ma_N_fan[11]) + ma_N_fan[11];
        ma_Ne_brutto[12] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[ 0] = 0.0833;
        ma_w[ 1] = 0.080;
        ma_w[ 2] = 0.080;
        ma_w[ 3] = 0.080;
        ma_w[ 4] = 0.080;
        ma_w[ 5] = 0.250;
        ma_w[ 6] = 0.0833;
        ma_w[ 7] = 0.100;
        ma_w[ 8] = 0.020;
        ma_w[ 9] = 0.020;
        ma_w[10] = 0.020;
        ma_w[11] = 0.020;
        ma_w[12] = 0.0833;
    }
    else if ( currstd == STD_OST || currstd == STD_GOST ) {

        m_numberOfPoints = ESCPOINTSNUMBER;

        prepArrays();

        ma_n[ 0] = m_idle;
        ma_n[ 1] = m_n_interim;
        ma_n[ 2] = m_n_interim;
        ma_n[ 3] = m_n_interim;
        ma_n[ 4] = m_n_interim;
        ma_n[ 5] = m_n_interim;
        ma_n[ 6] = m_idle;
        ma_n[ 7] = m_n_rated;
        ma_n[ 8] = m_n_rated;
        ma_n[ 9] = m_n_rated;
        ma_n[10] = m_n_rated;
        ma_n[11] = m_n_rated;
        ma_n[12] = m_idle;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_N_fan[i] = N_fan(m_N_fan_rated, ma_n[i], m_n_rated);
        }

        ma_Ne_brutto[ 0] = 0;

        if ( currstd == STD_OST ) {
            ma_Ne_brutto[ 1] = 0.02 * (m_Ne_interim - ma_N_fan[1]) + ma_N_fan[1];
        }
        else {
            ma_Ne_brutto[ 1] = 0.10 * (m_Ne_interim - ma_N_fan[1]) + ma_N_fan[1];
        }

        ma_Ne_brutto[ 2] = 0.25 * (m_Ne_interim - ma_N_fan[2]) + ma_N_fan[2];
        ma_Ne_brutto[ 3] = 0.50 * (m_Ne_interim - ma_N_fan[3]) + ma_N_fan[3];
        ma_Ne_brutto[ 4] = 0.75 * (m_Ne_interim - ma_N_fan[4]) + ma_N_fan[4];
        ma_Ne_brutto[ 5] = m_Ne_interim;
        ma_Ne_brutto[ 6] = 0;
        ma_Ne_brutto[ 7] = m_Ne_rated;
        ma_Ne_brutto[ 8] = 0.75 * (m_Ne_rated - ma_N_fan[8]) + ma_N_fan[8];
        ma_Ne_brutto[ 9] = 0.50 * (m_Ne_rated - ma_N_fan[9]) + ma_N_fan[9];
        ma_Ne_brutto[10] = 0.25 * (m_Ne_rated - ma_N_fan[10]) + ma_N_fan[10];

        if ( currstd == STD_OST ) {
            ma_Ne_brutto[11] = 0.02 * (m_Ne_rated - ma_N_fan[11]) + ma_N_fan[11];
        }
        else {
            ma_Ne_brutto[11] = 0.10 * (m_Ne_rated - ma_N_fan[11]) + ma_N_fan[11];
        }

        ma_Ne_brutto[12] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        if ( currstd == STD_OST ) {
            ma_w[ 0] = 0.066666667;
            ma_w[ 1] = 0.080;
            ma_w[ 2] = 0.080;
            ma_w[ 3] = 0.080;
            ma_w[ 4] = 0.080;
            ma_w[ 5] = 0.080;
            ma_w[ 6] = 0.066666667;
            ma_w[ 7] = 0.080;
            ma_w[ 8] = 0.080;
            ma_w[ 9] = 0.080;
            ma_w[10] = 0.080;
            ma_w[11] = 0.080;
            ma_w[12] = 0.066666667;
        }
        else {
            ma_w[ 0] = 0.0833;
            ma_w[ 1] = 0.080;
            ma_w[ 2] = 0.080;
            ma_w[ 3] = 0.080;
            ma_w[ 4] = 0.080;
            ma_w[ 5] = 0.250;
            ma_w[ 6] = 0.0833;
            ma_w[ 7] = 0.100;
            ma_w[ 8] = 0.020;
            ma_w[ 9] = 0.020;
            ma_w[10] = 0.020;
            ma_w[11] = 0.020;
            ma_w[12] = 0.0833;
        }
    }
    else if ( currstd == STD_R96E8 || currstd == STD_R96F8 ||
              currstd == STD_R96G8 || currstd == STD_R96D8 ||
              currstd == STD_R96H8 || currstd == STD_R96I8 ||
              currstd == STD_R96J8 || currstd == STD_R96K8 ) {

        m_numberOfPoints = H8POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;
        ma_n[3] = m_n_rated;
        ma_n[4] = m_n_interim;
        ma_n[5] = m_n_interim;
        ma_n[6] = m_n_interim;
        ma_n[7] = m_idle;

        ma_Ne_brutto[0] =         m_Ne_rated;
        ma_Ne_brutto[1] = 0.75  * m_Ne_rated;
        ma_Ne_brutto[2] = 0.50  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.10  * m_Ne_rated;
        ma_Ne_brutto[4] =         m_Ne_interim;
        ma_Ne_brutto[5] = 0.75  * m_Ne_interim;
        ma_Ne_brutto[6] = 0.50  * m_Ne_interim;
        ma_Ne_brutto[7] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
            ma_N_fan[i] = N_fan(m_N_fan_rated, ma_n[i], m_n_rated);
        }

        ma_w[0] = 0.15;
        ma_w[1] = 0.15;
        ma_w[2] = 0.15;
        ma_w[3] = 0.10;
        ma_w[4] = 0.10;
        ma_w[5] = 0.10;
        ma_w[6] = 0.10;
        ma_w[7] = 0.15;
    }
    else if ( currstd == STD_R96E5 || currstd == STD_R96F5 ||
              currstd == STD_R96G5 || currstd == STD_R96D5 ||
              currstd == STD_R96H5 || currstd == STD_R96I5 ||
              currstd == STD_R96J5 || currstd == STD_R96K5 ) {

        m_numberOfPoints = H5POINTSNUMBER;

        prepArrays();

        ma_Ne_brutto[0] =         m_Ne_rated;
        ma_Ne_brutto[1] = 0.75  * m_Ne_rated;
        ma_Ne_brutto[2] = 0.50  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25  * m_Ne_rated;
        ma_Ne_brutto[4] = 0.10  * m_Ne_rated;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_n[i] = m_n_rated;
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
            ma_N_fan[i] = m_N_fan_rated;
        }

        ma_w[0] = 0.05;
        ma_w[1] = 0.25;
        ma_w[2] = 0.30;
        ma_w[3] = 0.30;
        ma_w[4] = 0.10;
    }
    else if ( currstd == STD_C1 ) {

        m_numberOfPoints = C1POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;
        ma_n[3] = m_n_rated;
        ma_n[4] = m_n_interim;
        ma_n[5] = m_n_interim;
        ma_n[6] = m_n_interim;
        ma_n[7] = m_idle;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.1  * m_Ne_rated;
        ma_Ne_brutto[4] =        m_Ne_interim;
        ma_Ne_brutto[5] = 0.75 * m_Ne_interim;
        ma_Ne_brutto[6] = 0.5  * m_Ne_interim;
        ma_Ne_brutto[7] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.15;
        ma_w[1] = 0.15;
        ma_w[2] = 0.15;
        ma_w[3] = 0.1;
        ma_w[4] = 0.1;
        ma_w[5] = 0.1;
        ma_w[6] = 0.1;
        ma_w[7] = 0.15;
    }
    else if ( currstd == STD_D1 ) {

        m_numberOfPoints = D1POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.3;
        ma_w[1] = 0.5;
        ma_w[2] = 0.2;
    }
    else if ( currstd == STD_D2 ) {

        m_numberOfPoints = D2POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;
        ma_n[3] = m_n_rated;
        ma_n[4] = m_n_rated;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25 * m_Ne_rated;
        ma_Ne_brutto[4] = 0.1  * m_Ne_rated;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.05;
        ma_w[1] = 0.25;
        ma_w[2] = 0.3;
        ma_w[3] = 0.3;
        ma_w[4] = 0.1;
    }
    else if ( currstd == STD_E1 ) {

        m_numberOfPoints = E1POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_interim;
        ma_n[3] = m_n_interim;
        ma_n[4] = m_idle;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.75 * m_Ne_interim;
        ma_Ne_brutto[3] = 0.5  * m_Ne_interim;
        ma_Ne_brutto[4] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.08;
        ma_w[1] = 0.11;
        ma_w[2] = 0.19;
        ma_w[3] = 0.32;
        ma_w[4] = 0.3;
    }
    else if ( currstd == STD_E2 ) {

        m_numberOfPoints = E2POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;
        ma_n[3] = m_n_rated;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25 * m_Ne_rated;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.2;
        ma_w[1] = 0.5;
        ma_w[2] = 0.15;
        ma_w[3] = 0.15;
    }
    else if ( currstd == STD_E3 ) {

        m_numberOfPoints = E3POINTSNUMBER;

        prepArrays();

        ma_n[0] =        m_n_rated;
        ma_n[1] = 0.91 * m_n_rated;
        ma_n[2] = 0.8  * m_n_rated;
        ma_n[3] = 0.63 * m_n_rated;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25 * m_Ne_rated;

        ma_w[0] = 0.2;
        ma_w[1] = 0.5;
        ma_w[2] = 0.15;
        ma_w[3] = 0.15;
    }
    else if ( currstd == STD_E5 ) {

        m_numberOfPoints = E5POINTSNUMBER;

        prepArrays();

        ma_n[0] =        m_n_rated;
        ma_n[1] = 0.91 * m_n_rated;
        ma_n[2] = 0.8  * m_n_rated;
        ma_n[3] = 0.63 * m_n_rated;
        ma_n[4] =        m_idle;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25 * m_Ne_rated;
        ma_Ne_brutto[4] = 0;

        ma_w[0] = 0.08;
        ma_w[1] = 0.13;
        ma_w[2] = 0.17;
        ma_w[3] = 0.32;
        ma_w[4] = 0.3;
    }
    else if ( currstd == STD_F ) {

        m_numberOfPoints = FPOINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_interim;
        ma_n[2] = m_idle;

        ma_Ne_brutto[0] =       m_Ne_rated;
        ma_Ne_brutto[1] = 0.5 * m_Ne_interim;
        ma_Ne_brutto[2] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.25;
        ma_w[1] = 0.15;
        ma_w[2] = 0.6;
    }
    else if ( currstd == STD_G1 ) {

        m_numberOfPoints = G1POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_interim;
        ma_n[1] = m_n_interim;
        ma_n[2] = m_n_interim;
        ma_n[3] = m_n_interim;
        ma_n[4] = m_n_interim;
        ma_n[5] = m_idle;

        ma_Ne_brutto[0] = m_Ne_interim;
        ma_Ne_brutto[1] = 0.75 * m_Ne_interim;
        ma_Ne_brutto[2] = 0.5 * m_Ne_interim;
        ma_Ne_brutto[3] = 0.25 * m_Ne_interim;
        ma_Ne_brutto[4] = 0.1 * m_Ne_interim;
        ma_Ne_brutto[5] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.09;
        ma_w[1] = 0.2;
        ma_w[2] = 0.29;
        ma_w[3] = 0.3;
        ma_w[4] = 0.07;
        ma_w[5] = 0.05;
    }
    else if ( currstd == STD_G2 ) {

        m_numberOfPoints = G2POINTSNUMBER;

        prepArrays();

        ma_n[0] = m_n_rated;
        ma_n[1] = m_n_rated;
        ma_n[2] = m_n_rated;
        ma_n[3] = m_n_rated;
        ma_n[4] = m_n_rated;
        ma_n[5] = m_idle;

        ma_Ne_brutto[0] =        m_Ne_rated;
        ma_Ne_brutto[1] = 0.75 * m_Ne_rated;
        ma_Ne_brutto[2] = 0.5  * m_Ne_rated;
        ma_Ne_brutto[3] = 0.25 * m_Ne_rated;
        ma_Ne_brutto[4] = 0.1  * m_Ne_rated;
        ma_Ne_brutto[5] = 0;

        for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {
            ma_Me_brutto[i] = ma_Ne_brutto[i] * 9550.0 / ma_n[i];
        }

        ma_w[0] = 0.09;
        ma_w[1] = 0.2;
        ma_w[2] = 0.29;
        ma_w[3] = 0.3;
        ma_w[4] = 0.07;
        ma_w[5] = 0.05;
    }
    else {
        throw txError("Unknown value of parameter \"standard\"!");
    }
}

QString txPointsOfCycle::createReports() {

    const QString filenamePoints = m_commonParameters->val_srcFileNamePoints();

    QFile dataFile(filenamePoints);

    if ( !dataFile.open(QFile::WriteOnly) ) {
        throw txError("Can not open file " + filenamePoints + "!");
    }

    QTextStream fout(&dataFile);

    fout << right
         << qSetFieldWidth(COLUMNWIDTH);

    for ( ptrdiff_t i=0; i<SRCDATACAPTIONS_EMISSIONS.size(); i++ ) {
        fout << SRCDATACAPTIONS_EMISSIONS[i];
    }

    fout << qSetFieldWidth(0)
         << "\n"
         << fixed;

    for ( ptrdiff_t i=0; i<m_numberOfPoints; i++ ) {

        fout << qSetFieldWidth(COLUMNWIDTH)
             << qSetRealNumberPrecision(0);
        fout << (i + 1) << ma_n[i];
        fout << qSetRealNumberPrecision(PRECISION);
        fout << ma_Me_brutto[i] << ma_Ne_brutto[i] << ma_N_fan[i];
        fout << qSetRealNumberPrecision(PRECISION+1);
        fout << ma_w[i];
        fout << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0"
             << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0" << "0"
             << "0" << "0" << "0" << "0";
        fout << qSetFieldWidth(0) << "\n";
    }

    dataFile.close();

    return "libtoxic: File \"" + filenamePoints + "\" rewrited.\n";
}

void txPointsOfCycle::prepSrcData(const QVector<QVector<double> > &srcdata) {

    if ( srcdata.isEmpty() ) {
        throw txError("Incorrect source data array!");
    }

    if ( srcdata[0].size() != 11 && srcdata[0].size() != 6 ) {
        throw txError("Incorrect source data array!");
    }

    int currstd = m_calculationOptions->val_standard();

    if ( currstd == STD_EU6 || currstd == STD_EU5 ||
         currstd == STD_EU4 || currstd == STD_EU3 ) {

        m_n_hi        = srcdata[0][ 0];
        m_n_lo        = srcdata[0][ 1];
        m_idle        = srcdata[0][ 2];
        m_n_rated     = srcdata[0][ 3];
        m_N_fan_rated = srcdata[0][ 4];
        m_Ne_A        = srcdata[0][ 5];
        m_Ne_B        = srcdata[0][ 6];
        m_Ne_C        = srcdata[0][ 7];
        m_Ne_a1       = srcdata[0][ 8];
        m_Ne_a2       = srcdata[0][ 9];
        m_Ne_a3       = srcdata[0][10];

        ABC(m_n_hi, m_n_lo, &m_A, &m_B, &m_C, &m_a1, &m_a2, &m_a3, &m_n_ref);
    }
    else if ( (currstd == STD_EU2) || (currstd == STD_EU1)  || (currstd == STD_EU0) ||
              (currstd == STD_OST) || (currstd == STD_GOST) ||
              (currstd == STD_R96E8) || (currstd == STD_R96F8) ||
              (currstd == STD_R96G8) || (currstd == STD_R96D8) ||
              (currstd == STD_R96E5) || (currstd == STD_R96F5) ||
              (currstd == STD_R96G5) || (currstd == STD_R96D5) ||
              (currstd == STD_R96H8) || (currstd == STD_R96I8) ||
              (currstd == STD_R96J8) || (currstd == STD_R96K8) ||
              (currstd == STD_R96H5) || (currstd == STD_R96I5) ||
              (currstd == STD_R96J5) || (currstd == STD_R96K5) ||
              (currstd == STD_C1) || (currstd == STD_D1) || (currstd == STD_D2) ||
              (currstd == STD_E1) || (currstd == STD_E2) || (currstd == STD_E3) ||
              (currstd == STD_E5) ||
              (currstd == STD_F)  || (currstd == STD_G1) || (currstd == STD_G2) ) {

        m_idle        = srcdata[0][ 0];
        m_n_interim   = srcdata[0][ 1];
        m_n_rated     = srcdata[0][ 2];
        m_N_fan_rated = srcdata[0][ 3];
        m_Ne_interim  = srcdata[0][ 4];
        m_Ne_rated    = srcdata[0][ 5];
    }
    else {
        throw txError("Unknown value of parameter \"standard\"!");
    }
}

void txPointsOfCycle::prepArrays() {

    ma_n.clear();         ma_n.resize(m_numberOfPoints);
    ma_Me_brutto.clear(); ma_Me_brutto.resize(m_numberOfPoints);
    ma_Ne_brutto.clear(); ma_Ne_brutto.resize(m_numberOfPoints);
    ma_N_fan.clear();     ma_N_fan.resize(m_numberOfPoints);
    ma_w.clear();         ma_w.resize(m_numberOfPoints);
}

} // namespace toxic
