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

    //
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
