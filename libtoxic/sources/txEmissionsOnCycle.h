/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txEmissionsOnCycle.h

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

/******************************************************************************\
 *                                                                            *
 *    This class was designed for use in client applications to calculate     *
 *    emissions on cycles.                                                    *
 *                                                                            *
\******************************************************************************/

#ifndef TXEMISSIONSONCYCLE_H
#define TXEMISSIONSONCYCLE_H

#include "txCalculationInterface.h"
#include "txCommonParameters.h"
#include "txCalculationOptions.h"

#include <QString>
#include <QSharedPointer>

namespace toxic {

class txEmissionsOnCycle : public txCalculationInterface {

public:

    txEmissionsOnCycle(const QSharedPointer<txCommonParameters> &,
                       const QSharedPointer<txCalculationOptions> &);
    virtual ~txEmissionsOnCycle();

    void setSourceData(const QVector< QVector<double> > &);
    void setSourceData(); // srcFileNamePoints from CommonParameters will be used
    void calculate();
    QString results() const;
    QString createReports();
    QString lastReportDir() const;

private:

    txEmissionsOnCycle(const txEmissionsOnCycle &);
    txEmissionsOnCycle & operator=(const txEmissionsOnCycle &);

    QSharedPointer<txCalculationInterface> m_cycle;

};

} // namespace toxic

#endif // TXEMISSIONSONCYCLE_H
