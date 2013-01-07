/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnCycle.cpp

    Copyright (C) 2013 Artem Petrov <pa2311@gmail.com>

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

#include "txEmissionsOnCycle.h"
#include "txEmissionsOnR49R96.h"
#include "txEmissionsOnGOST51249.h"
#include "txConstants.h"

namespace toxic {

txEmissionsOnCycle::txEmissionsOnCycle(
        const QSharedPointer<txCommonParameters> &commpars,
        const QSharedPointer<txCalculationOptions> &calcopts
        ) {

    int currstd = calcopts->val_standard();

    if ( (currstd >= STD_EU6 && currstd <= STD_R96K5) ||
         currstd == STD_FREECALC ) {
        m_cycle = QSharedPointer<txEmissionsOnR49R96>
                (new txEmissionsOnR49R96(commpars, calcopts));
    }
    else if ( currstd >= STD_C1 && currstd <= STD_G2 ) {
        m_cycle = QSharedPointer<txEmissionsOnGOST51249>
                (new txEmissionsOnGOST51249(commpars, calcopts));
    }
}

txEmissionsOnCycle::~txEmissionsOnCycle() {
}

void txEmissionsOnCycle::setSourceData(
        const QVector<QVector<double> > &srcdata
        ) {
    m_cycle->setSourceData(srcdata);
}

void txEmissionsOnCycle::setSourceData() {
    m_cycle->setSourceData();
}

void txEmissionsOnCycle::calculate() {
    m_cycle->calculate();
}

QString txEmissionsOnCycle::results() const {
    return m_cycle->results();
}

QString txEmissionsOnCycle::createReports() {
    return m_cycle->createReports();
}

QString txEmissionsOnCycle::lastReportDir() const {
    return m_cycle->lastReportDir();
}

} // namespace toxic
