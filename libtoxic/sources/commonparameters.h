/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: commonparameters.h

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

#ifndef COMMONPARAMETERS_H
#define COMMONPARAMETERS_H

#include <QString>

class CommonParameters {

public:

    explicit CommonParameters();
    virtual ~CommonParameters();

    void readConfigFile(const QString &);

    QString val_filenameSourceEU3() const { return filenameSourceEU3; }
    QString val_filenameSourceEU0() const { return filenameSourceEU0; }
    QString val_filenamePoints() const { return filenamePoints; }
    QString val_filenamePowers() const { return filenamePowers; }
    QString val_dirnameReports() const { return dirnameReports; }
    double val_Dn() const { return Dn; }
    double val_ConcO2air() const { return ConcO2air; }
    double val_Rr() const { return Rr; }
    double val_L0() const { return L0; }
    double val_L() const { return L; }
    double val_ConcCO2air() const { return ConcCO2air; }
    double val_WH() const { return WH; }
    double val_WO2() const { return WO2; }
    double val_WN() const { return WN; }
    double val_roAir() const { return roAir; }
    double val_muNO2() const { return muNO2; }
    double val_muCO() const { return muCO; }
    double val_muCH() const { return muCH; }

private:

    CommonParameters(const CommonParameters &orig);
    CommonParameters &operator =(const CommonParameters &);

    QString filenameSourceEU3;
    QString filenameSourceEU0;
    QString filenamePoints;
    QString filenamePowers;
    QString dirnameReports;
    double Dn;
    double ConcO2air;
    double Rr;
    double L0;
    double L;
    double ConcCO2air;
    double WH;
    double WO2;
    double WN;
    double roAir;
    double muNO2;
    double muCO;
    double muCH;

};

#endif /* COMMONPARAMETERS_H */
