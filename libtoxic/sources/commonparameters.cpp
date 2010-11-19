/*
    commonparameters.cpp

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

#include "commonparameters.h"
#include "libtoxicconstants.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QIODevice>

CommonParameters::CommonParameters() : csvDelimiter      (";"),
                                       filenameSourceEU3 ("TempSourceData/SourceData1-EU3456.csv"),
                                       filenameSourceEU0 ("TempSourceData/SourceData1-EU012-r96-GOST30574.csv"),
                                       filenamePoints    ("TempSourceData/SourceData2-CyclePoints.csv"),
                                       filenamePowers    ("TempSourceData/SourceData3-FullLoadCurve.csv"),
                                       dirnameReports    ("Reports"),
                                       Dn         (90.0),
                                       ConcO2air  (20.8),
                                       Rr         (287.3),
                                       L0         (14.35),
                                       L          (0.43),
                                       ConcCO2air (0.06),
                                       WH         (12.6),
                                       WO2        (0.4),
                                       WN         (0),
                                       roAir      (1.184),
                                       muNO2      (46.0),
                                       muCO       (28.0),
                                       muCH       (13.85) {
}

CommonParameters::~CommonParameters() {
}

CommonParameters::CommonParameters(const CommonParameters &orig) {

    (void)(&orig);
}

CommonParameters &CommonParameters::operator =(const CommonParameters &x) {

    (void)(&x);
    return *this;
}

bool CommonParameters::ReadConfigFile(QString configFileName) {

    QFile configFile(configFileName);

    if ( !(configFile.open(QIODevice::ReadOnly)) ) {

        qDebug() << "libtoxic ERROR: CommonParameters: ReadConfigFile:" << configFileName << "not found!";

        return false;
    }

    QString s;
    QStringList elements;
    QRegExp rx(commentPattern);

    while ( !configFile.atEnd() ) {

        s = configFile.readLine().trimmed();

        if ( (!s.isEmpty()) && (!s.isNull()) && (!s.contains(rx)) ) {

            elements = s.split(parameterValueDelimiter, QString::SkipEmptyParts);

            if      (elements[0] == "csvdelimiter")      { csvDelimiter      = elements[1]; }
            else if (elements[0] == "filenameSourceEU3") { filenameSourceEU3 = elements[1]; }
            else if (elements[0] == "filenameSourceEU0") { filenameSourceEU0 = elements[1]; }
            else if (elements[0] == "filenamePoints")    { filenamePoints    = elements[1]; }
            else if (elements[0] == "filenamePowers")    { filenamePowers    = elements[1]; }
            else if (elements[0] == "dirnameReports")    { dirnameReports    = elements[1]; }
            else if (elements[0] == "Dn")                { Dn                = (elements[1]).toDouble(); }
            else if (elements[0] == "ConcO2air")         { ConcO2air         = (elements[1]).toDouble(); }
            else if (elements[0] == "Rr")                { Rr                = (elements[1]).toDouble(); }
            else if (elements[0] == "L0")                { L0                = (elements[1]).toDouble(); }
            else if (elements[0] == "L")                 { L                 = (elements[1]).toDouble(); }
            else if (elements[0] == "ConcCO2air")        { ConcCO2air        = (elements[1]).toDouble(); }
            else if (elements[0] == "WH")                { WH                = (elements[1]).toDouble(); }
            else if (elements[0] == "WO2")               { WO2               = (elements[1]).toDouble(); }
            else if (elements[0] == "WN")                { WN                = (elements[1]).toDouble(); }
            else if (elements[0] == "roAir")             { roAir             = (elements[1]).toDouble(); }
            else if (elements[0] == "muNO2")             { muNO2             = (elements[1]).toDouble(); }
            else if (elements[0] == "muCO")              { muCO              = (elements[1]).toDouble(); }
            else if (elements[0] == "muCH")              { muCH              = (elements[1]).toDouble(); }
            else                                         {

                qDebug() << "libtoxic WARNING: CommonParameters: ReadConfigFile: unknown parameter" << elements[0] << "!";
            }

            elements.clear();
        }
    }

    elements.clear();

    configFile.close();

    //

    QDir dir;
    QFileInfo fileinfo;

    fileinfo.setFile(filenameSourceEU3);

    if (!dir.exists(fileinfo.absoluteDir().absolutePath())) {

        dir.mkdir(fileinfo.absoluteDir().absolutePath());
    }

    fileinfo.setFile(filenameSourceEU0);

    if (!dir.exists(fileinfo.absoluteDir().absolutePath())) {

        dir.mkdir(fileinfo.absoluteDir().absolutePath());
    }

    fileinfo.setFile(filenamePoints);

    if (!dir.exists(fileinfo.absoluteDir().absolutePath())) {

        dir.mkdir(fileinfo.absoluteDir().absolutePath());
    }

    fileinfo.setFile(filenamePowers);

    if (!dir.exists(fileinfo.absoluteDir().absolutePath())) {

        dir.mkdir(fileinfo.absoluteDir().absolutePath());
    }

    if (!dir.exists(dirnameReports)) {

        dir.mkdir(dirnameReports);
    }

    //

    return true;
}

QString CommonParameters::Get_csvDelimiter      () const { return csvDelimiter;      }
QString CommonParameters::Get_filenameSourceEU3 () const { return filenameSourceEU3; }
QString CommonParameters::Get_filenameSourceEU0 () const { return filenameSourceEU0; }
QString CommonParameters::Get_filenamePoints    () const { return filenamePoints;    }
QString CommonParameters::Get_filenamePowers    () const { return filenamePowers;    }
QString CommonParameters::Get_dirnameReports    () const { return dirnameReports;    }
double CommonParameters::Get_Dn                 () const { return Dn;                }
double CommonParameters::Get_ConcO2air          () const { return ConcO2air;         }
double CommonParameters::Get_Rr                 () const { return Rr;                }
double CommonParameters::Get_L0                 () const { return L0;                }
double CommonParameters::Get_L                  () const { return L;                 }
double CommonParameters::Get_ConcCO2air         () const { return ConcCO2air;        }
double CommonParameters::Get_WH                 () const { return WH;                }
double CommonParameters::Get_WO2                () const { return WO2;               }
double CommonParameters::Get_WN                 () const { return WN;                }
double CommonParameters::Get_roAir              () const { return roAir;             }
double CommonParameters::Get_muNO2              () const { return muNO2;             }
double CommonParameters::Get_muCO               () const { return muCO;              }
double CommonParameters::Get_muCH               () const { return muCH;              }
