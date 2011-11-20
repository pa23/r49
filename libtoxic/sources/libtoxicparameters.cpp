/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: libtoxicparameters.cpp

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

#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "libtoxicconstants.h"
#include "toxicerror.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QRegExp>

LibtoxicParameters::LibtoxicParameters() :
    task           (TASK_EMISSIONS),
    Vh             (0),
    standard       (STD_EU4),
    chargingType   (CHARGINGTYPE_GASTURBINE),
    FuelType       (FUELTYPE_DIESEL),
    NOxSample      (NOXSAMPLE_WET),
    PTcalc         (PTCALC_NO),
    PTmass         (0),
    AddPointsCalc  (ADDPOINTSCALC_NO),
    CalcConfigFile ("_._") {
}

LibtoxicParameters::~LibtoxicParameters() {
}

LibtoxicParameters::LibtoxicParameters(const LibtoxicParameters &orig) {

    (void)(&orig);
}

LibtoxicParameters &LibtoxicParameters::operator =(const LibtoxicParameters &x) {

    (void)(&x);
    return *this;
}

void LibtoxicParameters::setTask(const ptrdiff_t &task_) {

    task = task_;
}

void LibtoxicParameters::setTask(const QString &task_) {

    task = defTask(task_);
}

void LibtoxicParameters::setVh(const double &Vh_) {

    Vh = Vh_;
}

void LibtoxicParameters::setStandard(const ptrdiff_t &standart_) {

    standard = standart_;
}

void LibtoxicParameters::setStandard(const QString &standart_) {

    standard = defStandard(standart_);
}

void LibtoxicParameters::setChargingType(const ptrdiff_t &chargingType_) {

    chargingType = chargingType_;
}

void LibtoxicParameters::setChargingType(const QString &chargingType_) {

    chargingType = defChargingType(chargingType_);
}

void LibtoxicParameters::setFuelType(const ptrdiff_t &FuelType_) {

    FuelType = FuelType_;
}

void LibtoxicParameters::setFuelType(const QString &FuelType_) {

    FuelType = defFuelType(FuelType_);
}

void LibtoxicParameters::setNOxSample(const ptrdiff_t &NOxSample_) {

    NOxSample = NOxSample_;
}

void LibtoxicParameters::setNOxSample(const QString &NOxSample_) {

    NOxSample = defNOxSample(NOxSample_);
}

void LibtoxicParameters::setPTcalc(const ptrdiff_t &PTcalc_) {

    PTcalc = PTcalc_;
}

void LibtoxicParameters::setPTcalc(const QString &PTcalc_) {

    PTcalc = defPTcalc(PTcalc_);
}

void LibtoxicParameters::setPTmass(const double &PTmass_) {

    PTmass = PTmass_;
}

void LibtoxicParameters::setAddPointsCalc(const ptrdiff_t &AddPointsCalc_) {

    AddPointsCalc = AddPointsCalc_;
}

void LibtoxicParameters::setAddPointsCalc(const QString &AddPointsCalc_) {

    AddPointsCalc = defAddPointsCalc(AddPointsCalc_);
}

void LibtoxicParameters::setCalcConfigFile(const QString &CalcConfigFile_) {

    CalcConfigFile = CalcConfigFile_;
}

void LibtoxicParameters::readCalcConfigFile(const QString &calcConfigFileName) {

    QFile calcConfigFile(calcConfigFileName);

    if ( !(calcConfigFile.open(QIODevice::ReadOnly)) ) {

        throw ToxicError("Can not open file " + calcConfigFileName + "!");
    }

    QString s;
    QStringList elements;
    QRegExp rx(COMMENTPATTERN);

    while ( !calcConfigFile.atEnd() ) {

        s = calcConfigFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) && (!s.contains(rx)) ) {

            elements = s.split(PARAMETERVALUEDELIMITER,
                               QString::SkipEmptyParts);

            if ( elements[0] == "task" ) {

                task = defTask(elements[1]);
            }
            else if ( elements[0] == "Vh" ) {

                Vh = (elements[1]).toDouble();
            }
            else if ( elements[0] == "standard" ) {

                standard = defStandard(elements[1]);
            }
            else if ( elements[0] == "ChargingType" ) {

                chargingType = defChargingType(elements[1]);
            }
            else if ( elements[0] == "FuelType" ) {

                FuelType = defFuelType(elements[1]);
            }
            else if ( elements[0] == "NOxSample" ) {

                NOxSample = defNOxSample(elements[1]);
            }
            else if ( elements[0] == "PTcalc" ) {

                PTcalc = defPTcalc(elements[1]);
            }
            else if ( elements[0] == "PTmass" ) {

                PTmass = (elements[1]).toDouble();
            }
            else if ( elements[0] == "AddPointsCalc" ) {

                AddPointsCalc = defAddPointsCalc(elements[1]);
            }
            else if ( elements[0] == "CalcConfigFile" ) {

                //
            }

            elements.clear();
        }
    }

    elements.clear();

    calcConfigFile.close();

    //

    CalcConfigFile = calcConfigFileName;
}

