/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txCalculationOptions.h

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

#ifndef TXCALCULATIONOPTIONS_H
#define TXCALCULATIONOPTIONS_H

#include <QString>

namespace toxic {

class txCalculationOptions {

public:

    txCalculationOptions();

    void setTask(int);
    void setTask(const QString &);
    int val_task() const {
        return m_task;
    }

    void setVh(double);
    double val_Vh() const {
        return m_Vh;
    }

    void setStandard(int);
    void setStandard(const QString &);
    int val_standard() const {
        return m_standard;
    }

    void setChargingType(int);
    void setChargingType(const QString &);
    int val_chargingType() const {
        return m_chargingType;
    }

    void setFuelType(int);
    void setFuelType(const QString &);
    int val_fuelType() const {
        return m_fuelType;
    }

    void setNOxSample(int);
    void setNOxSample(const QString &);
    int val_NOxSample() const {
        return m_NOxSample;
    }

    void setPTcalc(int);
    void setPTcalc(const QString &);
    int val_PTcalc() const {
        return m_PTcalc;
    }

    void setPTmass(double);
    double val_PTmass() const {
        return m_PTmass;
    }

    void setAddPointsCalc(int);
    void setAddPointsCalc(const QString &);
    int val_addPointsCalc() const {
        return m_addPointsCalc;
    }

    void setCalcConfigFile(const QString &);
    QString val_calcConfigFile() const {
        return m_calcConfigFile;
    }

    void readCalcConfigFile(const QString &);

    QString defStandardName(int) const;

private:

    int defTask(const QString &) const;
    int defStandard(const QString &) const;
    int defChargingType(const QString &) const;
    int defFuelType(const QString &) const;
    int defNOxSample(const QString &) const;
    int defPTcalc(const QString &) const;
    int defAddPointsCalc(const QString &) const;

    int m_task;
    double m_Vh;
    int m_standard;
    int m_chargingType;
    int m_fuelType;
    int m_NOxSample;
    int m_PTcalc;
    double m_PTmass;
    int m_addPointsCalc;
    QString m_calcConfigFile;

};

} // namespace toxic

#endif // TXCALCULATIONOPTIONS_H
