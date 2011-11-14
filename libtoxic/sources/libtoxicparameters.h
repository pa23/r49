/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: libtoxicparameters.h

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

#ifndef LIBTOXICPARAMETERS_H
#define	LIBTOXICPARAMETERS_H

#include <QString>

#include "libtoxicconstants.h"

class LibtoxicParameters {

public:

    explicit LibtoxicParameters();
    virtual ~LibtoxicParameters();

    void setTask(const ptrdiff_t &);
    void setTask(const QString &);

    void setVh(const double &);

    void setStandard(const ptrdiff_t &);
    void setStandard(const QString &);

    void setChargingType(const ptrdiff_t &);
    void setChargingType(const QString &);

    void setFuelType(const ptrdiff_t &);
    void setFuelType(const QString &);

    void setNOxSample(const ptrdiff_t &);
    void setNOxSample(const QString &);

    void setPTcalc(const ptrdiff_t &);
    void setPTcalc(const QString &);

    void setPTmass(const double &);

    void setAddPointsCalc(const ptrdiff_t &);
    void setAddPointsCalc(const QString &);

    void setCalcConfigFile(const QString &);

    ptrdiff_t val_Task() const { return task; }
    double val_Vh() const { return Vh; }
    ptrdiff_t val_Standard() const { return standard; }
    ptrdiff_t val_ChargingType() const { return chargingType; }
    ptrdiff_t val_FuelType() const { return FuelType; }
    ptrdiff_t val_NOxSample() const { return NOxSample; }
    ptrdiff_t val_PTcalc() const { return PTcalc; }
    double val_PTmass() const { return PTmass; }
    ptrdiff_t val_AddPointsCalc() const { return AddPointsCalc; }
    QString val_CalcConfigFile() const { return CalcConfigFile; }

    bool readCalcConfigFile(const QString &);

    QString defStandardName(const ptrdiff_t &) const;

private:

    LibtoxicParameters(const LibtoxicParameters &orig);
    LibtoxicParameters &operator =(const LibtoxicParameters &);

    ptrdiff_t task;
    double    Vh;
    ptrdiff_t standard;
    ptrdiff_t chargingType;
    ptrdiff_t FuelType;
    ptrdiff_t NOxSample;
    ptrdiff_t PTcalc;
    double    PTmass;
    ptrdiff_t AddPointsCalc;
    QString   CalcConfigFile;

    //

    ptrdiff_t defTask         (const QString &) const;
    ptrdiff_t defStandard     (const QString &) const;
    ptrdiff_t defChargingType (const QString &) const;
    ptrdiff_t defFuelType     (const QString &) const;
    ptrdiff_t defNOxSample    (const QString &) const;
    ptrdiff_t defPTcalc       (const QString &) const;
    ptrdiff_t defAddPointsCalc(const QString &) const;

};

#endif	/* LIBTOXICPARAMETERS_H */