QString LibtoxicParameters::defStandardName(const ptrdiff_t &val) const {

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

ptrdiff_t LibtoxicParameters::defTask(const QString &str) const {

    if      ( str == QString::number(TASK_POINTS      ) ||
              str == "points"       ) { return TASK_POINTS;       }
    else if ( str == QString::number(TASK_EMISSIONS   ) ||
              str == "emissions"    ) { return TASK_EMISSIONS;    }
    else if ( str == QString::number(TASK_REDUCEDPOWER) ||
              str == "ReducedPower" ) { return TASK_REDUCEDPOWER; }
    else if ( str == QString::number(TASK_ABCSPEEDS   ) ||
              str == "ABCspeeds"    ) { return TASK_ABCSPEEDS;    }
    else if ( str == QString::number(TASK_HELP        ) ||
              str == "help"         ) { return TASK_HELP;         }
    else                              { return TASK_EMISSIONS;    }
}

ptrdiff_t LibtoxicParameters::defStandard(const QString &str) const {

    if      ( str == QString::number(STD_EU6     ) ||
              str == "EU6"      ) { return STD_EU6;      }
    else if ( str == QString::number(STD_EU5     ) ||
              str == "EU5"      ) { return STD_EU5;      }
    else if ( str == QString::number(STD_EU4     ) ||
              str == "EU4"      ) { return STD_EU4;      }
    else if ( str == QString::number(STD_EU3     ) ||
              str == "EU3"      ) { return STD_EU3;      }
    else if ( str == QString::number(STD_EU2     ) ||
              str == "EU2"      ) { return STD_EU2;      }
    else if ( str == QString::number(STD_EU1     ) ||
              str == "EU1"      ) { return STD_EU1;      }
    else if ( str == QString::number(STD_EU0     ) ||
              str == "EU0"      ) { return STD_EU0;      }
    else if ( str == QString::number(STD_OST     ) ||
              str == "OST"      ) { return STD_OST;      }
    else if ( str == QString::number(STD_GOST    ) ||
              str == "GOST"     ) { return STD_GOST;     }
    else if ( str == QString::number(STD_R96E8   ) ||
              str == "r96E8"    ) { return STD_R96E8;    }
    else if ( str == QString::number(STD_R96F8   ) ||
              str == "r96F8"    ) { return STD_R96F8;    }
    else if ( str == QString::number(STD_R96G8   ) ||
              str == "r96G8"    ) { return STD_R96G8;    }
    else if ( str == QString::number(STD_R96D8   ) ||
              str == "r96D8"    ) { return STD_R96D8;    }
    else if ( str == QString::number(STD_R96E5   ) ||
              str == "r96E5"    ) { return STD_R96E5;    }
    else if ( str == QString::number(STD_R96F5   ) ||
              str == "r96F5"    ) { return STD_R96F5;    }
    else if ( str == QString::number(STD_R96G5   ) ||
              str == "r96G5"    ) { return STD_R96G5;    }
    else if ( str == QString::number(STD_R96D5   ) ||
              str == "r96D5"    ) { return STD_R96D5;    }
    else if ( str == QString::number(STD_R96H8   ) ||
              str == "r96H8"    ) { return STD_R96H8;    }
    else if ( str == QString::number(STD_R96I8   ) ||
              str == "r96I8"    ) { return STD_R96I8;    }
    else if ( str == QString::number(STD_R96J8   ) ||
              str == "r96J8"    ) { return STD_R96J8;    }
    else if ( str == QString::number(STD_R96K8   ) ||
              str == "r96K8"    ) { return STD_R96K8;    }
    else if ( str == QString::number(STD_R96H5   ) ||
              str == "r96H5"    ) { return STD_R96H5;    }
    else if ( str == QString::number(STD_R96I5   ) ||
              str == "r96I5"    ) { return STD_R96I5;    }
    else if ( str == QString::number(STD_R96J5   ) ||
              str == "r96J5"    ) { return STD_R96J5;    }
    else if ( str == QString::number(STD_R96K5   ) ||
              str == "r96K5"    ) { return STD_R96K5;    }
    else if ( str == QString::number(STD_C1      ) ||
              str == "C1"       ) { return STD_C1;       }
    else if ( str == QString::number(STD_D1      ) ||
              str == "D1"       ) { return STD_D1;       }
    else if ( str == QString::number(STD_D2      ) ||
              str == "D2"       ) { return STD_D2;       }
    else if ( str == QString::number(STD_E1      ) ||
              str == "E1"       ) { return STD_E1;       }
    else if ( str == QString::number(STD_E2      ) ||
              str == "E2"       ) { return STD_E2;       }
    else if ( str == QString::number(STD_E3      ) ||
              str == "E3"       ) { return STD_E3;       }
    else if ( str == QString::number(STD_E5      ) ||
              str == "E5"       ) { return STD_E5;       }
    else if ( str == QString::number(STD_F       ) ||
              str == "F"        ) { return STD_F;        }
    else if ( str == QString::number(STD_G1      ) ||
              str == "G1"       ) { return STD_G1;       }
    else if ( str == QString::number(STD_G2      ) ||
              str == "G2"       ) { return STD_G2;       }
    else if ( str == QString::number(STD_FREECALC) ||
              str == "FreeCalc" ) { return STD_FREECALC; }
    else                          { return STD_EU4;      }
}

ptrdiff_t LibtoxicParameters::defChargingType(const QString &str) const {

    if      ( str == QString::number(CHARGINGTYPE_NO)         ||
              str == "NoOrMechanical" ) { return CHARGINGTYPE_NO;         }
    else if ( str == QString::number(CHARGINGTYPE_GASTURBINE) ||
              str == "GasTurbine"     ) { return CHARGINGTYPE_GASTURBINE; }
    else                                { return CHARGINGTYPE_GASTURBINE; }
}

ptrdiff_t LibtoxicParameters::defFuelType(const QString &str) const {

    if      ( str == QString::number(FUELTYPE_DIESEL) ||
              str == "diesel" ) { return FUELTYPE_DIESEL; }
    else if ( str == QString::number(FUELTYPE_MOTOR ) ||
              str == "motor"  ) { return FUELTYPE_MOTOR;  }
    else if ( str == QString::number(FUELTYPE_MAZUT ) ||
              str == "mazut"  ) { return FUELTYPE_MAZUT;  }
    else                        { return FUELTYPE_DIESEL; }
}

ptrdiff_t LibtoxicParameters::defNOxSample(const QString &str) const {

    if      ( str == QString::number(NOXSAMPLE_WET) ||
              str == "wet" ) { return NOXSAMPLE_WET; }
    else if ( str == QString::number(NOXSAMPLE_DRY) ||
              str == "dry" ) { return NOXSAMPLE_DRY; }
    else                     { return NOXSAMPLE_WET; }
}

ptrdiff_t LibtoxicParameters::defPTcalc(const QString &str) const {

    if      ( str == QString::number(PTCALC_THROUGHSMOKE ) ||
              str == "ThroughSmoke"  ) { return PTCALC_THROUGHSMOKE;  }
    else if ( str == QString::number(PTCALC_THROUGHPTMASS) ||
              str == "ThroughPTmass" ) { return PTCALC_THROUGHPTMASS; }
    else if ( str == QString::number(PTCALC_NO           ) ||
              str == "no"            ) { return PTCALC_NO;            }
    else                               { return PTCALC_NO;            }
}

ptrdiff_t LibtoxicParameters::defAddPointsCalc(const QString &str) const {

    if      ( str == QString::number(ADDPOINTSCALC_YES) ||
              str == "yes" ) { return ADDPOINTSCALC_YES; }
    else if ( str == QString::number(ADDPOINTSCALC_NO ) ||
              str == "no"  ) { return ADDPOINTSCALC_NO;  }
    else                     { return ADDPOINTSCALC_NO;  }
}
