/*
    libtoxicparameters.h

    Copyright (C) 2009, 2010 Artem Petrov <pa2311@gmail.com>

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

#ifndef _LIBTOXICPARAMETERS_H
#define	_LIBTOXICPARAMETERS_H

#include <QString>

class LibtoxicParameters {

public:
    LibtoxicParameters();
    virtual ~LibtoxicParameters();
    void SetTask(QString);
    void SetVh(double *);
    void SetStandard(QString);
    void SetFuelType(QString);
    void SetNOxSample(QString);
    void SetPTcalc(QString);
    void SetPTmass(double *);
    void SetAddPointsCalc(QString);
    void SetCalcConfigFile(QString);
    QString GetTask() const;
    double GetVh() const;
    QString GetStandard() const;
    QString GetFuelType() const;
    QString GetNOxSample() const;
    QString GetPTcalc() const;
    double GetPTmass() const;
    QString GetAddPointsCalc() const;
    QString GetCalcConfigFile() const;

    bool ReadCalcConfigFile(QString);

private:
    LibtoxicParameters(const LibtoxicParameters &orig);
    LibtoxicParameters &operator =(const LibtoxicParameters &);
    QString task;
    double Vh;
    QString standard;
    QString FuelType;
    QString NOxSample;
    QString PTcalc;
    double PTmass;
    QString AddPointsCalc;
    QString CalcConfigFile;

};

#endif	/* _LIBTOXICPARAMETERS_H */
