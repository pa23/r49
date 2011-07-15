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

class LibtoxicParameters {

public:

    explicit LibtoxicParameters();
    virtual ~LibtoxicParameters();

    void setTask(QString);
    void setVh(double *);
    void setStandard(QString);
    void setFuelType(QString);
    void setNOxSample(QString);
    void setPTcalc(QString);
    void setPTmass(double *);
    void setAddPointsCalc(QString);
    void setReports(QString);
    void setCalcConfigFile(QString);

    QString val_Task() const;
    double val_Vh() const;
    QString val_Standard() const;
    QString val_FuelType() const;
    QString val_NOxSample() const;
    QString val_PTcalc() const;
    double val_PTmass() const;
    QString val_AddPointsCalc() const;
    QString val_Reports() const;
    QString val_CalcConfigFile() const;

    bool readCalcConfigFile(QString);

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
    QString Reports;
    QString CalcConfigFile;

};

#endif	/* LIBTOXICPARAMETERS_H */
