/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: commonparameters.h

    Copyright (C) 2009-2012 Artem Petrov <pa2311@gmail.com>

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

    CommonParameters();
    ~CommonParameters();

    void readConfigFile(const QString &);

    QString valFileNameSourceEU3() const { return fileNameSourceEU3; }
    QString valFileNameSourceEU0() const { return fileNameSourceEU0; }
    QString valFileNamePoints()    const { return fileNamePoints;    }
    QString valFileNamePowers()    const { return fileNamePowers;    }
    QString valDirNameReports()    const { return dirNameReports;    }
    double  valDn()                const { return Dn;                }
    double  valConcO2air()         const { return concO2air;         }
    double  valRr()                const { return Rr;                }
    double  valL0()                const { return L0;                }
    double  valL()                 const { return L;                 }
    double  valConcCO2air()        const { return concCO2air;        }
    double  valWH()                const { return WH;                }
    double  valWO2()               const { return WO2;               }
    double  valWN()                const { return WN;                }
    double  valmuNO2()             const { return muNO2;             }
    double  valmuCO()              const { return muCO;              }
    double  valmuCH()              const { return muCH;              }

private:

    CommonParameters(const CommonParameters &);
    void operator=(const CommonParameters &);

    QString fileNameSourceEU3;
    QString fileNameSourceEU0;
    QString fileNamePoints;
    QString fileNamePowers;
    QString dirNameReports;
    double Dn;
    double concO2air;
    double Rr;
    double L0;
    double L;
    double concCO2air;
    double WH;
    double WO2;
    double WN;
    double muNO2;
    double muCO;
    double muCH;

};

#endif /* COMMONPARAMETERS_H */
