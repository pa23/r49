/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txCalculationOptions.cpp

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

#include "txCalculationOptions.h"
#include "txConstants.h"
#include "txError.h"

#include <QStringList>
#include <QFile>
#include <QRegExp>

namespace toxic {

txCalculationOptions::txCalculationOptions() :
    m_task           (TASK_EMISSIONS),
    m_Vh             (0),
    m_standard       (STD_EU4),
    m_chargingType   (CHARGINGTYPE_GASTURBINE),
    m_fuelType       (FUELTYPE_DIESEL),
    m_NOxSample      (NOXSAMPLE_WET),
    m_PTcalc         (PTCALC_NO),
    m_PTmass         (0),
    m_addPointsCalc  (ADDPOINTSCALC_NO),
    m_calcConfigFile ("") {
}

void txCalculationOptions::setTask(int task_) {
    m_task = task_;
}

void txCalculationOptions::setTask(const QString &task_) {
    m_task = defTask(task_);
}

void txCalculationOptions::setVh(double Vh_) {
    m_Vh = Vh_;
}

void txCalculationOptions::setStandard(int standart_) {
    m_standard = standart_;
}

void txCalculationOptions::setStandard(const QString &standart_) {
    m_standard = defStandard(standart_);
}

void txCalculationOptions::setChargingType(int chargingType_) {
    m_chargingType = chargingType_;
}

void txCalculationOptions::setChargingType(const QString &chargingType_) {
    m_chargingType = defChargingType(chargingType_);
}

void txCalculationOptions::setFuelType(int fuelType_) {
    m_fuelType = fuelType_;
}

void txCalculationOptions::setFuelType(const QString &fuelType_) {
    m_fuelType = defFuelType(fuelType_);
}

void txCalculationOptions::setNOxSample(int NOxSample_) {
    m_NOxSample = NOxSample_;
}

void txCalculationOptions::setNOxSample(const QString &NOxSample_) {
    m_NOxSample = defNOxSample(NOxSample_);
}

void txCalculationOptions::setPTcalc(int PTcalc_) {
    m_PTcalc = PTcalc_;
}

void txCalculationOptions::setPTcalc(const QString &PTcalc_) {
    m_PTcalc = defPTcalc(PTcalc_);
}

void txCalculationOptions::setPTmass(double PTmass_) {
    m_PTmass = PTmass_;
}

void txCalculationOptions::setAddPointsCalc(int addPointsCalc_) {
    m_addPointsCalc = addPointsCalc_;
}

void txCalculationOptions::setAddPointsCalc(const QString &addPointsCalc_) {
    m_addPointsCalc = defAddPointsCalc(addPointsCalc_);
}

void txCalculationOptions::setCalcConfigFile(const QString &calcConfigFile_) {
    m_calcConfigFile = calcConfigFile_;
}

void txCalculationOptions::readCalcConfigFile(const QString &calcConfigFileName) {

    QFile calcConfFile(calcConfigFileName);

    if ( !calcConfFile.open(QIODevice::ReadOnly) ) {
        throw txError("Can not open file " + calcConfigFileName + "!");
    }

    QString s;
    QStringList elements;

    while ( !calcConfFile.atEnd() ) {

        s = calcConfFile.readLine().trimmed();

        if ( !s.isEmpty() ) {

            elements = s.split("=", QString::SkipEmptyParts);

            if ( elements[0] == "task" ) {
                m_task = defTask(elements[1]);
            }
            else if ( elements[0] == "Vh" ) {
                m_Vh = elements[1].toDouble();
            }
            else if ( elements[0] == "standard" ) {
                m_standard = defStandard(elements[1]);
            }
            else if ( elements[0] == "chargingType" ) {
                m_chargingType = defChargingType(elements[1]);
            }
            else if ( elements[0] == "fuelType" ) {
                m_fuelType = defFuelType(elements[1]);
            }
            else if ( elements[0] == "NOxSample" ) {
                m_NOxSample = defNOxSample(elements[1]);
            }
            else if ( elements[0] == "PTcalc" ) {
                m_PTcalc = defPTcalc(elements[1]);
            }
            else if ( elements[0] == "PTmass" ) {
                m_PTmass = elements[1].toDouble();
            }
            else if ( elements[0] == "addPointsCalc" ) {
                m_addPointsCalc = defAddPointsCalc(elements[1]);
            }
            else if ( elements[0] == "calcConfigFile" ) {
                m_calcConfigFile = elements[1];
            }

            elements.clear();
        }
    }

    elements.clear();

    calcConfFile.close();
}

QString txCalculationOptions::defStandardName(int val) const {

    if      ( val == STD_EU6     ) { return "R49_Euro-6";         }
    else if ( val == STD_EU5     ) { return "R49_Euro-5";         }
    else if ( val == STD_EU4     ) { return "R49_Euro-4";         }
    else if ( val == STD_EU3     ) { return "R49_Euro-3";         }
    else if ( val == STD_EU2     ) { return "R49_Euro-2";         }
    else if ( val == STD_EU1     ) { return "R49_Euro-1";         }
    else if ( val == STD_EU0     ) { return "R49_Euro-0";         }
    else if ( val == STD_OST     ) { return "OST_37.001.234-81";  }
    else if ( val == STD_GOST    ) { return "GOST_17.2.2.05-97";  }
    else if ( val == STD_R96E8   ) { return "R96_E8";             }
    else if ( val == STD_R96F8   ) { return "R96_F8";             }
    else if ( val == STD_R96G8   ) { return "R96_G8";             }
    else if ( val == STD_R96D8   ) { return "R96_D8";             }
    else if ( val == STD_R96E5   ) { return "R96_E5";             }
    else if ( val == STD_R96F5   ) { return "R96_F5";             }
    else if ( val == STD_R96G5   ) { return "R96_G5";             }
    else if ( val == STD_R96D5   ) { return "R96_D5";             }
    else if ( val == STD_R96H8   ) { return "R96_H8";             }
    else if ( val == STD_R96I8   ) { return "R96_I8";             }
    else if ( val == STD_R96J8   ) { return "R96_J8";             }
    else if ( val == STD_R96K8   ) { return "R96_K8";             }
    else if ( val == STD_R96H5   ) { return "R96_H5";             }
    else if ( val == STD_R96I5   ) { return "R96_I5";             }
    else if ( val == STD_R96J5   ) { return "R96_J5";             }
    else if ( val == STD_R96K5   ) { return "R96_K5";             }
    else if ( val == STD_C1      ) { return "GOST_R_51249-99_C1"; }
    else if ( val == STD_D1      ) { return "GOST_R_51249-99_D1"; }
    else if ( val == STD_D2      ) { return "GOST_R_51249-99_D2"; }
    else if ( val == STD_E1      ) { return "GOST_R_51249-99_E1"; }
    else if ( val == STD_E2      ) { return "GOST_R_51249-99_E2"; }
    else if ( val == STD_E3      ) { return "GOST_R_51249-99_E3"; }
    else if ( val == STD_E5      ) { return "GOST_R_51249-99_E5"; }
    else if ( val == STD_F       ) { return "GOST_R_51249-99_F";  }
    else if ( val == STD_G1      ) { return "GOST_R_51249-99_G1"; }
    else if ( val == STD_G2      ) { return "GOST_R_51249-99_G2"; }
    else if ( val == STD_FREECALC) { return "Free_Calculation";   }
    else                           { return "R49_Euro-4";         }
}

int txCalculationOptions::defTask(const QString &str) const {

    if ( str == QString::number(TASK_POINTS) ||
         str == "points" ) {
        return TASK_POINTS;
    }
    else if ( str == QString::number(TASK_EMISSIONS) ||
              str == "emissions" ) {
        return TASK_EMISSIONS;
    }
    else if ( str == QString::number(TASK_REDUCEDPOWER) ||
              str == "reducedPower" ) {
        return TASK_REDUCEDPOWER;
    }
    else if ( str == QString::number(TASK_ABCSPEEDS) ||
              str == "ABCspeeds" ) {
        return TASK_ABCSPEEDS;
    }
    else if ( str == QString::number(TASK_HELP) ||
              str == "help" ) {
        return TASK_HELP;
    }
    else {
        return TASK_EMISSIONS;
    }
}

int txCalculationOptions::defStandard(const QString &str) const {

    if ( str == QString::number(STD_EU6) ||
         str == "eu6" ) {
        return STD_EU6;
    }
    else if ( str == QString::number(STD_EU5) ||
              str == "eu5" ) {
        return STD_EU5;
    }
    else if ( str == QString::number(STD_EU4) ||
              str == "eu4" ) {
        return STD_EU4;
    }
    else if ( str == QString::number(STD_EU3) ||
              str == "eu3" ) {
        return STD_EU3;
    }
    else if ( str == QString::number(STD_EU2) ||
              str == "eu2" ) {
        return STD_EU2;
    }
    else if ( str == QString::number(STD_EU1) ||
              str == "eu1" ) {
        return STD_EU1;
    }
    else if ( str == QString::number(STD_EU0) ||
              str == "eu0" ) {
        return STD_EU0;
    }
    else if ( str == QString::number(STD_OST) ||
              str == "ost" ) {
        return STD_OST;
    }
    else if ( str == QString::number(STD_GOST) ||
              str == "gost" ) {
        return STD_GOST;
    }
    else if ( str == QString::number(STD_R96E8) ||
              str == "r96e8" ) {
        return STD_R96E8;
    }
    else if ( str == QString::number(STD_R96F8) ||
              str == "r96f8" ) {
        return STD_R96F8;
    }
    else if ( str == QString::number(STD_R96G8) ||
              str == "r96g8" ) {
        return STD_R96G8;
    }
    else if ( str == QString::number(STD_R96D8) ||
              str == "r96d8" ) {
        return STD_R96D8;
    }
    else if ( str == QString::number(STD_R96E5) ||
              str == "r96e5" ) {
        return STD_R96E5;
    }
    else if ( str == QString::number(STD_R96F5) ||
              str == "r96f5" ) {
        return STD_R96F5;
    }
    else if ( str == QString::number(STD_R96G5) ||
              str == "r96g5" ) {
        return STD_R96G5;
    }
    else if ( str == QString::number(STD_R96D5) ||
              str == "r96d5" ) {
        return STD_R96D5;
    }
    else if ( str == QString::number(STD_R96H8) ||
              str == "r96h8" ) {
        return STD_R96H8;
    }
    else if ( str == QString::number(STD_R96I8) ||
              str == "r96i8" ) {
        return STD_R96I8;
    }
    else if ( str == QString::number(STD_R96J8) ||
              str == "r96j8" ) {
        return STD_R96J8;
    }
    else if ( str == QString::number(STD_R96K8) ||
              str == "r96k8" ) {
        return STD_R96K8;
    }
    else if ( str == QString::number(STD_R96H5) ||
              str == "r96h5" ) {
        return STD_R96H5;
    }
    else if ( str == QString::number(STD_R96I5) ||
              str == "r96i5" ) {
        return STD_R96I5;
    }
    else if ( str == QString::number(STD_R96J5) ||
              str == "r96j5" ) {
        return STD_R96J5;
    }
    else if ( str == QString::number(STD_R96K5) ||
              str == "r96k5" ) {
        return STD_R96K5;
    }
    else if ( str == QString::number(STD_C1) ||
              str == "c1" ) {
        return STD_C1;
    }
    else if ( str == QString::number(STD_D1) ||
              str == "d1" ) {
        return STD_D1;
    }
    else if ( str == QString::number(STD_D2) ||
              str == "d2" ) {
        return STD_D2;
    }
    else if ( str == QString::number(STD_E1) ||
              str == "e1" ) {
        return STD_E1;
    }
    else if ( str == QString::number(STD_E2) ||
              str == "e2" ) {
        return STD_E2;
    }
    else if ( str == QString::number(STD_E3) ||
              str == "e3" ) {
        return STD_E3;
    }
    else if ( str == QString::number(STD_E5) ||
              str == "e5" ) {
        return STD_E5;
    }
    else if ( str == QString::number(STD_F) ||
              str == "f" ) {
        return STD_F;
    }
    else if ( str == QString::number(STD_G1) ||
              str == "g1" ) {
        return STD_G1;
    }
    else if ( str == QString::number(STD_G2) ||
              str == "g2" ) {
        return STD_G2;
    }
    else if ( str == QString::number(STD_FREECALC) ||
              str == "freeCalc" ) {
        return STD_FREECALC;
    }
    else {
        return STD_EU4;
    }
}

int txCalculationOptions::defChargingType(const QString &str) const {

    if ( str == QString::number(CHARGINGTYPE_NO) ||
         str == "noOrMechanical" ) {
        return CHARGINGTYPE_NO;
    }
    else if ( str == QString::number(CHARGINGTYPE_GASTURBINE) ||
              str == "gasTurbine" ) {
        return CHARGINGTYPE_GASTURBINE;
    }
    else {
        return CHARGINGTYPE_GASTURBINE;
    }
}

int txCalculationOptions::defFuelType(const QString &str) const {

    if ( str == QString::number(FUELTYPE_DIESEL) ||
         str == "diesel" ) {
        return FUELTYPE_DIESEL;
    }
    else if ( str == QString::number(FUELTYPE_MOTOR ) ||
              str == "motor" ) {
        return FUELTYPE_MOTOR;
    }
    else if ( str == QString::number(FUELTYPE_MAZUT ) ||
              str == "mazut" ) {
        return FUELTYPE_MAZUT;
    }
    else {
        return FUELTYPE_DIESEL;
    }
}

int txCalculationOptions::defNOxSample(const QString &str) const {

    if ( str == QString::number(NOXSAMPLE_WET) ||
         str == "wet" ) {
        return NOXSAMPLE_WET;
    }
    else if ( str == QString::number(NOXSAMPLE_DRY) ||
              str == "dry" ) {
        return NOXSAMPLE_DRY;
    }
    else {
        return NOXSAMPLE_WET;
    }
}

int txCalculationOptions::defPTcalc(const QString &str) const {

    if ( str == QString::number(PTCALC_THROUGHSMOKE) ||
         str == "throughSmoke" ) {
        return PTCALC_THROUGHSMOKE;
    }
    else if ( str == QString::number(PTCALC_THROUGHPTMASS) ||
              str == "throughPTmass" ) {
        return PTCALC_THROUGHPTMASS;
    }
    else if ( str == QString::number(PTCALC_THROUGHSMOKEANDPTMASS) ||
              str == "throughSmokeAndPTmass" ) {
        return PTCALC_THROUGHSMOKEANDPTMASS;
    }
    else if ( str == QString::number(PTCALC_NO) ||
              str == "no" ) {
        return PTCALC_NO;
    }
    else {
        return PTCALC_NO;
    }
}

int txCalculationOptions::defAddPointsCalc(const QString &str) const {

    if ( str == QString::number(ADDPOINTSCALC_YES) ||
         str == "yes" ) {
        return ADDPOINTSCALC_YES;
    }
    else if ( str == QString::number(ADDPOINTSCALC_NO) ||
              str == "no" ) {
        return ADDPOINTSCALC_NO;
    }
    else {
        return ADDPOINTSCALC_NO;
    }
}

} // namespace toxic
