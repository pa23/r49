/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: commonparameters.cpp

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

#include "commonparameters.h"
#include "libtoxicconstants.h"
#include "toxicerror.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QIODevice>
#include <QTextStream>

CommonParameters::CommonParameters() :
    fileNameSourceEU3 ("r49_TempSourceData/SourceData1-EU3456.csv"),
    fileNameSourceEU0 ("r49_TempSourceData/SourceData1-EU012-r96-GOST30574.csv"),
    fileNamePoints    ("r49_TempSourceData/SourceData2-CyclePoints.csv"),
    fileNamePowers    ("r49_TempSourceData/SourceData3-FullLoadCurve.csv"),
    dirNameReports    ("r49_Reports"),
    Dn         (90.0),
    concO2air  (20.8),
    Rr         (287.3),
    L0         (14.35),
    L          (0.43),
    concCO2air (0.06),
    WH         (12.6),
    WO2        (0.4),
    WN         (0),
    muNO2      (46.0),
    muCO       (28.0),
    muCH       (13.85) {
}

CommonParameters::~CommonParameters() {
}

void CommonParameters::readConfigFile(const QString &configFileName) {

    QFile configFile1(configFileName);
    QFile configFile2(QDir::homePath()
                      + QDir::separator()
                      + configFileName);
    QFile configFile;

    if ( configFile1.exists() ) {

        configFile.setFileName(configFile1.fileName());
    }
    else if ( configFile2.exists() ) {

        configFile.setFileName(configFile2.fileName());
    }
    else {

        const QString myPreferences =
                "//\n// This is r49 configuration file. "
                "Parameter-Value delimeter is \"=\" symbol.\n// "
                "Text after \"//\" is comment.\n//\n\n"
                "// Temporary rewrited source data files\nfileNameSourceEU3="
                + fileNameSourceEU3
                + "\n"
                "fileNameSourceEU0="
                + fileNameSourceEU0
                + "\n"
                "fileNamePoints="
                + fileNamePoints
                + "\n"
                "fileNamePowers="
                + fileNamePowers
                + "\n\n"
                "// Directory for calculation results\ndirNameReports="
                + dirNameReports
                + "\n\n"
                "// Measuring nozzle diameter "
                "(engine inlet air mass flow calculation)\nDn="
                + QString::number(Dn)
                + "\n\n"
                "// Air oxygen concentration\nconcO2air="
                + QString::number(concO2air)
                + "\n\n"
                "//\nRr="
                + QString::number(Rr)
                + "\n\n"
                "// Stoichiometric air/fuel ratio\nL0="
                + QString::number(L0)
                + "\n\n"
                "// Opacimeter effective base\nL="
                + QString::number(L)
                + "\n\n"
                "// Air carbon dioxide concentration\nconcCO2air="
                + QString::number(concCO2air)
                + "\n\n"
                "// Fuel percentage\nWH="
                + QString::number(WH)
                + "\n"
                "WO2="
                + QString::number(WO2)
                + "\n"
                "WN="
                + QString::number(WN)
                + "\n\n"
                "// Molecular weights\nmuNO2="
                + QString::number(muNO2)
                + "\n"
                "muCO="
                + QString::number(muCO)
                + "\n"
                "muCH="
                + QString::number(muCH)
                + "\n";

        if ( configFile1.open(QIODevice::WriteOnly) ) {

            QTextStream in(&configFile1);
            in << myPreferences;

            configFile1.close();

            configFile.setFileName(configFile1.fileName());
        }
        else if ( configFile2.open(QIODevice::WriteOnly) ) {

            QTextStream in(&configFile2);
            in << myPreferences;

            configFile2.close();

            configFile.setFileName(configFile2.fileName());
        }
        else {

            throw ToxicError("Can not create config file!");
        }
    }

    if ( !(configFile.open(QIODevice::ReadOnly)) ) {

        throw ToxicError("Can not open config file " + configFileName + "!");
    }

    QString s;
    QStringList elements;
    QRegExp rx(COMMENTPATTERN);

    while ( !configFile.atEnd() ) {

        s = configFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) && (!s.contains(rx)) ) {

            elements = s.split(PARAMETERVALUEDELIMITER,
                               QString::SkipEmptyParts);

            if ( elements[0] == "fileNameSourceEU3" ) {

                fileNameSourceEU3 = elements[1];
            }
            else if ( elements[0] == "fileNameSourceEU0" ) {

                fileNameSourceEU0 = elements[1];
            }
            else if ( elements[0] == "fileNamePoints" ) {

                fileNamePoints = elements[1];
            }
            else if ( elements[0] == "fileNamePowers" ) {

                fileNamePowers = elements[1];
            }
            else if ( elements[0] == "dirNameReports" ) {

                dirNameReports = elements[1];
            }
            else if ( elements[0] == "Dn" ) {

                Dn = (elements[1]).toDouble();
            }
            else if ( elements[0] == "concO2air" ) {

                concO2air = (elements[1]).toDouble();
            }
            else if ( elements[0] == "Rr" ) {

                Rr = (elements[1]).toDouble();
            }
            else if ( elements[0] == "L0" ) {

                L0 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "L" ) {

                L = (elements[1]).toDouble();
            }
            else if ( elements[0] == "concCO2air" ) {

                concCO2air = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WH" ) {

                WH = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WO2" ) {

                WO2 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WN" ) {

                WN = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muNO2" ) {

                muNO2 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muCO" ) {

                muCO = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muCH" ) {

                muCH = (elements[1]).toDouble();
            }

            elements.clear();
        }
    }

    elements.clear();

    configFile.close();

    //

    QDir dir;
    QFileInfo fileinfo;
    QString absPath;

    fileinfo.setFile(fileNameSourceEU3);
    absPath = fileinfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {

            throw ToxicError("Can not create directory " + absPath);
        }
    }

    fileinfo.setFile(fileNameSourceEU0);
    absPath = fileinfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {

            throw ToxicError("Can not create directory " + absPath);
        }
    }

    fileinfo.setFile(fileNamePoints);
    absPath = fileinfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {

            throw ToxicError("Can not create directory " + absPath);
        }
    }

    fileinfo.setFile(fileNamePowers);
    absPath = fileinfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {

            throw ToxicError("Can not create directory " + absPath);
        }
    }

    if ( !dir.exists(dirNameReports) ) {

        if ( !dir.mkdir(dirNameReports) ) {

            throw ToxicError("Can not create directory " + dirNameReports);
        }
    }
}
