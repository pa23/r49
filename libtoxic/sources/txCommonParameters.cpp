/*
    libtoxic
    Calculation of modes and specific emissions for stationary
    diesel engine test cycles.

    File: txCommonParameters.cpp

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

#include "txCommonParameters.h"
#include "txError.h"

#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QIODevice>

namespace toxic {

txCommonParameters::txCommonParameters() :
    m_commonDirName("r49data"),
    m_srcFileNameEU3 ("r49data"
                      + QString(QDir::separator())
                      + "TempSourceData"
                      + QString(QDir::separator())
                      + "SourceData1-EU3456.dat"),
    m_srcFileNameEU0 ("r49data"
                      + QString(QDir::separator())
                      + "TempSourceData"
                      + QString(QDir::separator())
                      + "SourceData1-EU012-R96-GOST30574.dat"),
    m_srcFileNamePoints ("r49data"
                         + QString(QDir::separator())
                         + "TempSourceData"
                         + QString(QDir::separator())
                         + "SourceData2-CyclePoints.dat"),
    m_srcFileNameRedPwr ("r49data"
                         + QString(QDir::separator())
                         + "TempSourceData"
                         + QString(QDir::separator())
                         + "SourceData3-FullLoadCurve.dat"),
    m_reportsDirName ("r49data"
                      + QString(QDir::separator())
                      + "Reports"),
    m_Dn         (90.0),
    m_concO2air  (20.8),
    m_Rr         (287.3),
    m_L0         (14.35),
    m_L          (0.43),
    m_concCO2air (0.06),
    m_WH         (12.6),
    m_WO2        (0.4),
    m_WN         (0),
    m_muNO2      (46.0),
    m_muCO       (28.0),
    m_muCH       (13.85) {
}

void txCommonParameters::readConfigFile(const QString &cfgFileName) {

    createNeededFiles();

    QFile cfgFile1(cfgFileName);
    QFile cfgFile2(QDir::homePath() + QDir::separator() + cfgFileName);
    QFile cfgFile;

    if ( cfgFile1.exists() ) {
        cfgFile.setFileName(cfgFile1.fileName());
    }
    else if ( cfgFile2.exists() ) {
        cfgFile.setFileName(cfgFile2.fileName());
    }
    else {

        const QString myPreferences =
                "//\n// This is r49 configuration file. "
                "Parameter-Value delimeter is \"=\" symbol.\n// "
                "Text after \"//\" is comment.\n//\n\n"
                "// Temporary rewrited source data files\nsrc_file_name_EU3="
                + m_srcFileNameEU3
                + "\n"
                "src_file_name_EU0="
                + m_srcFileNameEU0
                + "\n"
                "src_file_name_points="
                + m_srcFileNamePoints
                + "\n"
                "src_file_name_redpwr="
                + m_srcFileNameRedPwr
                + "\n\n"
                "// Directory for calculation results\nreports_dir_name="
                + m_reportsDirName
                + "\n\n"
                "// Measuring nozzle diameter "
                "(engine inlet air mass flow calculation)\nDn="
                + QString::number(m_Dn)
                + "\n\n"
                "// Air oxygen concentration\nconc_O2air="
                + QString::number(m_concO2air)
                + "\n\n"
                "//\nRr="
                + QString::number(m_Rr)
                + "\n\n"
                "// Stoichiometric air/fuel ratio\nL0="
                + QString::number(m_L0)
                + "\n\n"
                "// Opacimeter effective base\nL="
                + QString::number(m_L)
                + "\n\n"
                "// Air carbon dioxide concentration\nconc_CO2air="
                + QString::number(m_concCO2air)
                + "\n\n"
                "// Fuel percentage\nWH="
                + QString::number(m_WH)
                + "\n"
                "WO2="
                + QString::number(m_WO2)
                + "\n"
                "WN="
                + QString::number(m_WN)
                + "\n\n"
                "// Molecular weights\nmuNO2="
                + QString::number(m_muNO2)
                + "\n"
                "muCO="
                + QString::number(m_muCO)
                + "\n"
                "muCH="
                + QString::number(m_muCH)
                + "\n";

        if ( cfgFile1.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            cfgFile1.write(myPreferences.toUtf8());
            cfgFile1.close();

            cfgFile.setFileName(cfgFile1.fileName());
        }
        else if ( cfgFile2.open(QIODevice::WriteOnly | QIODevice::Text) ) {

            cfgFile2.write(myPreferences.toUtf8());
            cfgFile2.close();

            cfgFile.setFileName(cfgFile2.fileName());
        }
        else {
            throw txError("Can not create config file!");
        }

        return;
    }

    if ( !cfgFile.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        throw txError("Can not open config file " + cfgFileName + "!");
    }

    QString s;
    QStringList elements;
    QRegExp rx("^//");

    while ( !cfgFile.atEnd() ) {

        s = cfgFile.readLine().trimmed();

        if ( !s.isEmpty() && !s.contains(rx) ) {

            elements = s.split("=");

            if ( elements[0] == "src_file_name_EU3" ) {
                m_srcFileNameEU3 = elements[1];
            }
            else if ( elements[0] == "src_file_name_EU0" ) {
                m_srcFileNameEU0 = elements[1];
            }
            else if ( elements[0] == "src_file_name_points" ) {
                m_srcFileNamePoints = elements[1];
            }
            else if ( elements[0] == "src_file_name_redpwr" ) {
                m_srcFileNameRedPwr = elements[1];
            }
            else if ( elements[0] == "reports_dir_name" ) {
                m_reportsDirName = elements[1];
            }
            else if ( elements[0] == "Dn" ) {
                m_Dn = (elements[1]).toDouble();
            }
            else if ( elements[0] == "conc_O2air" ) {
                m_concO2air = (elements[1]).toDouble();
            }
            else if ( elements[0] == "Rr" ) {
                m_Rr = (elements[1]).toDouble();
            }
            else if ( elements[0] == "L0" ) {
                m_L0 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "L" ) {
                m_L = (elements[1]).toDouble();
            }
            else if ( elements[0] == "conc_CO2air" ) {
                m_concCO2air = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WH" ) {
                m_WH = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WO2" ) {
                m_WO2 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "WN" ) {
                m_WN = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muNO2" ) {
                m_muNO2 = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muCO" ) {
                m_muCO = (elements[1]).toDouble();
            }
            else if ( elements[0] == "muCH" ) {
                m_muCH = (elements[1]).toDouble();
            }

            elements.clear();
        }
    }

    elements.clear();

    cfgFile.close();
}

void txCommonParameters::createNeededFiles() {

    QDir dir;
    QFileInfo fileInfo;
    QString absPath;

    if ( !dir.exists(m_commonDirName) ) {

        if ( !dir.mkdir(m_commonDirName) ) {
            throw txError("Can not create directory " + m_commonDirName);
        }
    }

    fileInfo.setFile(m_srcFileNameEU3);
    absPath = fileInfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {
            throw txError("Can not create directory " + absPath);
        }
    }

    fileInfo.setFile(m_srcFileNameEU0);
    absPath = fileInfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {
            throw txError("Can not create directory " + absPath);
        }
    }

    fileInfo.setFile(m_srcFileNamePoints);
    absPath = fileInfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {
            throw txError("Can not create directory " + absPath);
        }
    }

    fileInfo.setFile(m_srcFileNameRedPwr);
    absPath = fileInfo.absoluteDir().absolutePath();

    if ( !dir.exists(absPath) ) {

        if ( !dir.mkdir(absPath) ) {
            throw txError("Can not create directory " + absPath);
        }
    }

    if ( !dir.exists(m_reportsDirName) ) {

        if ( !dir.mkdir(m_reportsDirName) ) {
            throw txError("Can not create directory " + m_reportsDirName);
        }
    }
}

} // namespace toxic
