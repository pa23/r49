/*
    libtoxicparameters.cpp

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

#include "libtoxicconstants.h"
#include "libtoxicparameters.h"
#include "libtoxicconstants.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QRegExp>

LibtoxicParameters::LibtoxicParameters() :
        task           ("emissions"   ),
        Vh             (0             ),
        standard       ("EU4"         ),
        FuelType       ("diesel"      ),
        NOxSample      ("wet"         ),
        PTcalc         ("ThroughSmoke"),
        PTmass         (0             ),
        AddPointsCalc  ("no"          ),
        CalcConfigFile ("_._"         ) {
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

void LibtoxicParameters::setTask           (QString  task_)           { task           =  task_;           }
void LibtoxicParameters::setVh             (double  *Vh_)             { Vh             = *Vh_;             }
void LibtoxicParameters::setStandard       (QString  standart_)       { standard       =  standart_;       }
void LibtoxicParameters::setFuelType       (QString  FuelType_)       { FuelType       =  FuelType_;       }
void LibtoxicParameters::setNOxSample      (QString  NOxSample_)      { NOxSample      =  NOxSample_;      }
void LibtoxicParameters::setPTcalc         (QString  PTcalc_)         { PTcalc         =  PTcalc_;         }
void LibtoxicParameters::setPTmass         (double  *PTmass_)         { PTmass         = *PTmass_;         }
void LibtoxicParameters::setAddPointsCalc  (QString  AddPointsCalc_)  { AddPointsCalc  =  AddPointsCalc_;  }
void LibtoxicParameters::setCalcConfigFile (QString  CalcConfigFile_) { CalcConfigFile =  CalcConfigFile_; }

QString LibtoxicParameters::val_Task           () const { return task;           }
double  LibtoxicParameters::val_Vh             () const { return Vh;             }
QString LibtoxicParameters::val_Standard       () const { return standard;       }
QString LibtoxicParameters::val_FuelType       () const { return FuelType;       }
QString LibtoxicParameters::val_NOxSample      () const { return NOxSample;      }
QString LibtoxicParameters::val_PTcalc         () const { return PTcalc;         }
double  LibtoxicParameters::val_PTmass         () const { return PTmass;         }
QString LibtoxicParameters::val_AddPointsCalc  () const { return AddPointsCalc;  }
QString LibtoxicParameters::val_CalcConfigFile () const { return CalcConfigFile; }

bool LibtoxicParameters::readCalcConfigFile(QString calcConfigFileName) {

    QFile calcConfigFile(calcConfigFileName);

    if ( !(calcConfigFile.open(QIODevice::ReadOnly)) ) {

        qDebug() << "libtoxic ERROR: LibtoxicParameters: readCalcConfigFile:" << calcConfigFileName << "not found!";

        return false;
    }

    QString s;
    QStringList elements;
    QRegExp rx(commentPattern);

    while ( !calcConfigFile.atEnd() ) {

        s = calcConfigFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) && (!s.contains(rx)) ) {

            elements = s.split(parameterValueDelimiter, QString::SkipEmptyParts);

            if      (elements[0] == "task")           { task           = elements[1]; }
            else if (elements[0] == "Vh")             { Vh             = (elements[1]).toDouble(); }
            else if (elements[0] == "standard")       { standard       = elements[1]; }
            else if (elements[0] == "FuelType")       { FuelType       = elements[1]; }
            else if (elements[0] == "NOxSample")      { NOxSample      = elements[1]; }
            else if (elements[0] == "PTcalc")         { PTcalc         = elements[1]; }
            else if (elements[0] == "PTmass")         { PTmass         = (elements[1]).toDouble(); }
            else if (elements[0] == "AddPointsCalc")  { AddPointsCalc  = elements[1]; }
            else if (elements[0] == "CalcConfigFile") { CalcConfigFile = elements[1]; }
            else                                      {

                qDebug() << "libtoxic WARNING: LibtoxicParameters: readCalcConfigFile: unknown parameter" << elements[0] << "!";
            }

            elements.clear();
        }
    }

    elements.clear();

    calcConfigFile.close();

    return true;
}
