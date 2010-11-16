/*
    commonparameters.h

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

#ifndef COMMONPARAMETERS_H_
#define COMMONPARAMETERS_H_

#include <QString>

class CommonParameters {

public:
    CommonParameters();
    virtual ~CommonParameters();
    bool ReadConfigFile(QString);
    QString Get_csvDelimiter() const;
    QString Get_filenameSourceEU3() const;
    QString Get_filenameSourceEU0() const;
    QString Get_filenamePoints() const;
    QString Get_filenamePowers() const;
    QString Get_dirnameReports() const;
    double Get_Dn() const;
    double Get_ConcO2air() const;
    double Get_Rr() const;
    double Get_L0() const;
    double Get_L() const;
    double Get_ConcCO2air() const;
    double Get_WH() const;
    double Get_WO2() const;
    double Get_WN() const;
    double Get_roAir() const;
    double Get_muNO2() const;
    double Get_muCO() const;
    double Get_muCH() const;

private:
    CommonParameters(const CommonParameters &orig);
    CommonParameters &operator =(const CommonParameters &);
    QString csvDelimiter;
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

#endif /* COMMONPARAMETERS_H_ */
