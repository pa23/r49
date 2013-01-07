/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txCommonParameters.h

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

#ifndef TXCOMMONPARAMETERS_H
#define TXCOMMONPARAMETERS_H

#include <QString>

namespace toxic {

class txCommonParameters {

public:

    txCommonParameters();

    void readConfigFile(const QString &);

    QString val_srcFileNameEU3() const {
        return m_srcFileNameEU3;
    }

    QString val_srcFileNameEU0() const {
        return m_srcFileNameEU0;
    }

    QString val_srcFileNamePoints() const {
        return m_srcFileNamePoints;
    }

    QString val_srcFileNameRedPwr() const {
        return m_srcFileNameRedPwr;
    }

    QString val_reportsDirName() const {
        return m_reportsDirName;
    }

    double val_Dn() const {
        return m_Dn;
    }

    double val_concO2air() const {
        return m_concO2air;
    }

    double val_Rr() const {
        return m_Rr;
    }

    double val_L0() const {
        return m_L0;
    }

    double val_L() const {
        return m_L;
    }

    double val_concCO2air() const {
        return m_concCO2air;
    }

    double val_WH() const {
        return m_WH;
    }

    double val_WO2() const {
        return m_WO2;
    }

    double val_WN() const {
        return m_WN;
    }

    double val_muNO2() const {
        return m_muNO2;
    }

    double val_muCO() const {
        return m_muCO;
    }

    double val_muCH() const {
        return m_muCH;
    }

private:

    void createNeededFiles();

    QString m_srcFileNameEU3;
    QString m_srcFileNameEU0;
    QString m_srcFileNamePoints;
    QString m_srcFileNameRedPwr;
    QString m_reportsDirName;
    double m_Dn;
    double m_concO2air;
    double m_Rr;
    double m_L0;
    double m_L;
    double m_concCO2air;
    double m_WH;
    double m_WO2;
    double m_WN;
    double m_muNO2;
    double m_muCO;
    double m_muCH;

};

} // namespace toxic

#endif // TXCOMMONPARAMETERS_H
