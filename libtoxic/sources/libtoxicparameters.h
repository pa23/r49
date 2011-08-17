/*
    libtoxicparameters.h

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

    void setTask(ptrdiff_t);          void setTask(QString);
    void setVh(double *);
    void setStandard(ptrdiff_t);      void setStandard(QString);
    void setChargingType(ptrdiff_t);  void setChargingType(QString);
    void setFuelType(ptrdiff_t);      void setFuelType(QString);
    void setNOxSample(ptrdiff_t);     void setNOxSample(QString);
    void setPTcalc(ptrdiff_t);        void setPTcalc(QString);
    void setPTmass(double *);
    void setAddPointsCalc(ptrdiff_t); void setAddPointsCalc(QString);
    void setCalcConfigFile(QString);

    ptrdiff_t val_Task()           const;
    double    val_Vh()             const;
    ptrdiff_t val_Standard()       const;
    ptrdiff_t val_ChargingType()   const;
    ptrdiff_t val_FuelType()       const;
    ptrdiff_t val_NOxSample()      const;
    ptrdiff_t val_PTcalc()         const;
    double    val_PTmass()         const;
    ptrdiff_t val_AddPointsCalc()  const;
    QString   val_CalcConfigFile() const;

    bool readCalcConfigFile(QString);

    QString defStandardName(ptrdiff_t) const;

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

    ptrdiff_t defTask         (QString) const;
    ptrdiff_t defStandard     (QString) const;
    ptrdiff_t defChargingType (QString) const;
    ptrdiff_t defFuelType     (QString) const;
    ptrdiff_t defNOxSample    (QString) const;
    ptrdiff_t defPTcalc       (QString) const;
    ptrdiff_t defAddPointsCalc(QString) const;

};

#endif	/* LIBTOXICPARAMETERS_H */
