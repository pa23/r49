/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txConstants.h

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

#ifndef TXCONSTANTS_H
#define TXCONSTANTS_H

#include <QStringList>

namespace toxic {

const QStringList SRCDATACAPTIONS_11 = (
            QStringList()
            << "n_hi[min-1]"
            << "n_lo[min-1]"
            << "idle[min-1]"
            << "n_rated[min-1]"
            << "N_fan_rated[kW]"
            << "Ne_A[kW]"
            << "Ne_B[kW]"
            << "Ne_C[kW]"
            << "Ne_a1[kW]"
            << "Ne_a2[kW]"
            << "Ne_a3[kW]"
            );

const QStringList SRCDATACAPTIONS_6 = (
            QStringList()
            << "idle[min-1]"
            << "n_interim[min-1]"
            << "n_rated[min-1]"
            << "N_fan_rated[kW]"
            << "Ne_interim[kW]"
            << "Ne_rated[kW]"
            );

const QStringList SRCDATACAPTIONS_EMISSIONS = (
            QStringList()
            << "Point[-]"
            << "n[min-1]"
            << "Me_b[Nm]"
            << "Ne_b[kW]"
            << "N_fan[kW]"
            << "w[-]"
            << "t0[oC]"
            << "B0[kPa]"
            << "Ra[%]"
            << "dPn[kPa]"
            << "Gair[kg/h]"
            << "Gfuel[kg/h]"
            << "C_NOx[ppm]"
            << "gNOx[g/kWh]"
            << "C_CO[ppm]"
            << "C_CH[ppm]"
            << "C_CO2in[%]"
            << "C_CO2out[%]"
            << "C_O2[%]"
            << "Ka[m-1]"
            << "Ka[%]"
            << "FSN[-]"
            << "Pr[kPa]"
            << "ts[oC]"
            << "tauf[s]"
            << "qmdw[g/s]"
            << "qmdew[g/s]"
            << "rd[-]"
            );

const QStringList SRCDATACAPTIONS_REDPOWER = (
            QStringList()
            << "Point[-]"
            << "n[min-1]"
            << "Me_b[Nm]"
            << "t0[oC]"
            << "B0[kPa]"
            << "Ra[%]"
            << "S[kPa]"
            << "pk[kPa]"
            << "Gfuel[kg/h]"
            << "N_k[kW]"
            << "N_fan[kW]"
            << "t_c[oC]"
            << "t_oil[oC]"
            << "tk[oC]"
            << "tks[oC]"
            << "t_fuel[oC]"
            << "pks[kPa]"
            << "Gair[kg/h]"
            );

const int ESCPOINTSNUMBER = 13;
const int ESCADDPOINTSNUMBER = 3;
const int H8POINTSNUMBER = 8;
const int H5POINTSNUMBER = 5;
const int C1POINTSNUMBER = 8;
const int D1POINTSNUMBER = 3;
const int D2POINTSNUMBER = 5;
const int E1POINTSNUMBER = 5;
const int E2POINTSNUMBER = 4;
const int E3POINTSNUMBER = 4;
const int E5POINTSNUMBER = 5;
const int FPOINTSNUMBER = 3;
const int G1POINTSNUMBER = 6;
const int G2POINTSNUMBER = 6;

const int COLUMNWIDTH = 15;
const int PRECISION = 2;

enum {
    TASK_POINTS,
    TASK_EMISSIONS,
    TASK_REDUCEDPOWER,
    TASK_ABCSPEEDS,
    TASK_HELP
};

enum {
    // after modification fix:
    // Emissions*::checkSrcData()
    // EmissionsOnCycle::EmissionsOnCycle()
    // txEmissionsOnR49R96::saveReportHTML()
    STD_EU6,     // realized in EmissionsOnR49R96
    STD_EU5,
    STD_EU4,
    STD_EU3,
    STD_EU2,
    STD_EU1,
    STD_EU0,
    STD_OST3700123481,
    STD_GOST17220597,
    STD_R96E8,
    STD_R96F8,
    STD_R96G8,
    STD_R96D8,
    STD_R96E5,
    STD_R96F5,
    STD_R96G5,
    STD_R96D5,
    STD_R96H8,
    STD_R96I8,
    STD_R96J8,
    STD_R96K8,
    STD_R96H5,
    STD_R96I5,
    STD_R96J5,
    STD_R96K5,
    STD_C1,      // realized in EmissionsOnGOST51249
    STD_D1,
    STD_D2,
    STD_E1,
    STD_E2,
    STD_E3,
    STD_E5,
    STD_F,
    STD_G1,
    STD_G2,
    STD_FREECALC // realized in EmissionsOnR49R96
};

enum {
    CHARGINGTYPE_NO,
    CHARGINGTYPE_GASTURBINE
};

enum {
    FUELTYPE_DIESEL,
    FUELTYPE_MOTOR,
    FUELTYPE_MAZUT
};

enum {
    NOXSAMPLE_WET,
    NOXSAMPLE_DRY
};

enum {
    PTCALC_THROUGHSMOKE,
    PTCALC_THROUGHPTMASS,
    PTCALC_THROUGHSMOKEANDPTMASS,
    PTCALC_NO
};

enum {
    ADDPOINTSCALC_YES,
    ADDPOINTSCALC_NO
};

} // namespace toxic

#endif // TXCONSTANTS_H
