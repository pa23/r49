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

namespace toxic {

txPointsOfCycle::txPointsOfCycle(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts) :
    m_commonParameters(commpars),
    m_calculationOptions(calcopts),
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

    if ( srcdata.isEmpty() ) {
        throw txError("Incorrect source data array!");
    }

    if ( srcdata[0].size() != 11 || srcdata[0].size() != 6 ) {
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

void txPointsOfCycle::setSourceData() {

    //
}

void txPointsOfCycle::calculate() {

    //
}

QString txPointsOfCycle::createReports() {

    //
}

void txPointsOfCycle::prepArrays(ptrdiff_t n) {

    //
}

} // namespace toxic
